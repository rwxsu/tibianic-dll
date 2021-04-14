#ifndef __OTSERV_NETWORK_MESSAGE_H__
#define __OTSERV_NETWORK_MESSAGE_H__

#include <iostream>
#include <string.h>
#include <stdint.h>

#include "position.h"

#define NETWORKMESSAGE_MAXSIZE 15340

class NetworkMessage
{
	public:
		enum { header_length = 2 };
		enum { max_body_length = NETWORKMESSAGE_MAXSIZE - header_length };
		enum encryption_t { NO_ENCRYPTION = 0, XTEA = 1 };

		NetworkMessage(encryption_t type = NO_ENCRYPTION){
			m_encryption = type;
			
			switch(type){
				case XTEA: {
					m_MsgSize = 0;
					m_ReadPos = 4;
					break;
				}
				
				default: {
					Reset();
					break;
				}
			}
			
			m_MsgBuf = new uint8_t[NETWORKMESSAGE_MAXSIZE];
		}
		
		NetworkMessage(encryption_t type, uint16_t size){
			m_encryption = type;
			
			switch(type){
				case XTEA: {
					m_MsgSize = 0;
					m_ReadPos = 4;
					break;
				}
				
				default: {
					Reset();
					break;
				}
			}
			
			m_MsgBuf = new uint8_t[((size + 7) & ~7) + header_length];
		}
		
		NetworkMessage(uint8_t *packet);
		NetworkMessage(uint8_t *packet, uint16_t size);
		
		~NetworkMessage(){
			delete[] m_MsgBuf;
		};
		

		void Reset(){
			m_encryption = NO_ENCRYPTION;
			m_MsgSize = 0;
			m_ReadPos = 2;
		}
		
		uint8_t  GetByte(){return m_MsgBuf[m_ReadPos++];}

		uint16_t GetU16(){
			uint16_t v = *(uint16_t*)(m_MsgBuf + m_ReadPos);
			m_ReadPos += 2;
			return v;
		}
		
		uint32_t GetU32(){
			uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
			m_ReadPos += 4;
			return v;
		}
		
		uint8_t PeekByte(){
			return m_MsgBuf[m_ReadPos];
		}
		
		uint16_t PeekU16(){
			uint16_t v = *(uint16_t*)(m_MsgBuf + m_ReadPos);
			return v;
		}
		
		uint32_t PeekU32(){
			uint32_t v = *(uint32_t*)(m_MsgBuf + m_ReadPos);
			return v;
		}

		uint16_t GetSpriteId(){
			return GetU16();
		}

		std::string GetString();
		std::string GetRaw();
		Position GetPosition(){
			Position pos;
			pos.x = GetU16();
			pos.y = GetU16();
			pos.z = GetByte();
			
			return pos;
		}
		// skips count unknown/unused bytes in an incoming message
		void SkipBytes(int count){m_ReadPos += count;}
		void dbgEnabled(){m_ReadPos = 0; }
		
		void AddPosition(const Position &pos){
			AddU16(pos.x);
			AddU16(pos.y);
			AddByte(pos.z);
		}

		// simply write functions for outgoing message
		void AddByte(uint8_t  value){
			if(!canAdd(1))
				return;
			m_MsgBuf[m_ReadPos++] = value;
			m_MsgSize++;
		}

		void AddU16(uint16_t value){
			if(!canAdd(2))
				return;
			*(uint16_t*)(m_MsgBuf + m_ReadPos) = value;
			m_ReadPos += 2; m_MsgSize += 2;
		}
		void AddU32(uint32_t value){
			if(!canAdd(4))
				return;
			*(uint32_t*)(m_MsgBuf + m_ReadPos) = value;
			m_ReadPos += 4; m_MsgSize += 4;
		}

		void AddBytes(const char* bytes, uint32_t size);
		void AddPaddingBytes(uint32_t n);

		void AddString(const std::string& value){AddString(value.c_str());}
		void AddString(const char* value);

		int32_t getMessageLength() const { return m_MsgSize; }
		bool isSmthToRead(){ 
			return m_ReadPos < m_MsgSize; 
		}
		
		/* important */
		void writeMessageLength(){ 
			if(m_encryption == XTEA){
				*(uint16_t*)(&m_MsgBuf[2]) = getMessageLength();
			} else {
				*(uint16_t*)(m_MsgBuf) = getMessageLength();
			}
		}
		
		void setMessageLength(int32_t newSize) { m_MsgSize = newSize; }
		int32_t getReadPos() const { return m_ReadPos; }
		void setReadPos(int32_t pos) {m_ReadPos = pos; }

		int32_t decodeHeader();

		char* getBuffer() { return (char*)&m_MsgBuf[0]; }
		char* getReadBuffer(){ return (char*)&m_MsgBuf[m_ReadPos]; }
		char* getBodyBuffer() { return (char*)&m_MsgBuf[header_length]; }


	protected:
		inline bool canAdd(uint32_t size)
		{
			return (size + m_ReadPos < max_body_length);
		};

		int32_t m_MsgSize;
		int32_t m_ReadPos;
		
		encryption_t m_encryption;
		/*
		uint8_t m_MsgBuf[NETWORKMESSAGE_MAXSIZE];
		*/
		uint8_t* m_MsgBuf;
};
#endif // #ifndef __NETWORK_MESSAGE_H__
