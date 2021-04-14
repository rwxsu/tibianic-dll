#ifndef __XTEA__
#define __XTEA_

#include <iostream>

#include "networkmessage.h"

void XTEA_encrypt(NetworkMessage& msg, uint32_t m_key[4]);
void XTEA_encrypt(char* buf, int messageLength, uint32_t m_key[4]);
bool XTEA_decrypt(NetworkMessage& msg, uint32_t m_key[4]);

#endif
