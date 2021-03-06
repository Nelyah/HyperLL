
#include "murmur3.h"

#ifndef HYP_LOG_LOG
#define HYP_LOG_LOG


#define a_16 0.673
#define a_32 0.697
#define a_64 0.709
#define a_128(m) 0.7213/(1+0.079/m) 

int lastBits(int n, int val);
float hyperLL_32bits(void);
void addItem(uint32_t hashVal);
void init(void);
void freeAll(void);



#endif
