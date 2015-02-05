#include <inttypes.h>

#ifndef DEF_VARINT
#define DEF_VARINT
uint8_t* encode (uint32_t value);
uint32_t decode(uint8_t* encoded);
uint32_t decode2(uint8_t* encoded, int *len);

#endif
