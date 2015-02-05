#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitStruct.h"

int SPARSE_LIMIT = (SIZE_OF_VALUE*(1 << P))/(P+SIZE_OF_VALUE);

void bitv_set(bit_st *b, int bit, int val) {
    int bitR = bitv_get(b,bit);
    if (bitR == 1) {
        if (val == 0) {
            b->words[bit >> 5] -= 1 << (31 - (bit % BITS_PER_WORD));
        }
    }else if (bitR == 0){
        b->words[bit >> 5] |= val << (31 - (bit % BITS_PER_WORD));
    }
}


int bitv_get(bit_st *b, int bit) {
    return (b->words[bit >> 5] & (1 << (31 - (bit % BITS_PER_WORD)))) >> (31 - (bit % BITS_PER_WORD));
}

int  bitv_read(bit_st *b, int index) {
    int value = 0, i, bit;

    if (b->mode == DENSE_MODE) {
        bit = index*SIZE_OF_VALUE+SIZE_OF_VALUE-1;
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            value += (bitv_get(b, bit) << i);
            bit--;
        }
        return value;
    }else if (b->mode == SPARSE_MODE){
        int curKey=-1, lastPos;
        bit = P-1; 
        lastPos = bit;
        while (curKey != index) {
            curKey = 0;
            for (i = 0; i < P; i++) {
                curKey += (bitv_get(b, bit) << i);
                bit--;
            }
            if (curKey == index) {
                bit = lastPos + SIZE_OF_VALUE;
                for (i = 0; i < SIZE_OF_VALUE; i++) {
                    value += bitv_get(b, bit) << i;
                    bit--;
                }
                return value;
            }
            bit = lastPos+SIZE_OF_VALUE+P;
            lastPos = bit;
            if (curKey != index && (bit > b->nbits || curKey > index)) {
                // L'index n'est pas répertorié
                return -1;
            }
        }
    }

    return -2;
}

bit_st* getDense(bit_st* b){
    if (b->mode == DENSE_MODE) return b;
    bit_st *bn = bitv_alloc(SIZE_OF_VALUE*(1 << P));
    bn->mode = DENSE_MODE;
    bn->nbits = (SIZE_OF_VALUE*(1 << P));
    int i, value=0, index=0, bit, bitn;
    bit = P+SIZE_OF_VALUE-1;
    while(bit < b->nbits){
        index = 0;
        value = 0;
        bitv_readBits(b,&index,&value,bit);
        bitn = (index*SIZE_OF_VALUE)+SIZE_OF_VALUE-1;
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            bitv_set(bn, bitn, (value & 1)); 
            bitn--;
            value >>= 1;
        }
        bit += P + SIZE_OF_VALUE;
    }

    return bn;

}

void bitv_append(bit_st *b, int index, int value){
    // value must be < 2**6
    int i, bit=0;
    if (b->mode == DENSE_MODE){ 
        bit = index*SIZE_OF_VALUE+SIZE_OF_VALUE-1;
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            bitv_set(b, bit, (value & 1));
            bit--;
            value >>= 1;
        }
    }else if (b->mode == SPARSE_MODE){
        bit = b->nbits+P+SIZE_OF_VALUE-1;
        while (bit > b->nbAlloc) b = bitv_realloc(b, b->nbAlloc*2);
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            bitv_set(b, bit, (value & 1));
            bit--;
            value >>=1;
        }
        for (i = 0; i < P; i++) {
            bitv_set(b, bit, (index & 1));
            bit--;
            index >>=1;
        }
        b->nbits += P + SIZE_OF_VALUE;
    }
}

