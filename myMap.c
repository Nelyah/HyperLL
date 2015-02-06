#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "myMap.h"
#include "bitStruct.h"
#include "deltaVarIntEncoder.h"
#include "deltaVarIntDecoder.h"


void unifTab(uint32_t* Mval, uint32_t* Midx, int* sizeM){
    /* removes all index duplicate in M, and keeps the one with highest value 
     * M should already be sorted */
    int max=0, idx=0, cpt=0, cpt2=0;
    while (cpt < *sizeM){
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
    *sizeM = cpt2;
}

uint32_t fusionInt(uint32_t idx, uint32_t value){
    // P should be < 26
    uint32_t val = 0;
    val += idx << 6;
    val += value;

    return val;
}

void splitInt(uint32_t* idx, uint32_t* value, uint32_t toSplit){

    *value = 0;
    *idx = 0;
    *value = toSplit & ((1 << SIZE_OF_VALUE)-1);
    toSplit >>= SIZE_OF_VALUE;
    *idx = toSplit & ((1 << P)-1);
}

bit_st* merge(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){
    bit_st* bn = NULL;
    if (b->mode == SPARSE_MODE) {
        bn = merge_sparse(b,Mval,Midx,sizeM);
    }else{
        bn = merge_dense(b,Mval,Midx,sizeM);
    }
    return bn;
}

bit_st* merge_sparse(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){ // M is already sorted
    //merge b1 and M and returns bn
    int cptM=0;
    uint32_t gn = 0; // stores all the getNext()
    uint32_t gn_idx = 0, gn_val = 0, tmpFus = 0;

    reset_delta();
    deltaVarIntDecoder(b->words);
    gn = getNext();

    // il faut trieeerr 
    unifTab(Mval, Midx, &sizeM);


    bit_st* bn = bitv_alloc(b->nbAlloc);
    bn->mode = b->mode;

    printf("gn %d\n",gn);
    while (cptM < sizeM && gn != -1){
        splitInt(&gn_idx, &gn_val, gn);
        printf("gn %d %d \n",gn_idx,gn_val);
        printf("M %d %d \n",Midx[cptM],Mval[cptM]);
        if (gn_idx < Midx[cptM]) {  
            // first case ------------------------------------------------------------
            tmpFus = fusionInt(gn_idx, gn_val);
            appendInt32(tmpFus, bn->words);
            gn = getNext();
        }else if (gn_idx > Midx[cptM]){
            // second case ------------------------------------------------------------
            tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
            appendInt32(tmpFus, bn->words);
            cptM++;
        }else if(gn_idx == Midx[cptM]){
            if (gn_val >= Mval[cptM]) {
                tmpFus = fusionInt(gn_idx, gn_val);
                appendInt32(tmpFus, bn->words);
                gn = getNext();
                cptM++;
            } else if (gn_val < Mval[cptM]){
                gn = getNext();
                tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
                appendInt32(tmpFus, bn->words);
                cptM++;
            }
        
        }
    }

    if (gn  == -1) {
        // old list is empty
        while (cptM < sizeM) {
            tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
            appendInt32(tmpFus, bn->words);
            cptM++;
        }
    }else{
        while (gn != -1) {
            appendInt32(gn, bn->words);
            gn = getNext();
        }
    }
    return bn;
}


bit_st* merge_dense(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){ // M is already sorted

    int bit, cptM = 0;
    int keyB, valB, b_isDone = 0;
    bit_st* bn = bitv_alloc(b->nbAlloc);
    bn->mode = b->mode;
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
                b_isDone = 1;
            }else {
                bitv_readBits(b, &keyB, &valB, bit);
                bit += SIZE_OF_VALUE;
            }
        }else if (Midx[cptM] > keyB){
            // second case ------------------------------------------------------------
            bitv_append(bn, Midx[cptM], Mval[cptM]);
            cptM++;
        }else if (keyB == Midx[cptM]){
            // third case ------------------------------------------------------------
            if (valB > Mval[cptM]) bitv_append(bn, keyB, valB);
            else bitv_append(bn, Midx[cptM], Mval[cptM]);
            cptM++;

            if (bit > b->nbits) { 
                b_isDone = 1;
            }else{
                bitv_readBits(b, &keyB, &valB, bit);
                bit += SIZE_OF_VALUE;
            }
        }
    }
    if (b_isDone == 1) {
        while (cptM < sizeM){
            bitv_append(bn, Midx[cptM], Mval[cptM]);
            cptM++;
        }

    }else if (b_isDone == 1){
        bitv_append(bn, keyB, valB); 
        while (bit < b->nbits){
            bitv_readBits(b, &keyB, &valB, bit);
            bit += SIZE_OF_VALUE;
            bitv_append(bn, keyB, valB); 
        }
    }
    return bn;

}


