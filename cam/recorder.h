#ifndef __RECORDERH__
#define __RECORDERH__

#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>

#include "networkmessage.h"
#include "crc.h"

class Recorder {
	public:
		Recorder();
		~Recorder();
		
		void enable();
		void disable();
		
		bool isEnabled();
	
		std::string flush();
		
		bool empty(){ return m_packets.empty(); };
		void clear();
		
		void addPacket(NetworkMessage* packet);
		
		static std::string name();
	private:
		std::vector<std::pair<uint64_t, NetworkMessage*> > m_packets;
		bool m_enabled;
};

#endif
