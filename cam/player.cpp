#include "player.h"
#include "rsa.h"

#include <stdio.h>
#include <windows.h>

Player::Player(){
	m_timeAlignment = 0;
	m_timeTotal = 0;
	m_timeCurrent = 0;
	m_timeSleep = 0;
	m_nextPacket = 0;
	
	m_speed = 1.0;
	m_running = false;
	m_online = false;
	m_info = false;
	m_fastForward = false;
	
	m_socket = INVALID_SOCKET;
}

Player::~Player(){
	{
		ScopedLock lock(m_runningLock);
		m_running = false;
	}
	
	ScopedLock lock(m_threadLock);
	
	// It is now possible to safely free messages - THIS FUNCTION IS THREAD UNSAFE
	free();
}

void Player::sendNetworkMessage(SOCKET m_client, NetworkMessage& output){
	output.writeMessageLength();
	
	{
		ScopedLock lock(m_xteaLock);
		XTEA_encrypt(output, m_xtea);
	}
	
	send(m_client, output.getBuffer(), (*(uint16_t*)output.getBuffer()) + 2, 0);
}

void Player::sendClientMessage(SOCKET m_client, std::stringstream& msg){
	NetworkMessage output(NetworkMessage::XTEA);
	
	output.AddByte(0xB4);
	output.AddByte(0x12);
	output.AddString(msg.str());
	
	sendNetworkMessage(m_client, output);
}

void Player::sendClientInfo(SOCKET m_client){
	std::stringstream information;
	information << "Welcome to Tibia CAM player, command set:" << std::endl;
	information << "speed x - sets playback speed to x value" << std::endl;
	information << "time - displays current and total time of the movie" << std::endl;
	information << "ff hh:mm:ss - Fast forward till given time" << std::endl;
	information << "help, info - displays this message" << std::endl;
	information << std::endl;
	information << "Total movie length: " << MilisecondsToStr(getTimeTotal()) << std::endl;
	
	sendClientMessage(m_client, information);
}

