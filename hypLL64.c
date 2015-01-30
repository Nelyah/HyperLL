#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL64.h"
#include <time.h>
#include <inttypes.h>

#define P 14 // precision argument

uint64_t *M = NULL;
int m;
double a_m;
int m_size;

void init(){
  m_size = pow(2,P);
    M = calloc(m_size,sizeof(uint64_t));
    if (M == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
}

void reset(){
  memset(M, 0, m_size);
}

void freeAll(){
    free(M);
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

    idx = (uint64_t)(hashVal) >> (uint64_t)(64 - P);
    w = lastBits((64-P), hashVal);
    w = (uint64_t)(w) << P;
    clz = __builtin_clz(w)+1;
    if (M[idx] < clz) {
        M[idx] = clz;
    }
    
}

float count_file(char* filename){ // Uses file to predict corrected estimation
    int nbRegist_0 = 0; // number of register equal to 0
    double rawEst=0, estim=0, sumComput=0;
    rawEst=0;
    int i;
    
    FILE* f = fopen(filename,"r");

    for (i = 0; i < m; i++) {
        sumComput += 1.0/pow(2, M[i]);
        if (M[i] == 0) nbRegist_0++;
    }

    sumComput = 1.0/sumComput;
    rawEst = a_m * (uint64_t)(m)*(uint64_t)(m) * sumComput;
    float card, estimCard, mean, median, pct01, pct99;
    float prevMean=0, nextMean=0, bestMean=0;
    float tmpPrev;
    int nextIsBest = 0, prevCard = 0, nextCard = 0, prevCardTmp;
    fscanf(f,"%f %f %f %f %f %f",&card,&estimCard,&mean,&median,&pct01,&pct99);
    bestMean = mean;
    tmpPrev = mean;
    while(fscanf(f,"%f %f %f %f %f %f",&card,&estimCard,&mean,&median,&pct01,&pct99) != EOF) {
        if (nextIsBest == 1) {
            nextMean = mean;
            nextCard = card;
            nextIsBest = 0;
        }
        if (fabsf(rawEst-mean) < fabsf(rawEst-bestMean)) {
            bestMean = mean;
            prevMean = tmpPrev;
            prevCard = prevCardTmp;
            nextIsBest = 1;
        }
        tmpPrev = mean;
        prevCardTmp = card;
    }
    estim = (nextMean+prevMean)/2;
    estim = ((rawEst-prevMean)/(nextMean-prevMean))*prevCard;
    estim += ((nextMean-rawEst)/(nextMean-prevMean))*nextCard;

    fclose(f);
    return estim;
}

float count_raw(){
    int nbRegist_0 = 0; // number of register equal to 0
    double rawEst=0, sumComput=0;
    rawEst=0;
    int i;


    for (i = 0; i < m; i++) {
        sumComput += 1.0/pow(2, M[i]);
        if (M[i] == 0) nbRegist_0++;
    }

    sumComput = 1.0/sumComput;
    rawEst = a_m * (uint64_t)(m)*(uint64_t)(m) * sumComput;
    // raw HLL
    return rawEst;
}

float count(){
    int nbRegist_0 = 0; // number of register equal to 0
    double rawEst=0, estim=0, sumComput=0;
    rawEst=0;
    int i;


    for (i = 0; i < m; i++) {
        sumComput += 1.0/pow(2, M[i]);
        if (M[i] == 0) nbRegist_0++;
    }

    sumComput = 1.0/sumComput;
    rawEst = a_m * (uint64_t)(m)*(uint64_t)(m) * sumComput;
    // raw HLL
    //return rawEst;

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
    m = pow(2,P); // m : m = 2**p
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

    
// Computation
    
    //float estim = count();

    //printf("%f",estim);
    return 0;
}


