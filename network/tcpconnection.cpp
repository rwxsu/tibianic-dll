#include "tcpconnection.h"
#include "tools.h"

#include <sstream>
#include <errno.h>
#include <windows.h>

TcpConnection::TcpConnection(type_t type){
	m_type = type;
	m_status = CONNECTION_DISCONNECTED;
	m_running = true;
	
	// Set default buffer for incoming messages
	m_inBuffer = NULL;
	m_inLength = 0;
	
	// Set default buffer for outgoing messages
	m_outBuffer = NULL;
	m_outLength = 0;
	
	m_thread = new Thread(TcpConnection::worker, this);
}

TcpConnection::~TcpConnection(){
	{
		ScopedLock lock(m_runningLock);
		m_running = false;
	}
	
	ScopedLock lock(m_threadLock);
	delete m_thread;
}

DWORD WINAPI TcpConnection::worker(LPVOID lpParam){
	TcpConnection* connection = reinterpret_cast<TcpConnection*>(lpParam);
	ScopedLock threadLock(connection->m_threadLock);
	
	SOCKET connectionSocket = INVALID_SOCKET;
	sockaddr_in connectionAddress;
	hostent* connectionHost = NULL;
	uint8_t connectionBuffer[NETWORKMESSAGE_MAXSIZE];
	
	bool running = true;
	while(running){
		status_t status;
		{
			ScopedLock lock(connection->m_paramsLock);
			status = connection->m_status;
		}
		
		switch(status){
			case CONNECTION_DISCONNECTED: {
				break;
			}
			
			case CONNECTION_CONNECTING: {
				type_t connectionType;
				{
					ScopedLock lock(connection->m_paramsLock);
					connectionType = connection->m_type;
				}
				
				connectionSocket = socket(AF_INET, connectionType == CONNECTION_TCP ? SOCK_STREAM : SOCK_DGRAM, connectionType == CONNECTION_TCP ? IPPROTO_TCP : IPPROTO_UDP);
				if(connectionSocket == INVALID_SOCKET){
					ScopedLock lock(connection->m_paramsLock);
					
					connection->m_status = CONNECTION_ERROR;
					connection->m_error = std::string("failed to create socket");
					break;
				}	
				
				{
					ScopedLock lock(connection->m_paramsLock);
					
					connectionAddress.sin_family = AF_INET;
					connectionAddress.sin_port = htons(connection->m_port);		
					connectionHost = gethostbyname(connection->m_host.c_str());
				}
				
				if(connectionHost == NULL){
					ScopedLock lock(connection->m_paramsLock);
					
					connection->m_status = CONNECTION_ERROR;
					connection->m_error = std::string("failed to resolve host");
					break;
				}
				
				CopyMemory(&connectionAddress.sin_addr, connectionHost->h_addr_list[0], connectionHost->h_length);
				
				if(connect(connectionSocket, (sockaddr*)&connectionAddress, sizeof(connectionAddress)) == SOCKET_ERROR){
					ScopedLock lock(connection->m_paramsLock);
					
					connection->m_status = CONNECTION_ERROR;
					connection->m_error = std::string("failed while connect");
					break;
				}
				
				// Non blocking socket
				u_long iMode = 1;
				ioctlsocket(connectionSocket, FIONBIO, &iMode);
				
				// Everything went fine
				{
					ScopedLock lock(connection->m_paramsLock);
					
					connection->m_status = CONNECTION_CONNECTED;
					connection->m_error = std::string("connected");
					
					// Information about connected address
					sockaddr_in addr;
					int socklen = sizeof(addr);
					
					// Acquire ip address of the end point
					if(getpeername(connectionSocket, (sockaddr*)&addr, &socklen) == 0){
						connection->m_endpoint = std::string(inet_ntoa(addr.sin_addr));
					}
					
					// get rid of the old buffer
					if(connection->m_inBuffer){
						free(connection->m_inBuffer);
					}
					
					connection->m_inBuffer = NULL;				
					connection->m_inLength = 0;
				}
				
				break;
			}
			
			case CONNECTION_CONNECTED: {
				int length = recv(connectionSocket, (char*)connectionBuffer, NETWORKMESSAGE_MAXSIZE, 0);
				
				if (length < 0){
					if ((WSAGetLastError() == EAGAIN) || (WSAGetLastError() == WSAEWOULDBLOCK)){
						//printf("non-blocking operation returned EAGAIN or EWOULDBLOCK\n");
					} else {
						ScopedLock lock(connection->m_paramsLock);
						
						std::stringstream error;
						error << "error while fetching message: " << WSAGetLastError();
					
						connection->m_status = CONNECTION_ERROR;
						connection->m_error = error.str();
						break;
					}
				} else if(length == 0){ // connection was closed
					ScopedLock lock(connection->m_paramsLock);
					
					connection->m_status = CONNECTION_DISCONNECTED;
					connection->m_error = std::string("connection closed");
					
					break;
				} else {
					ScopedLock lock(connection->m_paramsLock);
					
					// Realloc will take care of everything even if buffer is NULL
					connection->m_inBuffer = (uint8_t*)realloc(connection->m_inBuffer, connection->m_inLength + length);
					
					// Copy current data to main buffer
					memcpy(&connection->m_inBuffer[connection->m_inLength], connectionBuffer, length);
		
					// Set new length of the buffer
					connection->m_inLength = connection->m_inLength + length;
				}
				
				// To send data length
				uint8_t* outBuffer = NULL;
				uint32_t outLength = 0;
				
				// Send awaiting data
				{
					ScopedLock lock(connection->m_paramsLock);
					
					if(connection->m_outBuffer){
						outBuffer = connection->m_outBuffer;	
						outLength = connection->m_outLength;
						
						// Set default values
						connection->m_outBuffer = NULL;
						connection->m_outLength = 0;
						
						// Information for the user
						connection->m_error = std::string("sending data...");
					}
				}
				
				if(outBuffer){
					int outTransmission = 0;
					int outReturn = 0;
					
					while(outTransmission != outLength){
						if((outReturn = send(connectionSocket, (char*)outBuffer + outTransmission, outLength - outTransmission, 0)) == SOCKET_ERROR){
							ScopedLock lock(connection->m_paramsLock);
					
							connection->m_status = CONNECTION_ERROR;
							connection->m_error = std::string("connection error during send");
							break;
						}
						
						outTransmission = outTransmission + outReturn;
					}
					
					// Free buffer
					free(outBuffer);
				}
				
				break;
			}
			
			case CONNECTION_DISCONNECT: {
				if(connectionSocket != INVALID_SOCKET){
					closesocket(connectionSocket);
					
					connectionSocket = INVALID_SOCKET;
				}
				
				ScopedLock lock(connection->m_paramsLock);
				
				connection->m_status = CONNECTION_DISCONNECTED;
				connection->m_error = std::string("connection closed on request");
				break;
			}
			
			case CONNECTION_ERROR: {
				// Do smth?
				break;
			}
			
			default: break;
		}
		
		ScopedLock lock(connection->m_runningLock);
		running = connection->m_running;
	}
	
	if(connectionSocket != INVALID_SOCKET){
		closesocket(connectionSocket);
	}
}

