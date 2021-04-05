#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

// msb must be the position of the msb (from 0 to range)
uint32_t sext(uint32_t val, uint8_t msb);

#endif