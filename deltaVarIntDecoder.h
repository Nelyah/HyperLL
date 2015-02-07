#ifndef DEF_DELTAVARINT_DECODER
#include <inttypes.h>

void deltaVarIntDecoder(char* value, int size);
void reset_varintDecoder(void);

uint32_t getNext_noDelta();

uint32_t getNext();

#endif
