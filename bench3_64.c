#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "murmur3.h"
#include "sort.h"
#include "hypLL64.h"
#include <time.h>


#define BUF_SIZE 4096
#define HASH_SEED 0
#define P 14 // precision argument



int main(int argc, const char **argv) {
    //hyperLL_32bits();
    int step = atoi(argv[1]);
    int nbExp = atoi(argv[2]);
    uint64_t hashVal[4];
    int cpt = 0;
    int idx,i,j,k;
    float estim = 0;
    clock_t start, end, total;
    uint64_t* valTab = NULL;
    float* tabEstim = NULL;
    char* fileRead = NULL;
    srand(time(NULL));

    FILE* f = fopen("plot_acc_1000.txt","w");
    valTab = malloc(2*pow(10,9)*sizeof(uint64_t));
    fileRead = malloc(500*sizeof(char));
    tabEstim = malloc((nbExp*(100000/step)+1)*sizeof(float));
    init();
    strcpy(fileRead,"plot_1000.txt");

printf("coucou\n");
    for (i = 0; i < 50000000; i++) {
        MurmurHash3_x64_128(&i, sizeof(int), HASH_SEED, hashVal);
        valTab[cpt] = hashVal[0];
        cpt++;
    }
    
    printf("coucou\n");
    start = clock();
    for (i = 0; i < nbExp; i++) {
        idx = rand() % (50000000 - 100000);
        for (j = 0; j < 100000; j+=step) {
            for (k = idx; k < (idx+step); k++) {
                addItem(valTab[k]);
            }
            hyperLL_64bits();
            estim = count();
            tabEstim[i+(j/step)*nbExp] = estim;
            idx+=step;
        }
        freeAll();
        init();
        printf("i : %d  (estim : %f)\n",i,estim);
    }
    printf("addItem done.\n");


// sort by Experience
    for (i = 0; i < 100000; i+=nbExp) {
        quickSort(tabEstim, i, i+(nbExp-1));
    }

    float sum = 0, mean, median, pct01, pct99;
    int cardCur = 0;
    for (i = 0; i < 100000; i+=step) {
        sum = 0;
        for (j = 0; j < nbExp; j++) {
            sum += tabEstim[j+(i/step)*nbExp];
        }
        mean = sum/nbExp;
        pct01 = tabEstim[(int)((i/step)*nbExp+0.01*nbExp)];
        pct99 = tabEstim[(int)((i/step)*nbExp+0.99*nbExp)];
        median = tabEstim[(int)((i/step)*nbExp+0.5*nbExp)];
        fprintf(f,"%d %f %f %f %f\n",cardCur, mean, median, pct01, pct99);
        cardCur += step;
    }



    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
    printf("%d\n",(int)total);
    
    fclose(f);
    return 0;
}
