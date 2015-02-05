#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "myMap.h"
#include "bitStruct.h"


void unifTab(uint64_t* Mval, uint64_t* Midx){
/* removes all index duplicate in M, and keeps the one with highest value 
 * M should already be sorted */
    int max=0, idx=0, cpt=0, cpt2=0;
    while (cpt < SPARSE_LIMIT/4){
        max = Mval[cpt];
        idx = Midx[cpt];
        if (idx != Midx[cpt+1]) {
            cpt++;
        }else{
            cpt++;
            while(idx == Midx[cpt]){
                if (max < Mval[cpt]) max = Mval[cpt];
                cpt++;
            }
        }
        Midx[cpt2] = Midx[cpt-1];
        Mval[cpt2] = Mval[cpt-1];
        cpt2++;
    }
    while (cpt2 < SPARSE_LIMIT/4) {
        Midx[cpt2] = 0;
        Mval[cpt2] = 0;
        cpt2++;
    }
}


bit_st* merge(bit_st* b, uint64_t* Mval, uint64_t* Midx, int sizeM){ // M is already sorted
    //merge b1 and M and returns bn
    int keyB = 0, valB = 0;
    int bit=0;
    int cptM=0;
    int b_isDone = 0;

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
printf("sizeM %d\n",sizeM);
printf("cptM %d\n",cptM);
printf("bit : %d nbit %d\n",bit,b->nbits);
        while (cptM < sizeM && b_isDone == 0){
printf("cptM %d\n",cptM);
            if (keyB < Midx[cptM]) {  
// first case ------------------------------------------------------------
                bitv_append(bn, keyB, valB); 
                if (bit > b->nbits) { 
                    b_isDone = 1;
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                }else {
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += P + SIZE_OF_VALUE;
                }
                printf("1ère\n");
            }else if (keyB > Midx[cptM]){
// second case ------------------------------------------------------------
                bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;
                if (cptM >= sizeM) {
                    bitv_append(bn, keyB, valB); 
                    while (bit < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                    b_isDone = 1;
                }
                printf("2ème\n");
            }else if (keyB == Midx[cptM]){
// third case ------------------------------------------------------------
                if (valB > Mval[cptM]) bitv_append(bn, keyB, valB);
                else bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;

                if ((bit+1) > b->nbits) { 
                    b_isDone = 1;
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
                    while (bit < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                    b_isDone = 1;
                }
                printf("3ème\n");
            }
        }
    }else if (b->mode == DENSE_MODE){
        bit = SIZE_OF_VALUE - 1;
    
        // First read of valB and Midx[cptM]
        bitv_readBits(b, &keyB, &valB, bit);
        bit += SIZE_OF_VALUE;
        // ------------------------
        
        while (cptM < sizeM && b_isDone == 0){
            if (keyB < Midx[cptM]) { 
// first case ------------------------------------------------------------
                bitv_append(bn, keyB, valB); 
                if (bit > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                    b_isDone = 1;
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
                    while (bit < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                    b_isDone = 1;
                }
            }else if (keyB == Midx[cptM]){
// third case ------------------------------------------------------------
                if (valB > Mval[cptM]) bitv_append(bn, keyB, valB);
                else bitv_append(bn, Midx[cptM], Mval[cptM]);
                cptM++;

                if (bit > b->nbits) { 
                    while (cptM < sizeM){
                        bitv_append(bn, Midx[cptM], Mval[cptM]);
                        cptM++;
                    }
                    b_isDone = 1;
                }else{
                    bitv_readBits(b, &keyB, &valB, bit);
                    bit += SIZE_OF_VALUE;
                }
                if (cptM > sizeM){
                    bitv_append(bn, keyB, valB); 
                    while (bit < b->nbits){
                        bitv_readBits(b, &keyB, &valB, bit);
                        bit += P + SIZE_OF_VALUE;
                        bitv_append(bn, keyB, valB); 
                    }
                    b_isDone = 1;
                }
            }
        }
    }
    
    printf("bit : %d cptM %d\n",bit,cptM);
    return bn;
}





int main(int argc, char *argv[]) {
    bit_st *b = bitv_alloc(11);
    b->mode = SPARSE_MODE;

    bitv_append(b,4,15);
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
    uint64_t* Mval = malloc(10*sizeof(uint64_t));
    uint64_t* Midx = malloc(10*sizeof(uint64_t));
    Midx[0] = 1;
    Mval[0] = 3;
    Midx[1] = 3;
    Mval[1] = 16;
    Midx[2] = 5;
    Mval[2] = 10;
    Midx[3] = 8;
    Mval[3] = 9;
    bit_st* bn = merge(b,Mval,Midx,4);
    printf("1 : %d\n",bitv_read(bn,1));
    printf("3 : %d\n",bitv_read(bn,3));
    printf("4 : %d\n",bitv_read(bn,4));
    printf("5 : %d\n",bitv_read(bn,5));
    printf("b5 %d\n",bitv_read(b,5));
    printf("6 : %d\n",bitv_read(bn,6));
    printf("b6 %d\n",bitv_read(b,6));
    printf("7 : %d\n",bitv_read(bn,7));
    printf("b7 %d\n",bitv_read(b,7));
    printf("8 : %d\n",bitv_read(bn,8));

    printf("get dense...\n");
    bn = getDense(bn);
    printf("8 : %d\n",bitv_read(bn,8));

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
