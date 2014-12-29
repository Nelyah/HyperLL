#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"

#define BUF_SIZE 4096
#define HASH_SEED 0
#define P 12 // precision argument


int lastBits(int n, int val){
// returns the last n bits of val
    uint32_t mask;
    mask = (1 << n) - 1;
    uint32_t returnVal = val & mask;
    return returnVal;
}

float hyperLL_32bits(void){
    char *buffer = NULL;
    int *M = NULL;
    int rc, i, clz=0;
    int m = pow(2,P); // m : m = 2**p
    uint32_t hashVal[4]; // Only hashVal[0] will be used for a 32bit return value
    uint32_t idx, w;
    double a_m = 0.7213/(1+0.079/m); // for m >= 128 ??


    buffer = (char*) malloc(BUF_SIZE*sizeof(char));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    M = (int*) calloc(m,sizeof(int)); 
    if (M == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

// Aggregation

    while((rc = scanf("%s\n",buffer)) != EOF){ // reads from standard input
        MurmurHash3_x86_32(buffer, strlen(buffer), HASH_SEED, hashVal);
        idx = hashVal[0] >> (32 - P);
        w = lastBits((32-P), hashVal[0]);
        w = w << P;
        clz = __builtin_clz(w)+1;
        if (M[idx] < clz) 
            M[idx] = clz;
    }
    
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

    free(M);
    free(buffer);
    printf("%f",estim);
    return estim;
}




int main(int argc, const char **argv) {
    hyperLL_32bits();
    
    return 0;
}
