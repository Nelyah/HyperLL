#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "hypLL64.h"
#include <time.h>


#define BUF_SIZE 4096
#define HASH_SEED 0
#define P 14 // precision argument



int main(int argc, const char **argv) {
    //hyperLL_32bits();
    uint64_t hashVal[4];
    int cpt = 0, cpt2 = 0;
    int i,j;
    clock_t start, end, total;
    uint64_t* valTab = NULL;

    valTab = malloc(2*pow(10,9)*sizeof(uint64_t));
    init();

    for (i = 0; i < 100; i++) {
        for (j = 0; j < pow(10,7); j++) {
            MurmurHash3_x64_128(&j, sizeof(int), HASH_SEED, hashVal);
            valTab[cpt2] = hashVal[0];
            cpt2++;
        }
        for (j = 0; j < pow(10,7); j++) {
            MurmurHash3_x64_128(&j, sizeof(int), HASH_SEED, hashVal);
            valTab[cpt2] = hashVal[0];
            cpt2++;
        }
        printf("%d\n",i);
    }
    
    start = clock();
    while(cpt < cpt2){ 
        addItem(valTab[cpt2]);
        cpt++;
    }
    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
    printf("%d\n",(int)total);
    
    return 0;
}   