DWORD WINAPI Player::Server(LPVOID lpParam){
	Player* player = reinterpret_cast<Player*>(lpParam);
	ScopedLock lock(player->m_threadLock);
	
	{
		ScopedLock lock(player->m_runningLock);
		player->m_running = true;
	}
	
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	while(true){
		// It seems like user requested to shutdown the cam player
		if(!player->running()){
			if(player->m_socket != INVALID_SOCKET){
				closesocket(player->m_socket);
				player->m_socket = INVALID_SOCKET;
			}
			
			break;
		}
		
		if(player->m_socket == INVALID_SOCKET){
			sockaddr_in local;
			
			local.sin_family = AF_INET;
			local.sin_addr.s_addr = INADDR_ANY;
			local.sin_port = htons(CAM_PLAYER_PORT);
			
			player->m_socket = socket(AF_INET, SOCK_STREAM, 0);
			if(player->m_socket == INVALID_SOCKET){
				ScopedLock lock(player->m_statusLock);
				player->m_status = std::string("Cam Player: Failed to create socket!");
				break;
			}
			
			if(bind(player->m_socket, (sockaddr*)&local, sizeof(local)) != 0){
				closesocket(player->m_socket);
				player->m_socket = INVALID_SOCKET;
				
				ScopedLock lock(player->m_statusLock);
				player->m_status = std::string("Cam Player: Failed while binding server to a port!");
				break;
			}
			
			if(listen(player->m_socket, SOMAXCONN) != 0){
				closesocket(player->m_socket);
				player->m_socket = INVALID_SOCKET;
				
				ScopedLock lock(player->m_statusLock);
				player->m_status = std::string("Cam Player: Failed to listen on port!");
				break;
			}
		}
		
		// Player can only host cam to one client at the time
		SOCKET m_client = INVALID_SOCKET;
		
		fd_set conn;
		FD_ZERO(&conn);
		FD_SET(player->m_socket, &conn);
		
		if(select(0, &conn, NULL, NULL, &timeout) > 0){
			sockaddr_in from;
			int fromlen = sizeof(from);
			m_client = accept(player->m_socket, (struct sockaddr*)&from, &fromlen);
			 
			if(m_client == INVALID_SOCKET){
				closesocket(player->m_socket);
				player->m_socket = INVALID_SOCKET;
					
				ScopedLock lock(player->m_statusLock);
				player->m_status = std::string("Cam Player: Failed to accept client");
				break;
			}
		}
		
		while(m_client != INVALID_SOCKET){
			if(!player->running()){
				closesocket(m_client);
				closesocket(player->m_socket);
				
				m_client = INVALID_SOCKET;
				player->m_socket = INVALID_SOCKET;
				
				break;
			}
		
			fd_set conn;
			FD_ZERO(&conn);
			FD_SET(m_client, &conn);
		
			if(select(0, &conn, NULL, NULL, &timeout) > 0){ // some packet is waiting to be read
				static char buffer[NETWORKMESSAGE_MAXSIZE];
				static uint16_t& plen = *(uint16_t*)buffer;
				
				int n = 0;
				while(n != NetworkMessage::header_length){
					int m = recv(m_client, buffer + n, NetworkMessage::header_length - n, 0);
					if(m <= 0){
						closesocket(m_client);
						m_client = INVALID_SOCKET;
						break;
					}
					
					n = n + m;
				}
					
				while(n != plen + NetworkMessage::header_length){
					int m = recv(m_client, buffer + n, plen - (n - NetworkMessage::header_length), 0);
					if(m <= 0){
						closesocket(m_client);
						m_client = INVALID_SOCKET;
						break;
					}
					
					n = n + m;
				}
				
				// Apparently the message is full so lets parse it
				if(n == plen + NetworkMessage::header_length){
					NetworkMessage msg((uint8_t*)buffer);
					
					// If the player is not online we expect the first packet to be either login packet or game packet
					if(!player->m_online){
						uint8_t pid = msg.GetByte();
						switch(pid){
							case 0x01: {
								uint16_t clientos = msg.GetU16();
								uint16_t version  = msg.GetU16();
								
								msg.SkipBytes(12);
								
								if(!g_rsa->decrypt(msg)){
									ScopedLock lock(player->m_statusLock);
									player->m_status = std::string("Cam Player: Failed to decrypt RSA");
									break;
								}		
								
								///* main XTEA key */
								{
									ScopedLock lock(player->m_xteaLock);
									player->m_xtea[0] = msg.GetU32();
									player->m_xtea[1] = msg.GetU32();
									player->m_xtea[2] = msg.GetU32();
									player->m_xtea[3] = msg.GetU32();
								}
								
								uint32_t accname = msg.GetU32();
								if(accname){
									NetworkMessage output(NetworkMessage::XTEA);
									output.AddByte(0x0A);
									output.AddString("Cam Player: Please do not type any account number or password.");
									player->sendNetworkMessage(m_client, output);
									break;
								}
								
								std::vector<std::string> characters;
								
								DirectoryReader d(CAM_DIR);
								
								char filename[MAX_PATH];
								while(d.getNextFile(filename)){
									std::string extension = std::string(filename);
									extension = extension.substr(extension.find_last_of('.') + 1);
									
									if(extension.compare(CAM_EXTENSION) == 0){
										characters.push_back(filename);
									}
								}
								
								NetworkMessage output(NetworkMessage::XTEA);
								//Add char list
								output.AddByte(0x64);
								output.AddByte(characters.size());
								for(int i = 0; i < characters.size(); i++){
									output.AddString(characters[i]);
									output.AddString("CAM");
									output.AddU32(16777343);
									output.AddU16(7171);
								}
								//Add premium days
								output.AddU16(0xFFFF);
								
								player->sendNetworkMessage(m_client, output);
								
								break;
							}
							
							case 0x0A: {
								uint16_t clientos = msg.GetU16();
								uint16_t version  = msg.GetU16();
								
								if(!g_rsa->decrypt(msg)){
									ScopedLock lock(player->m_statusLock);
									player->m_status = std::string("Cam Player: Failed to decrypt RSA");
									break;
								}
								
								///* main XTEA key */
								{
									ScopedLock lock(player->m_xteaLock);
									player->m_xtea[0] = msg.GetU32();
									player->m_xtea[1] = msg.GetU32();
									player->m_xtea[2] = msg.GetU32();
									player->m_xtea[3] = msg.GetU32();
								}
								
								bool isSetGM = (msg.GetByte() == 1);
								uint32_t accname = msg.GetU32();
								const std::string name = msg.GetString();
								const std::string password = msg.GetString();
								
								if(!player->load(std::string(CAM_DIR) + std::string("/") + name)){
									NetworkMessage output(NetworkMessage::XTEA);
									output.AddByte(0x0A);
									output.AddString("Cam Player: Failed to load file!");
									player->sendNetworkMessage(m_client, output);
									break;
								}
								
								player->m_timeCurrent = Timer::tickCount();
								player->setNextPacket(0);
                player->setSpeed(1.0);
								player->m_timeForward = 0;
								
								player->m_online = true;
								player->m_info = false;
								
								break;
							}
							
							default: break;
						}
						
						if(!player->m_online){
							closesocket(m_client);
							m_client = INVALID_SOCKET;
						}
					} else {		
						// The message has to be decrypted but since key can be changed during being online we have a locking mechanism
						{
							ScopedLock lock(player->m_xteaLock);
							XTEA_decrypt(msg, player->m_xtea);
						}
						
						// Packet ID for parsing
						uint8_t pid = msg.GetByte();
						
						switch(pid){
							case 0x96: { // Player has said something
								switch(msg.GetByte()){
									case SPEAK_PRIVATE:
									case SPEAK_PRIVATE_RED:
									case SPEAK_RVR_ANSWER:
										msg.GetString();
										break;
									case SPEAK_CHANNEL_Y:
									case SPEAK_CHANNEL_R1:
									case SPEAK_CHANNEL_R2:
										msg.GetU16();
										break;
									default:
										break;
								}

								const std::string text = msg.GetString();
								
								std::stringstream information;
								if(iequalsn(text, "speed")){
									double speed;
									sscanf(text.c_str(), "speed %lf", &speed);
									player->setSpeed(speed);
									information << "Playback speed has been set to " << speed << "x";
								} else if(iequals(text, "time")){
									information << "Current movie time is " << MilisecondsToStr(player->getNextPacket()) << " of " << MilisecondsToStr(player->getTimeTotal());			
								} else if(iequalsn(text, "ff")){
									player->m_timeForward = StrToMiliseconds(text);
									player->m_fastForward = true;
								} else if(iequalsn(text, "help") || iequalsn(text, "info")){
									player->sendClientInfo(m_client);
								}
								
								if(!information.str().empty()){
									player->sendClientMessage(m_client, information);
								}
								
								break;
							}
		
							case 0x14: { // Control + q or logout button
								player->setSpeed(1.0);
								player->m_online = false;
								player->free();
								
								closesocket(m_client);
								m_client = INVALID_SOCKET;
								
								break;
							}
							
							default: break;
						}
					}
				}
			}
			
			if(player->m_timeForward > player->getNextPacket()){
				if(player->getSpeed() != 2000.0){
					player->setSpeed(2000.0);
				}
			} else if(player->m_fastForward){
				player->setSpeed(1.0);
				player->m_fastForward = false;
				
				std::stringstream information;
				information << "The movie has been fastforwarded to " << MilisecondsToStr(player->m_timeForward);
				player->sendClientMessage(m_client, information);
			}
			
			// Send next packet if client is online
			if(player->m_online){
				std::stringstream information;
				
				{
					ScopedLock lock(player->m_informationLock);
					information.str(player->m_information.str());
					player->m_information.str("");
				}
				
				if(!information.str().empty()){
					player->sendClientMessage(m_client, information);
				}
				
				if(!player->m_timeSleep || Timer::tickCount() > player->m_timeSleep){
					NetworkMessage* message = player->getPacket();
					if(message == NULL){
						player->setSpeed(1.0);
						player->m_online = false;
						player->free();
						
						closesocket(m_client);
						m_client = INVALID_SOCKET;
					} else {				
						player->sendNetworkMessage(m_client, *message);
						if(!player->m_info){
							player->m_info = true;
							player->sendClientInfo(m_client);
						}
						
						player->wait(player->getSpeed());
						delete message;
					}
				}
			}
		}
	}
	
	player->m_online = false;
}

