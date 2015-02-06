#ifndef DEF_MYMAP
#define DEF_MYMAP

#include "types.h"
uint32_t fusionInt(uint32_t, uint32_t value);
void splitInt(uint32_t* idx, uint32_t* value, uint32_t toSplit);
void unifTab(uint32_t* Mval, uint32_t* Midx, int* sizeM);
bit_st* merge(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM);
bit_st* merge_sparse(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM);
bit_st* merge_dense(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM);


int main(int argc, char *argv[]);

#endif
