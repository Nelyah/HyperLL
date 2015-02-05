#ifndef DEF_DELTAVARINT_DECODER
#include <inttypes.h>

void deltaVarIntDecoder(char* value);

uint32_t getNext_noDelta();

uint32_t getNext();

#endif
