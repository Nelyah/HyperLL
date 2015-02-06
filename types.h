#ifndef DEF_TYPES
#define DEF_TYPES

#define SIZE_OF_VALUE 6
#define SPARSE_MODE -1
#define DENSE_MODE -2 
#include "hypLL64.h"
extern int SPARSE_LIMIT; 



typedef char word_t;
enum { BITS_PER_WORD = 8 };

typedef struct {
    word_t *words; 
    int cptW;
    int nwords; 
    int nbits;  // number of used bits
    int nbAlloc; // number of allocated bits
    int mode;
} bit_st;


#endif
