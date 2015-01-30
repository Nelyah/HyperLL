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

    FILE* f = fopen("plot.txt","w");
    valTab = malloc(HASHED_VALUES*sizeof(uint64_t));
    if (valTab == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    fileRead = malloc(500*sizeof(char));
    tabEstim = malloc((nbExp*(100000/step)+1)*sizeof(float));
    init();
    strcpy(fileRead,"plot_1000.txt");

    //filling the array of hashed value
    for (i = 0; i < HASHED_VALUES; i++) {
     MurmurHash3_x64_128(&i, sizeof(int), HASH_SEED, hashVal);
     valTab[cpt] =hashVal[0] ;
     cpt++;
    }
    
    start = clock();
    for (i = 0; i < nbExp; i++) {
        idx = rand() % (HASHED_VALUES - 100000);
        for (j = 0; j < 100000; j+=step) {
            if(j!=0){
                for (k = idx; k < (idx+step); k++) {
                    addItem(valTab[k]);
                }
            }
            hyperLL_64bits();
            estim = count_raw();
            tabEstim[i+(j/step)*nbExp] = estim;
            idx+=step;
        }
        reset();

	printf("%d\n",i);
    }
    printf("addItem done.\n");


// sort by Experience
    for (i = 0; i < 100000/step; i+=nbExp) {
        quickSort(tabEstim, i, i+(nbExp-1));
    }

    float sum = 0, mean;
    int cardCur = 0;
    float relative;
    for (i = 0; i < 100000; i+=step) {
        sum = 0;
        for (j = 0; j < nbExp; j++) {
            sum += tabEstim[j+(i/step)*nbExp];
        }
        mean = sum/nbExp;
        if (i != 0) {
            relative = fabsf((mean-cardCur)/cardCur);
            fprintf(f,"%d %f\n",cardCur, relative);
        }
        cardCur += step;
    }



    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
    printf("%d\n",(int)total);
    
    fclose(f);
    return 0;
}
