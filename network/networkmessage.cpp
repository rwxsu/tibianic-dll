#include <string>
#include <iostream>
#include <sstream>

#include "networkmessage.h"

NetworkMessage::NetworkMessage(uint8_t *packet){
	Reset();
	memcpy(&m_MsgSize, packet, 2);
	m_MsgSize += 2;
	
	m_MsgBuf = new uint8_t[m_MsgSize];
	memcpy(m_MsgBuf, packet, m_MsgSize); 
}

NetworkMessage::NetworkMessage(uint8_t *packet, uint16_t size){
	Reset();
	m_MsgSize = size;
	m_ReadPos = 0;
	
	m_MsgBuf = new uint8_t[m_MsgSize];
	memcpy(m_MsgBuf, packet, m_MsgSize); 
}

int32_t NetworkMessage::decodeHeader()
{
	int32_t size = (int32_t)(m_MsgBuf[0] | m_MsgBuf[1] << 8);
	m_MsgSize = size;
	return size;
}

/******************************************************************************/
std::string NetworkMessage::GetString()
{
	uint16_t stringlen = GetU16();
	if(stringlen >= (NETWORKMESSAGE_MAXSIZE - m_ReadPos))
	return std::string();

	char* v = (char*)(m_MsgBuf + m_ReadPos);
	m_ReadPos += stringlen;
	return std::string(v, stringlen);
}

std::string NetworkMessage::GetRaw()
{
	uint16_t stringlen = m_MsgSize- m_ReadPos;
	if(stringlen >= (NETWORKMESSAGE_MAXSIZE - m_ReadPos))
	return std::string();

	char* v = (char*)(m_MsgBuf + m_ReadPos);
	m_ReadPos += stringlen;
	return std::string(v, stringlen);
}

/******************************************************************************/

void NetworkMessage::AddString(const char* value)
{
	uint32_t stringlen = (uint32_t)strlen(value);
	if(!canAdd(stringlen+2) || stringlen > 8192)
	return;

	AddU16(stringlen);
	strcpy((char*)(m_MsgBuf + m_ReadPos), value);
	m_ReadPos += stringlen;
	m_MsgSize += stringlen;
}

void NetworkMessage::AddBytes(const char* bytes, uint32_t size)
{
	if(!canAdd(size) || size > 8192)
		return;

	memcpy(m_MsgBuf + m_ReadPos, bytes, size);
	m_ReadPos += size;
	m_MsgSize += size;
}

void NetworkMessage::AddPaddingBytes(uint32_t n)
{
	if(!canAdd(n))
		return;

	memset((void*)&m_MsgBuf[m_ReadPos], 0x33, n);
	m_MsgSize = m_MsgSize + n;
	m_ReadPos = m_ReadPos + n;
}