bool Player::running(){
	ScopedLock lock(m_runningLock);
	return m_running;
}

bool Player::start(){
	// In case of old thread running - it will be a command for it to shutdown
	{
		ScopedLock lock(m_runningLock);
		m_running = false;
	}
	
	// If an old thread was already there we need to wait for it to shutdown
	{
		ScopedLock lock(m_threadLock);
	}
	
	Thread thread(&Player::Server, this);
	return thread.isRunning();
}

void Player::stop(){
	{
		ScopedLock lock(m_runningLock);
		m_running = false;
	}
}

void Player::increaseSpeed(){
	if(running()){
		ScopedLock lock(m_informationLock);
		setSpeed(std::min(256.0, getSpeed() * 2));
	}
}

void Player::decreaseSpeed(){
	if(running()){
		ScopedLock lock(m_informationLock);
		setSpeed(std::max(0.0625, getSpeed() / 2.0));
	}
}

double Player::getSpeed(){
	ScopedLock lock(m_speedLock);
	return m_speed;
}

void Player::setSpeed(double speed){
	ScopedLock lock(m_speedLock);
	m_speed = speed;
}

uint64_t Player::getNextPacket(){
	ScopedLock lock(m_nextLock);
	return m_nextPacket;
}

void Player::setNextPacket(uint64_t next){
	ScopedLock lock(m_nextLock);
	m_nextPacket = next;
}

