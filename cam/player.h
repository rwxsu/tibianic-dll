#ifndef __PLAYERH__
#define __PLAYERH__

#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <windows.h>
#include <winsock.h>

#include "networkmessage.h"
#include "rsa.h"
#include "xtea.h"
#include "lock.h"
#include "thread.h"
#include "const.h"
#include "tools.h"
#include "timer.h"

typedef std::vector<std::pair<uint64_t, NetworkMessage*> > PacketVector;

class Player {
	public:
		Player();
		~Player();		
		
		// Functions save to use in different threads
		bool running();
		
		bool start();
		void stop();
		
		void increaseSpeed();
		void decreaseSpeed();
		
		double getSpeed();
		void setSpeed(double speed);
		
		uint64_t getNextPacket();
		uint64_t getTimeTotal();
			
		std::string getFileName();
	protected:
		void sendNetworkMessage(SOCKET m_client, NetworkMessage& output);
		void sendClientMessage(SOCKET m_client, std::stringstream& msg);
		void sendClientInfo(SOCKET m_client);
		
		// Server thread function
		static DWORD WINAPI Server(LPVOID lpParam);
		
		// Thread unsafe function
		void free();
		
		// This function can only be used within server thread
		bool load(std::string filename);
		
		// This function can only be used within server thread
		NetworkMessage* getPacket();
		
		// This function can only be used within server thread
		void wait(double timescale = 1.0);
		
		// Thread safe functions
		void setNextPacket(uint64_t next);
		void setTimeTotal(uint64_t total);
	private:
		PacketVector m_packets;	
		SOCKET m_socket;
		
		uint32_t m_xtea[4];
		
		uint64_t m_timeAlignment;
		uint64_t m_timeTotal;
		uint64_t m_timeCurrent;
		uint64_t m_timeSleep;
		uint64_t m_timeForward;	
		uint64_t m_nextPacket;
		
		bool m_fastForward;
		bool m_running;
		bool m_online;
		bool m_info;
		
		double m_speed;
		
		std::stringstream m_information;
		
		std::string m_status;
		std::string m_file;
		
		// Lock mechanisms for different variables
		Lock m_threadLock;
		Lock m_runningLock;
		Lock m_statusLock;
		Lock m_xteaLock;
		Lock m_speedLock;
		Lock m_nextLock;
		Lock m_totalLock;
		Lock m_informationLock;
		Lock m_fileLock;
};

#endif
