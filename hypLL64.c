#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL64.h"
#include <time.h>
#include <inttypes.h>

#define FILE_SIZE 1000

uint64_t *M = NULL;
int m;
double a_m;
int m_size;
int* tabCard = NULL;
float* tabMean = NULL;
int cpt_file_size;
int loaded=0;
int nbRegist_0 = 0; // number of register equal to 0

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
    m_size = pow(2,P);
    M = calloc(m_size,sizeof(uint64_t));
    if (M == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
}

void reset(){
  nbRegist_0 = 0;
  int i;
  for (i=0; i<m_size; i++) M[i]=0;
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

float count_raw(){
  float rawEst=0, sumComput=0;
  int i;
  
  for (i = 0; i < m; i++) {
    sumComput += 1.0/pow(2, M[i]);
    if (M[i] == 0) nbRegist_0++;
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
    return 0;
}