int main(int argc, char *argv[]) {
    bit_st *b = bitv_alloc(256);
    b->mode = SPARSE_MODE;



    reset_delta();
    uint32_t gn = 0;
    uint32_t idx, val;
    idx = 5; val =3;
    gn = fusionInt(idx,val);
    appendInt32(gn,b->words);
    deltaVarIntDecoder(b->words);
    printf("strl %d\n",(int)strlen(b->words));
    

//    bitv_append(b,4,15);
    printf("nbits %d\n",b->nbAlloc);
//    bitv_append(b,5,3);
    printf("nbits %d\n",b->nbAlloc);
//    bitv_append(b,6,3);
    idx = 6; val =3;
    gn = fusionInt(idx,val);
    appendInt32(gn,b->words);
    printf("nbits %d\n",b->nbAlloc);
//    bitv_append(b,7,3);
    idx = 7; val =3;
    gn = fusionInt(idx,val);
    appendInt32(gn,b->words);
    idx = 9; val =2;
    gn = fusionInt(idx,val);
    appendInt32(gn,b->words);
//    updateMax(b,7,4);
    printf("nbits %d\n",b->nbAlloc);
    idx = 800; val =8;
    gn = fusionInt(idx,val);
    appendInt32(gn,b->words);
//    bitv_append(b,800,3);
    printf("nbits %d\n",b->nbits);
    bitv_dump(b);
    printf("nbits %d\n",b->nbits);
    printf("nbAlloc %d\n",b->nbAlloc);
    printf("read : %d\n",bitv_read(b,7));
    uint32_t* Mval = malloc(10*sizeof(uint32_t));
    uint32_t* Midx = malloc(10*sizeof(uint32_t));
    Midx[0] = 1;
    Mval[0] = 3;
    Midx[1] = 3;
    Mval[1] = 16;
    Midx[2] = 5;
    Mval[2] = 10;
    Midx[3] = 8;
    Mval[3] = 9;
    //b = getDense(b);
    printf("read : %d\n",bitv_read(b,7));
    bit_st* bn = b;
    bn = merge(b,Mval,Midx,4);
    
    reset_delta();
    deltaVarIntDecoder(bn->words);
    printf("pouet\n");

    
    printf("Mode bn : %d , mode b : %d\n",bn->mode,b->mode);
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    //printf("3 : %d\n",bitv_read(bn,3));
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    //printf("4 : %d\n",bitv_read(bn,4));
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    //printf("5 : %d\n",bitv_read(bn,5));
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);
    gn = getNext();
    splitInt(&idx,&val, gn);
    printf("%d : %d\n",idx,val);

    printf("b5 %d\n",bitv_read(b,5));
    printf("6 : %d\n",bitv_read(bn,6));
    printf("b6 %d\n",bitv_read(b,6));
    printf("7 : %d\n",bitv_read(bn,7));
    printf("b7 %d\n",bitv_read(b,7));
    printf("8 : %d\n",bitv_read(bn,8));

    printf("get dense...\n");
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
