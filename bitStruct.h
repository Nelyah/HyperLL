#ifndef DEF_BITSTRUCT
#define DEF_BITSTRUCT
#include "types.h"


bit_st* bitv_realloc(bit_st* b, int bits);
bit_st* bitv_alloc(int bits);
void bitv_append(bit_st *b, int index, int value);
void bitv_free(bit_st *b);
int bitv_get(bit_st *b, int bit);
int bitv_read(bit_st *b, int index);
void bitv_set(bit_st *b, int bit, int val);
void bitv_write(bit_st *b, int index, int value);
int bytesUsed(bit_st *b);
void updateMax(bit_st* b, int index, int value);
void bitv_readBits(bit_st* b, int *index, int *value, int bit);
void bitv_dump(bit_st *b);
bit_st* getDense(bit_st* b);

#endif
