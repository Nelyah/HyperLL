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



int main(int argc, const char **argv) {
    //hyperLL_32bits();
    int step = atoi(argv[1]);
    int nbStep = atoi(argv[2]);
    int hashVal[4];
    int i,j;
    int cpt = 0;
    clock_t start, end, total;
    int* valTab = NULL;
    srand(time(NULL));

    valTab = malloc(55000000*sizeof(int));
    init();

    
    for (i = 0; i < 50000000; i++) {
        MurmurHash3_x86_32(&i, sizeof(int), HASH_SEED, hashVal);
        valTab[i] = hashVal[0];
    }
    
    start = clock();
    int startJ = rand()%(50000000-(step*nbStep));
    for (i = 0; i < nbStep; i++) {
        for (j = startJ; j < startJ+step; j++) {
            addItem(valTab[j]);
        }
        float estim = hyperLL_32bits();
        cpt+=100;
        printf("%f  %d\n",estim,cpt);

        startJ += step;
    }

    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
//    printf("%d\n",(int)total);
    freeAll();
    
    return 0;
}
