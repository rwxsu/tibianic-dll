#include <windows.h>
#include <sstream>

#include "xtea.h"

extern void raiseError(std::stringstream &s, bool exit = false);
extern void raiseError(std::string s, bool exit = false);

void XTEA_encrypt(char* buf, int messageLength, uint32_t m_key[4]){
	uint32_t k[4];
	k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];

	int read_pos = 0;
	uint32_t* buffer = (uint32_t*)buf;
	while(read_pos < messageLength/4){
		uint32_t v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
		uint32_t delta = 0x61C88647;
		uint32_t sum = 0;

		for(int32_t i = 0; i < 32; ++i) {
			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum -= delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
		}
		buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}
}

void XTEA_encrypt(NetworkMessage& msg, uint32_t m_key[4]){
	uint32_t k[4];
	k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];

	int32_t messageLength = msg.getMessageLength() + NetworkMessage::header_length;

	//add bytes until reach 8 multiple
	uint32_t n;
	if((messageLength % 8) != 0){
		n = 8 - (messageLength % 8);
		//msg.AddPaddingBytes(n);
		messageLength = messageLength + n;
	}

	int read_pos = 0;
	uint32_t* buffer = (uint32_t*)(msg.getBuffer() + NetworkMessage::header_length);
	while(read_pos < messageLength/4){
		uint32_t v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
		uint32_t delta = 0x61C88647;
		uint32_t sum = 0;

		for(int32_t i = 0; i < 32; ++i) {
			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum -= delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
		}
		buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}
	
	*(uint16_t*)msg.getBuffer() = messageLength;
	msg.setMessageLength(messageLength);
}

bool XTEA_decrypt(NetworkMessage& msg, uint32_t m_key[4]){
	if((msg.getMessageLength() - 2) % 8 != 0){
		return false;
	}

	uint32_t k[4];
	k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];

	uint32_t* buffer = (uint32_t*)(msg.getBuffer() + msg.getReadPos());
	int read_pos = 0;
	int32_t messageLength = msg.getMessageLength() - NetworkMessage::header_length;
	while(read_pos < messageLength/4){
		uint32_t v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
		uint32_t delta = 0x61C88647;
		uint32_t sum = 0xC6EF3720;

		for(int32_t i = 0; i < 32; ++i) {
			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
			sum += delta;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
		}
		buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}

	int tmp = msg.GetU16();
	if(tmp > msg.getMessageLength()){
		return false;
	}

	msg.setMessageLength(tmp + NetworkMessage::header_length * 2); // Including xtea + packet headers
	return true;
}