void TcpConnection::tryconnect(std::string host, uint16_t port){
	ScopedLock lock(m_paramsLock);
	if(m_status == CONNECTION_DISCONNECTED || m_status == CONNECTION_ERROR){
		m_status = CONNECTION_CONNECTING;
		
		m_host = host;
		m_port = port;
		
		m_error = std::string("connecting...");
	}
}

void TcpConnection::disconnect(){
	ScopedLock lock(m_paramsLock);
	if(m_status == CONNECTION_CONNECTED){
		m_status = CONNECTION_DISCONNECT;
	}
}

void TcpConnection::push(char* buffer, uint32_t size){
	ScopedLock lock(m_paramsLock);
	
	m_outBuffer = (uint8_t*)realloc(m_outBuffer, m_outLength + size);
	memcpy(&m_outBuffer[m_outLength], buffer, size);
	m_outLength = m_outLength + size;
}

uint32_t TcpConnection::pop(char* buffer){
	ScopedLock lock(m_paramsLock);
	
	uint32_t ret = m_inLength;
	if(m_inBuffer){
		memcpy(buffer, m_inBuffer, m_inLength);
		
		// Free the buffer
		free(m_inBuffer);
		
		// Set default values
		m_inBuffer = NULL;
		m_inLength = 0;
		
		return ret;
	}
	
	return ret;
}

TcpConnection::status_t TcpConnection::getStatus(){
	ScopedLock lock(m_paramsLock);
	return m_status;
}

std::string TcpConnection::getError(){
	ScopedLock lock(m_paramsLock);
	return m_error;
}

std::string TcpConnection::getEndPoint(){
	ScopedLock lock(m_paramsLock);
	return m_endpoint;
}