uint64_t Player::getTimeTotal(){
	ScopedLock lock(m_totalLock);
	return m_timeTotal;
}

void Player::setTimeTotal(uint64_t total){
	ScopedLock lock(m_totalLock);
	m_timeTotal = total;
}

std::string Player::getFileName(){
	ScopedLock lock(m_fileLock);
	return m_file;
}

void Player::free(){
	for(PacketVector::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
		delete it->second;
	}
	
	m_packets.clear();
}

bool Player::load(std::string filename){
	// free previous messages
	free();
	
	FILE* file = fopen(filename.c_str(), "rb");
	if(!file){
		return false;
	}
	
	// Lets save filename for further reference
	{
		ScopedLock lock(m_fileLock);
		m_file = filename;
	}
	
	uint32_t headerLength;
	fread(&headerLength, 4, 1, file);
	
	//skip header for now
	fseek(file, headerLength, SEEK_CUR);
	
	while(!feof(file)){
		static uint64_t timeOffset;
		static uint8_t packet[NETWORKMESSAGE_MAXSIZE];
		
		fread(&timeOffset, 8, 1, file);	
		fread(packet, NetworkMessage::header_length, 1, file);
		fread(packet + NetworkMessage::header_length, 1, *(uint16_t*)packet, file);
		
		uint16_t plen = *(uint16_t*)packet + NetworkMessage::header_length;
		
		NetworkMessage* message  = new NetworkMessage(NetworkMessage::XTEA, plen);
		memcpy(message->getBuffer() + NetworkMessage::header_length, packet, plen);
		message->setMessageLength(*(uint16_t*)packet);
		
		if(m_packets.size() == 0){
			m_timeAlignment = timeOffset;
		}
		
		m_packets.push_back(std::make_pair(timeOffset, message));
	}
	
	// remove last packet
	setTimeTotal((m_packets.begin() + (m_packets.size() - 1))->first - m_timeAlignment);
	m_packets.erase(m_packets.begin() + (m_packets.size() - 1));
	
	fclose(file);
	return true;
}

NetworkMessage* Player::getPacket(){
	if(m_packets.size() == 0){
		return NULL;
	}
	
	std::pair<uint64_t, NetworkMessage*> packet = *m_packets.begin();
	
	// Lets erase packet entry
	m_packets.erase(m_packets.begin());
	
	m_timeAlignment = packet.first;
	return packet.second;
}

void Player::wait(double timescale /* = 1.0 */){
	if(m_packets.size() == 0){
		return;
	}
	
	PacketVector::iterator packet = m_packets.begin();
	
	setNextPacket(getNextPacket() + (packet->first - m_timeAlignment));
	
	if(m_fastForward){
		m_timeSleep = 0;
	} else {
		m_timeSleep = Timer::tickCount() + ((packet->first - m_timeAlignment) / timescale);
	}
}
