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
#define HASHED_VALUES 5000000
#define MAX_CARD 100000


int main(int argc, const char **argv) {
    //hyperLL_32bits();
    int step = atoi(argv[1]);
    int nbExp = atoi(argv[2]);
    uint64_t hashVal[4];
    int cpt = 0;
    int idx,i,j,k;
    double estim = 0;
    clock_t start, end, total;
    uint64_t* valTab = NULL;
    float* tabEstim = NULL;
    char* fileRead = NULL;
    srand(time(NULL));

    FILE* f = fopen("plot.txt","w");
    valTab = malloc(HASHED_VALUES*sizeof(uint64_t));
    fileRead = malloc(500*sizeof(char));
    tabEstim = malloc((nbExp*(MAX_CARD/step)+1)*sizeof(float));
    hyperLL_64bits();
    init();
    strcpy(fileRead,"plot_raw_1000.txt");

    printf("hachage ...\n");
    for (i = 0; i < HASHED_VALUES; i++) {
        MurmurHash3_x64_128(&i, sizeof(int), HASH_SEED, hashVal);
        valTab[cpt] = hashVal[0];
        cpt++;
    }
    
    printf("hachage fini.\n");
    start = clock();
    for (i = 0; i < nbExp; i++) {
        idx = rand() % (HASHED_VALUES - MAX_CARD);
        for (j = 0; j < MAX_CARD; j+=step) {
            if(j!=0){
                for (k = idx; k < (idx+step); k++) {
                    addItem(valTab[k]);
                }
            }
            estim = 0;
            merge_tabs();
            estim = count_raw(); // Count raw here, change to count() for a 
                                    // better estimation
            tabEstim[i+(j/step)*nbExp] = estim;
            idx+=step;
        }
        reset();
        /*if(i%50==0)*/ printf("i : %d  (estim : %f)\n",i,estim);
    }
    printf("addItem done.\n");


    // sort by Experience
    for (i = 0; i < MAX_CARD/step; i+=nbExp) {
      quickSort(tabEstim, i, i+(nbExp-1));
    }

    float  median, pct01, pct99;
    double mean,sum =0;
    int cardCur = 0;
    for (i = 0; i < MAX_CARD; i+=step) {
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