void bitv_write(bit_st* b, int index, int value){
    if (b->mode == DENSE_MODE){
        int curVal = bitv_read(b, index);
        if (value > curVal) bitv_append(b, index, value);
    }else {
        int bit, i, lastPos, curVal=0;
        int curKey = -1;
        bit = P-1; 
        lastPos = bit;
        while (curKey != index) {
            curKey = 0;
            for (i = 0; i < P; i++) {
                curKey += (bitv_get(b, bit) << i);
                bit--;
            }
            if (curKey == index) {
                bit = lastPos + SIZE_OF_VALUE ;
                lastPos = bit;
                for (i = 0; i < SIZE_OF_VALUE; i++) {
                    curVal += bitv_get(b, bit) << i;
                    bit--;
                }
                bit += SIZE_OF_VALUE;
                break;
            }
            bit = lastPos+SIZE_OF_VALUE+P;
            lastPos = bit;
            if (curKey != index && (bit > b->nbits || curKey > index)) {
                // L'index n'est pas répertorié
                curKey = -1;
                break;
            }
        }
        if (curKey == -1 ) {
            bitv_append(b,index,value);
            
        }else{
            for (i = 0; i < SIZE_OF_VALUE; i++) {
                bitv_set(b, bit, (value & 1));
                bit--;
                value >>=1;
            }
        }
    }
}


void bitv_free(bit_st *b) {
    if (b != NULL) {
        if (b->words != NULL) free(b->words);
        free(b);
    }
}

bit_st* bitv_realloc(bit_st* b, int bits) {
    
    long prevSizeof = b->nwords;
    b->nwords = (bits >> 5) + 1;
    b->nbAlloc  = b->nwords*BITS_PER_WORD;
    b->words  = realloc(b->words, b->nwords*sizeof(b->words));

    if (b->words == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    memset(b->words+prevSizeof, 0, sizeof(b->words)*(b->nwords-prevSizeof));

    return b;
}

bit_st* bitv_alloc(int bits) {
    
    bit_st *b = malloc(sizeof(bit_st));
    if (b == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    b->nwords = (bits >> 5) + 1;
    b->nbAlloc  = b->nwords*BITS_PER_WORD;
    b->words  = calloc(b->nwords, sizeof(b->words));
    b->nbits = 0;

    if (b->words == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    memset(b->words, 0, sizeof(b->words) * b->nwords);

    return b;
}

void bitv_readBits(bit_st* b, int *index, int *value, int bit){
    int v1 = 0, idx = 0, i;
    if (b->mode == SPARSE_MODE){
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            v1 += (bitv_get(b, bit) << i);
            bit--;
        }
        for (i = 0; i < P; i++) {
            idx += (bitv_get(b, bit) << i);
            bit--;
        }
        *value = v1;
        *index = idx;
    }else if (b->mode == DENSE_MODE){
        *index = bit/SIZE_OF_VALUE;
        for (i = 0; i < SIZE_OF_VALUE; i++) {
            v1 += (bitv_get(b, bit) << i);
            bit--;
        }
        *value = v1;
    }
}


void updateMax(bit_st* b, int index, int value){
    if (b->mode == DENSE_MODE){
        int curVal = bitv_read(b, index);
        if (value > curVal) bitv_append(b, index, value);
    }else {
        int bit, i, lastPos, curVal=0;
        int curKey = -1;
        bit = P-1; 
        lastPos = bit;
        while (curKey != index) {
            curKey = 0;
            for (i = 0; i < P; i++) {
                curKey += (bitv_get(b, bit) << i);
                bit--;
            }
            if (curKey == index) {
                bit = lastPos + SIZE_OF_VALUE ;
                lastPos = bit;
                for (i = 0; i < SIZE_OF_VALUE; i++) {
                    curVal += bitv_get(b, bit) << i;
                    bit--;
                }
                bit += SIZE_OF_VALUE;
                break;
            }
            bit = lastPos+SIZE_OF_VALUE+P;
            lastPos = bit;
            if (curKey != index && (bit > b->nbits || curKey > index)) {
                // L'index n'est pas répertorié
                curKey = -1;
                break;
            }
        }
        if (curKey == -1 ) {
            bitv_append(b,index,value);
        }else if (curVal < value){
            for (i = 0; i < SIZE_OF_VALUE; i++) {
                bitv_set(b, bit, (value & 1));
                bit--;
                value >>=1;
            }
        }
    }
}


int bytesUsed(bit_st *b) {
    int val = b->nbits/8;
    if (b->nbits % 8 != 0) val++;
    return val;
}

void bitv_dump(bit_st *b) {
    if (b == NULL) return;

    for(int i = 0; i < b->nwords; i++) {
        word_t w = b->words[i];

        for (int j = 0; j < BITS_PER_WORD; j++) {
            printf("%d", w & 1);
            w >>= 1;
        }

        printf(" ");
    }

    printf("\n");
}

