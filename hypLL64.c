#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL64.h"
#include <time.h>
#include <inttypes.h>

#define BUF_SIZE 4096
#define HASH_SEED 0
#define P 14 // precision argument

char *buffer = NULL;
uint64_t *M = NULL;
int m;
double a_m;

void init(){
    M = calloc(pow(2,P),sizeof(uint64_t));
    if (M == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    buffer = malloc(BUF_SIZE*(sizeof(char)));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void freeAll(){
    free(M);
    free(buffer);
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
    int nextIsBest = 0;
    fscanf(f,"%f %f %f %f %f %f",&card,&estimCard,&mean,&median,&pct01,&pct99);
    bestMean = mean;
    tmpPrev = mean;
    while(fscanf(f,"%f %f %f %f %f %f",&card,&estimCard,&mean,&median,&pct01,&pct99) != EOF) {
        if (nextIsBest == 1) {
            nextMean = mean;
            nextIsBest = 0;
        }
        if (fabs(rawEst-mean) < fabs(rawEst-bestMean)) {
            bestMean = mean;
            prevMean = tmpPrev;
            nextIsBest = 1;
        }
        tmpPrev = mean;
    }
    estim = (nextMean+prevMean)/2;

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
    uint64_t hashVal[4]; // Only hashVal[0] will be used for a 64bit return value
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

    while(scanf("%s\n",buffer) != EOF){ // reads from standard input
        MurmurHash3_x64_128(buffer, strlen(buffer), HASH_SEED, hashVal);
        addItem(hashVal[0]);
    }
    
// Computation
    
    //float estim = count();

    //printf("%f",estim);
    return 0;
}

int main(int argc, const char **argv) {
    init();
    hyperLL_64bits();
    float estim = count();
    printf("%f\n",estim);
    return 0;
}
