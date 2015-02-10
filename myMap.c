#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "myMap.h"
#include "bitStruct.h"
#include "deltaVarIntEncoder.h"
#include "deltaVarIntDecoder.h"
#include "sort.h"

int c =0;

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
        if (max > Mval[cpt-1]) Mval[cpt2] = max; 
        else Mval[cpt2] = Mval[cpt-1];
        cpt2++;
    }
    *sizeM = cpt2;
}

uint32_t fusionInt(uint32_t idx, uint32_t value){
/* Useful for varint encoding, returns the value which 
 * should be passed to the encoder
 */
    // P should be < 26
    uint32_t val = 0;
    val += idx << 6;
    val += value;

    return val;
}

void splitInt(uint32_t* idx, uint32_t* value, uint32_t toSplit){
/* fill the two arguments with the right values. toSplit should be 
 * the value returned by varint decoder
 */

    *value = 0;
    *idx = 0;
    *value = toSplit & ((1 << SIZE_OF_VALUE)-1);
    toSplit >>= SIZE_OF_VALUE;
    *idx = toSplit & ((1 << P)-1);
}

bit_st* merge(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){
/* merge function, will call the right function depending on the mode 
 * of the bitmap
 */
    bit_st* bn = NULL;
    if (b->mode == SPARSE_MODE) {
        bn = merge_sparse(b,Mval,Midx,sizeM);
    }else{
        bn = merge_dense(b,Mval,Midx,sizeM);
    }
    bitv_free(b);
    return bn;
}

bit_st* merge_sparse(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){ // M is already sorted
/* The bitmap should be varint encoded. The two tabs are those filled in 
 * the hypLL64 file, and are those we want to merge with our bitmap. 
 * This returns a bitmap varint encoded 
 */
    //merge b1 and M and returns bn
    int cptM=0;
    uint32_t gnTmp = 0, gn = 0; // stores all the getNext()
    uint32_t gn_idx = 0, gn_val = 0, tmpFus = 0;
    word_t* b_words_init = b->words;
    bit_st* b_init = b;
    reset_delta();
    deltaVarIntDecoder(b->words, b->cptW);
    gn = getNext();

    quickSort_idx_val(Midx,Mval,0,sizeM-1);
    unifTab(Mval, Midx, &sizeM);


    bit_st* bn = NULL;
    bn = bitv_alloc(b_init->nbAlloc, b_init->mode);

    while (cptM < sizeM && gn != -1){
        splitInt(&gn_idx, &gn_val, gn);
        if (gn_idx < Midx[cptM]) {  
            // first case ------------------------------------------------------------
            appendInt32(gn, bn);
            gnTmp = gn;
            while(gn == gnTmp)
                gn = getNext();
        }else if (gn_idx > Midx[cptM]){
            // second case ------------------------------------------------------------
            tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
            appendInt32(tmpFus, bn);
            cptM++;
        }else if(gn_idx == Midx[cptM]){
            // third case ------------------------------------------------------------
            if (gn_val >= Mval[cptM]) {
                appendInt32(gn, bn);
                gn = getNext();
                cptM++;
            } else {
                gn = getNext();
                tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
                appendInt32(tmpFus, bn);
                cptM++;
            }
        
        }
    }

    if (gn  == -1) {
        // old list is empty
        while (cptM < sizeM) {
            tmpFus = fusionInt(Midx[cptM], Mval[cptM]);
            appendInt32(tmpFus, bn);
            cptM++;
        }
    }else{
        // tabs are empty
        while (gn != -1) {
            appendInt32(tmpFus, bn);
            gn = getNext();
        }
    }
    b = b_init;
    b->words = b_words_init;
    reset_varintDecoder();
    return bn;
}


bit_st* merge_dense(bit_st* b, uint32_t* Mval, uint32_t* Midx, int sizeM){ // M is already sorted
/* Same as the other merge function, but will be called on a dense bitmap 
 * and returns a dense bitmap.
 */

// *********************  SPECIAL ANNOTATION  *************************
// This function was an honorable oppenent, but we get the best of it !
// ********************************************************************

    int bit, cptM = 0;
    int keyB, valB, b_isDone = 0;
    bit_st* bn = NULL;
    bn = bitv_alloc(b->nbAlloc, b->mode);
    bit = SIZE_OF_VALUE - 1;
    quickSort_idx_val(Midx,Mval,0,sizeM-1);
    unifTab(Mval, Midx, &sizeM);

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
        }else if (Midx[cptM] < keyB){
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

    }else if (cptM >= sizeM){
        bitv_append(bn, keyB, valB); 
        while (bit < b->nbits){
            bitv_readBits(b, &keyB, &valB, bit);
            bit += SIZE_OF_VALUE;
            bitv_append(bn, keyB, valB); 
        }
    }
    return bn;

}

