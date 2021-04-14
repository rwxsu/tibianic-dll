#ifndef __TCPCONNECTIONH__
#define __TCPCONNECTIONH__

#include "const.h"
#include "thread.h"
#include "lock.h"
#include "networkmessage.h"

#include <iostream>
#include <map>

class TcpConnection {
	public:
		enum status_t {
			CONNECTION_DISCONNECTED = 0,
			CONNECTION_CONNECTING = 1,
			CONNECTION_CONNECTED = 2,
			CONNECTION_DISCONNECT = 3,
			CONNECTION_ERROR = 4,
		};
		
		enum type_t {
			CONNECTION_DIAGRAM,
			CONNECTION_TCP
		};
		
		TcpConnection(type_t type);
		~TcpConnection();
		
		/* Performing connection or disconnect */
		void tryconnect(std::string host, uint16_t port);
		void disconnect();
		
		/* Messages related functions */
		void push(char* buffer, uint32_t size);
		uint32_t pop(char* buffer);
		
		/* Information related functions */
		status_t getStatus();
		std::string getError();
		std::string getEndPoint();
	protected:
		static DWORD WINAPI worker(LPVOID lpParam);
	private:
		Lock m_threadLock;
		Lock m_runningLock;
		Lock m_paramsLock;
		
		Thread* m_thread;
		bool m_running;
		
		status_t m_status;
		type_t m_type;
		
		/* The host and port which we connect to */
		std::string m_host;
		uint16_t m_port;
		
		std::string m_error;
		std::string m_endpoint;
		
		/* Input stream */
		uint8_t* m_inBuffer;
		uint32_t m_inLength;
		
		/* Output stream */
		uint8_t* m_outBuffer;
		uint32_t m_outLength;
};

#endif
