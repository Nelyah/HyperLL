#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "myMap.h"
#include "hypLL64.h"
#include "bitStruct.h"








bit_st* merge(bit_st* b, uint64_t* Mval, uint64_t* Midx){ // M is already sorted
    //merge b1 and M and returns bn
    int keyB = 0, valB = 0;
    int bit=0;
    int cptM=0;
    int sizeM = SPARSE_LIMIT / 4;

    bit_st* bn = bitv_alloc(b->nbAlloc);
    bn->mode = b->mode;
    if (b->nbits == 0) {
        while (cptM < sizeM){
            bitv_append(bn, Midx[cptM], Mval[cptM]);
            cptM++;
        }
        return bn;
    }

    if (b->mode == SPARSE_MODE) {
        
        bit = P + SIZE_OF_VALUE-1;
        
        // First read of valB and Midx[cptM]
        bitv_readBits(b, &keyB, &valB, bit);
        bit += P + SIZE_OF_VALUE;
        // ------------------------

        while (cptM < sizeM || (bit+1) > b->nbits){
            if (keyB < Midx[cptM]) {  
// first case ------------------------------------------------------------
                bitv_append(bn, keyB, valB); 
                if ((bit+1) > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                }else {
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += P + SIZE_OF_VALUE;
                }
            }else if (keyB > Midx[cptM]){
// second case ------------------------------------------------------------
                bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;
                if (cptM >= sizeM) {
                    bitv_append(bn, keyB, valB); 
                    while ((bit+1) < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                }
            }else if (keyB == Midx[cptM]){
// third case ------------------------------------------------------------
                if (valB > Mval[cptM]) bitv_append(bn, keyB, valB);
                else bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;

                if ((bit+1) > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                }else{
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += P + SIZE_OF_VALUE;
                }
                if (cptM > sizeM){
                    bitv_append(bn, keyB, valB); 
                    while ((bit+1) < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                }
            }
        }
    }else if (b->mode == DENSE_MODE){
        bit = SIZE_OF_VALUE - 1;
    
        // First read of valB and Midx[cptM]
        bitv_readBits(b, &keyB, &valB, bit);
        bit += SIZE_OF_VALUE;
        Midx[cptM] = Midx[cptM];
        // ------------------------
        
        while (cptM < sizeM || (bit+1) > b->nbits){
            if (keyB < Midx[cptM]) { 
// first case ------------------------------------------------------------
                bitv_append(bn, keyB, valB); 
                if ((bit+1) > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                }else {
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += SIZE_OF_VALUE;
                }
            }else if (Midx[cptM] > keyB){
// second case ------------------------------------------------------------
                bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;
                if (cptM >= sizeM) {
                    bitv_append(bn, keyB, valB); 
                    while ((bit+1) < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                }
            }else if (keyB == Midx[cptM]){
// third case ------------------------------------------------------------
                if (valB > Mval[cptM]) bitv_append(bn, keyB, valB);
                else bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;

                if ((bit+1) > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                }else{
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += SIZE_OF_VALUE;
                }
                if (cptM > sizeM){
                    bitv_append(bn, keyB, valB); 
                    while ((bit+1) < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                }
            }
        }
    }
    
    return bn;
}





int main(int argc, char *argv[]) {
    struct bitv *b = bitv_alloc(11);
    b->mode = SPARSE_MODE;

    bitv_append(b,5,3);
    printf("nbits %d\n",b->nbits);
    bitv_append(b,6,3);
    printf("nbits %d\n",b->nbits);
    bitv_append(b,7,3);
    updateMax(b,7,4);
    bitv_dump(b);
    printf("nbits %d\n",b->nbits);
    printf("nbAlloc %d\n",b->nbAlloc);
    printf("read : %d\n",bitv_read(b,7));
/*    
    word_t a = 5;
    bitv_write(b,0,8);
    a = a << 29;
//    bitv_set(b, 5);
    a = bitv_read(b, 0);
    printf("a : %d\n",a);
//    bitv_set(b, 1*20+3);
//    bitv_set(b, 3);
    bitv_set(b, 3);
    bitv_set(b, 5);
    bitv_set(b, 7);
    bitv_set(b, 9);
    bitv_set(b, 32);
    bitv_free(b);*/
                                    
    return 0;
}
