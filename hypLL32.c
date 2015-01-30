#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL.h"
#include <time.h>

#define BUF_SIZE 4096
#define HASH_SEED 0
#define P 14 // precision argument

char *buffer = NULL;
int *M = NULL;

void init(){
    M = calloc(pow(2,P),sizeof(int));
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

int lastBits(int n, int val){
// returns the last n bits of val
// val needs to be unsigned int 32bit
    uint32_t mask;
    mask = (1 << n) - 1;
    uint32_t returnVal = val & mask;
    return returnVal;
}

void addItem(uint32_t hashVal){
// Aggregation
    uint32_t idx, w;
    int clz=0;

    idx = hashVal >> (32 - P);
    w = lastBits((32-P), hashVal);
    w = w << P;
    clz = __builtin_clz(w)+1;
    if (M[idx] < clz) 
        M[idx] = clz;
       
}

float hyperLL_32bits(void){
    int i;
    int m = pow(2,P); // m : m = 2**p
    uint32_t hashVal[4]; // Only hashVal[0] will be used for a 32bit return value
    double a_m;
    switch (P){
        case 4:
            a_m = a_16;
            break;
        case 5:
            a_m = a_32;
            break;
        case 6:
            a_m = a_64;
            break;
        default:
            a_m = a_128(m);
    }


    //init();


/*    while(scanf("%s\n",buffer) != EOF){ // reads from standard input
        MurmurHash3_x86_32(buffer, strlen(buffer), HASH_SEED, hashVal);
        addItem(hashVal[0]);
    }*/
    
// Computation

    int nbRegist_0 = 0; // number of register equal to 0
    double rawEst=0, estim=0, sumComput=0;


    for (i = 0; i < m; i++) {
        sumComput += pow(2, (-M[i]));
        if (M[i] == 0) nbRegist_0++;
    }

    sumComput = pow(sumComput, -1);
    rawEst = a_m * (m*m) * sumComput;

    if (rawEst <= ((5.0/2.0)*m)) {
        if (nbRegist_0 != 0) {
           // estim = linearCounting(m, nbRegist_0);
           estim = m * log((double)(m)/nbRegist_0);
        }else{
            estim = rawEst;
       }
    }else if (rawEst <= ((1.0/30.0) * pow(2,32))) {
        estim = rawEst;
    }else{
        estim = -pow(2,32)*log(1 - (rawEst/pow(2,32)));
    }

    //printf("%f",estim);
    return estim;
}

/*int main(int argc, const char **argv) {
    hyperLL_32bits();
    return 0;
}*/
