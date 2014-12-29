#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include "murmur3.h"

#define BUF_SIZE 1024
#define HASH_SEED 0
#define P 14 // precision argument
#define MAX_THREAD 200

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
volatile int n,nbPthread=0,maxThread;
volatile int* M = NULL;

int lastBits(int n, int val){
// returns the last n bits of val
    uint32_t mask;
    mask = (1 << n) - 1;
    uint32_t returnVal = val & mask;
    return returnVal;
}

void *aggregation(void){
    
    int rc, clz=0;
    int *M_tab = (int*) M;
    char buffer[BUF_SIZE];
    uint32_t idx, w;
    uint32_t hashVal[4]; // Only hashVal[0] will be used for a 32bit return value

        pthread_mutex_lock(&lock);
    rc = scanf("%s\n",buffer);
        pthread_mutex_unlock(&lock);
    
    while(rc != EOF){ // reads from standard input
        pthread_mutex_lock(&lock);
    rc = scanf("%s\n",buffer);
        pthread_mutex_unlock(&lock);
//        printf("%s\n",buffer);
        MurmurHash3_x86_32(buffer, strlen(buffer), HASH_SEED, hashVal);
    //    printf("%d\n",nbPthread);
        idx = hashVal[0] >> (32 - P);
        w = lastBits((32-P), hashVal[0]);
        w = w << P;
        clz = __builtin_clz(w)+1;

        pthread_mutex_lock(&lock);
        if (M_tab[idx] < clz) 
            M_tab[idx] = clz;
        pthread_mutex_unlock(&lock);
    }
        pthread_mutex_lock(&lock);
        nbPthread--;
        pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
    return (void*)M_tab;
}




float hyperLL_32bits(void){
    char *buffer = NULL;
    pthread_t *tabThread = NULL;
    int rc, i;
    int m = pow(2,P); // m : m = 2**p
    double a_m = 0.7213/(1+0.079/m); // for m >= 128 ??
    
    tabThread = (pthread_t*)malloc(MAX_THREAD*sizeof(pthread_t));
    if(tabThread == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    buffer = (char*) malloc(BUF_SIZE*sizeof(char));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    M = (volatile int*) calloc(m,sizeof(volatile int)); 
    if (M == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

// Aggregation
    /*for (i = 0; i < MAX_THREAD; i++) {
        pthread_mutex_lock(&lock);
        nbPthread++;
        pthread_mutex_unlock(&lock);
        rc=pthread_create(&tabThread[nbPthread-1],NULL,(void*)aggregation,NULL);
    }*/
    pthread_t thread1, thread2, thread3;
    int status;
    status = pthread_create (
            &thread1, NULL, (void*)aggregation, NULL);
    if (status != 0){
        perror("pthread");
        exit(EXIT_FAILURE);
    }
    status = pthread_create (
            &thread2, NULL, (void*)aggregation, NULL);
    if (status != 0){
        perror("pthread");
        exit(EXIT_FAILURE);
    }
    status = pthread_create (
            &thread3, NULL, (void*)aggregation, NULL);
    if (status != 0){
        perror("pthread");
        exit(EXIT_FAILURE);
    }
    //aggregation();
while(nbPthread>0) {}
printf("test\n");
    printf("re\n");
    
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

//    free((void*)M);
    free(buffer);
    printf("%f",estim);
    return estim;
}




int main(int argc, const char **argv) {
    hyperLL_32bits();
    printf("coucou\n");
    
    return 0;
}
