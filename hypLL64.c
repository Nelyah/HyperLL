#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL64.h"
#include "bitStruct.h"
#include "myMap.h"
#include "deltaVarIntDecoder.h"
#include "deltaVarIntEncoder.h"
#include <time.h>
#include <inttypes.h>

#define FILE_SIZE 1000

uint32_t *Mval = NULL;
uint32_t *Midx = NULL;
int m;
double a_m;
int m_size;
int* tabCard = NULL;
float* tabMean = NULL;
int cpt_file_size;
int loaded=0;
int cptM = 0;
int nbRegist_0 = 0; // number of register equal to 0
bit_st* bs = NULL;

void loadFile(char* filename){
    tabCard = malloc(FILE_SIZE*sizeof(int));
    tabMean = malloc(FILE_SIZE*sizeof(float));
    int card;
    float mean, median, pct01,pct99;
    cpt_file_size=0;
    FILE* f = fopen(filename,"r");

    while(fscanf(f,"%d %f %f %f %f",&card,&mean,&median,&pct01,&pct99) != EOF) {
        tabCard[cpt_file_size] = card;
        tabMean[cpt_file_size] = mean;
        cpt_file_size++;
    }
    loaded=1;
    fclose(f);
}



float extrapol(float* tabX, int* tabY, int size, float observed) {
    int i=0;
    while (i < size && tabX[i] < observed) {
        i++;
    }
    if (i==size) return tabY[i-1] * observed/tabX[i-1];
    if (i == 0) return tabY[0] * observed/tabX[0];
    if (tabX[i] == observed) return tabY[i];

    float estim;
    
    float prevX = tabX[i-1]; 
    float nextX = tabX[i];
    float prevY = tabY[i-1];
    float nextY = tabY[i];
    estim = prevY+(observed-prevX)*(nextY-prevY)/(nextX-prevX);
    return estim;
}

void init(){
    m_size = (1 << P);
    Midx = calloc(m_size,sizeof(uint32_t));
    if (Midx == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    Mval = calloc(m_size,sizeof(uint32_t));
    if (Mval == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    cptM = 0; 
    bs = bitv_alloc(64, SPARSE_MODE);
}

void resetSmallTabs(){
    memset(Midx,0,m_size);
    memset(Mval,0,m_size);
    cptM=0;
}

void reset(){
    nbRegist_0 = 0;
    memset(Midx,0,m_size);
    memset(Mval,0,m_size);
    cptM=0;
    int mode = bs->mode;
    bitv_free(bs);
    bs = bitv_alloc(64,mode);
}

void freeAll(){
    bitv_free(bs);
    free(Mval);
    free(Midx);
}



uint64_t lastBits(int n, uint64_t val){
// returns the last n bits of val
// val needs to be unsigned int 64bits
    uint64_t mask;
    mask = ((uint64_t)(1) << n) - 1;
    uint64_t returnVal = val & mask;
    return returnVal;
}

void addItem(uint64_t hashVal){
// Aggregation
    uint64_t idx, w;
    int clz=0;
    bit_st* tmpbs = bs;

    idx = (uint64_t)(hashVal) >> (uint64_t)(64 - P);
    w = lastBits((64-P), hashVal);
    w = (uint64_t)(w) << P;
    clz = __builtin_clz(w)+1;
    Mval[cptM] = clz;
    Midx[cptM] = idx;
    cptM++;
    if (cptM == SPARSE_LIMIT/4) {
        bs = NULL;
        bs = merge(tmpbs,Mval,Midx,cptM);
        resetSmallTabs();
    }
}

void merge_tabs(){
//    printf("cptM %d\n",cptM);
    bit_st* tmpbs = bs;
    bs = NULL;
    bs = merge(tmpbs, Mval, Midx, cptM);
    resetSmallTabs();
}

float count_raw(){
    float rawEst=0, sumComput=0;
    int i, cpt = 0;
    nbRegist_0 = 0;
      
//      printf("0\n");
//      printf("0 %d\n",bs->mode);
    if (bs->mode == DENSE_MODE) {
        int val;
        for (i = 0; i < m; i++) {
            val = bitv_read(bs,i);
            sumComput += 1.0/(1 << val);
            if (val == 0) nbRegist_0++;
        }
    }else if (bs->mode == SPARSE_MODE){
        uint32_t gn, idx = 0, val = 0;
        word_t* bs_w = bs->words;
        reset_delta();
        deltaVarIntDecoder(bs->words, bs->cptW);
        printf("rawcount\n");
        while((gn = getNext()) != -1){
            splitInt(&idx, &val, gn);
            //printf("%d %d\n",idx,val);
            sumComput += 1.0/(1 << val);
            cpt++;
        }
        printf("cpt %d\n",cpt);
        bs->words = bs_w;
        nbRegist_0 = (m-cpt);
        //printf("cpt : %d\n",cpt);
        //printf("nbReg %d\n",nbRegist_0);
        sumComput += (m-cpt);
    }
    sumComput = 1.0/sumComput;


    rawEst = a_m * (uint64_t)(m)*(uint64_t)(m) * sumComput;
    return rawEst;
}

float count_file(char* filename){
  float rawEst=0, corrected=0; 
  if (loaded != 1) loadFile(filename);

  rawEst = count_raw();
  corrected = extrapol(tabMean, tabCard, cpt_file_size, rawEst);

  return corrected;
}

float count_linearCounting(){
  float rawEst=0, estim=0;
  rawEst = count_raw();
  if (nbRegist_0 != 0) {
    // estim = linearCounting(m, nbRegist_0);
    estim = m * log((double)(m)/nbRegist_0);
  }else{
    estim = rawEst;
  }
  return estim;
}

float count(){
    double rawEst=0, estim=0;
    rawEst = count_raw();

    if (rawEst <= ((5.0/2.0)*m)) {
        if (nbRegist_0 != 0) {
           // estim = linearCounting(m, nbRegist_0);
           estim = m * log((double)(m)/nbRegist_0);
        }else{
            estim = rawEst;
       }
    }else if (rawEst <= ((1.0/30.0) * pow(2,64))) {
        estim = rawEst;
    }else{
        estim = -pow(2,64)*log(1 - (rawEst/pow(2,64)));
    }
    return estim;
}


float hyperLL_64bits(void){
    m = 1 << P; // m : m = 2**p
    switch (P){
        case 4:
            a_m = a_16;
            break;
        case 5:
            a_m = a_64;
            break;
        case 6:
            a_m = a_64;
            break;
        default:
            a_m = a_128(m);
    }
    return 0;
}


