#ifndef __CRC__
#define __CRC__

#include <stdint.h>
#include <iostream>

uint32_t crc32(std::string filename);
uint32_t crc32(uint8_t* data_in, uint16_t bytes);
uint8_t crc8(uint8_t* data_in, uint16_t number_of_bytes_to_read);

#endif
