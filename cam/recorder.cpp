#include "recorder.h"
#include "const.h"

#include <windows.h>
#include <time.h>
#include <fstream>

Recorder::Recorder(){
	m_enabled = false;
}

Recorder::~Recorder(){
	clear();
}

void Recorder::enable(){
	m_enabled = true;
}

void Recorder::disable(){
	m_enabled = false;
}

bool Recorder::isEnabled(){
	return m_enabled;
}

std::string Recorder::flush(){
	std::string filename = Recorder::name();
	std::string fullpath = std::string(CAM_DIR) + std::string("/") + filename;
	
	std::ofstream file;
	file.open(fullpath.c_str(), std::ios::out | std::ios::app | std::ios::binary); 
	
	if(!file.is_open()){
		return std::string();
	}
	
	uint32_t header_size = 8;
	file.write((char*)&header_size, 4);
	
	
	// now lets calculate checksum of all packets size + pid
	uint64_t checksum = 0;
	
	int n = 0;
	for(std::vector<std::pair<uint64_t, NetworkMessage*> >::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
		NetworkMessage* msg = it->second;
		
		checksum ^= (uint64_t)crc8((uint8_t*)msg->getBuffer(), 3) << (uint64_t)(n << 3);
		
		if(++n & (1 << 3)){
			n = 0;
		}
	}
	
	file.write((char*)&checksum, 8);
	
	for(std::vector<std::pair<uint64_t, NetworkMessage*> >::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
		NetworkMessage* msg = it->second;
		uint16_t psize = msg->getMessageLength();	
		
		file.write((char*)&it->first, 8);
		file.write(msg->getBuffer(), msg->getMessageLength());
		file.flush();
	}
	
	clear();
	file.close();
	
	return filename;
}

void Recorder::clear(){
	for(std::vector<std::pair<uint64_t, NetworkMessage*> >::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
		delete it->second;
	}
	
	m_packets.clear();
}

void Recorder::addPacket(NetworkMessage* packet){
	uint64_t tick = GetTickCount();
	
	m_packets.push_back(std::make_pair(tick, packet));
}

std::string Recorder::name(){
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	std::stringstream ss;
	ss << timeinfo->tm_year + 1900 << "-";
	ss << timeinfo->tm_mon + 1 << "-";
	ss << timeinfo->tm_mday << "-";
	ss << timeinfo->tm_hour << "-";
	ss << timeinfo->tm_min << "-";
	ss << timeinfo->tm_sec;
	ss << ".cam";
	
	return ss.str();
}

