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
/* This function is the main used to perform the 
 * relative error calcul. One should give 2 parameters when 
 * running it : The step as the first argument, and the number
 * of experiments as the second.
 */
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

    FILE* f = fopen("plot_file_relative_corrige.txt","w");
    valTab = malloc(HASHED_VALUES*sizeof(uint64_t));
    if (valTab == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    fileRead = malloc(500*sizeof(char));
    tabEstim = malloc((nbExp*(MAX_CARD/step)+1)*sizeof(float));
    init();
    strcpy(fileRead,"plot_raw_1000.txt");

    //filling the array of hashed value
    for (i = 0; i < HASHED_VALUES; i++) {
     MurmurHash3_x64_128(&i, sizeof(int), HASH_SEED, hashVal);
     valTab[cpt] =hashVal[0] ;
     cpt++;
    }
    
    start = clock();
    hyperLL_64bits();
    for (i = 0; i < nbExp; i++) {
      idx = rand() % (HASHED_VALUES - MAX_CARD);
      for (j = 0; j < MAX_CARD; j+=step) {
	if(j!=0){
	  for (k = idx; k < (idx+step); k++) {
	    addItem(valTab[k]);
	  }
	}
	estim = count_file(fileRead);
	tabEstim[i+(j/step)*nbExp] = estim;
	idx+=step;
      }
      reset();
      if(i%10 == 0)printf("%d\n",i);
    }

    // sort by Experience
    for (i = 0; i < MAX_CARD/step; i+=nbExp) {
        quickSort(tabEstim, i, i+(nbExp-1));
    }

    float sum = 0, mean;
    int cardCur = 0;
    float relative;
    for (i = 0; i < MAX_CARD; i+=step) {
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
