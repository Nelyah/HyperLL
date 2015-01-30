#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "hypLL64.h"
#include <time.h>


int main(int argc, const char **argv) {
    int cpt = 0;
    clock_t start, end, total;
    uint64_t* valTab = NULL;

    valTab = malloc(2*pow(10,9)*sizeof(uint64_t));

    init();
    start = clock();
    while(cpt < pow(10,9)){ 
        addItem(valTab[cpt]);
        cpt++;
    }
    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
    printf("%d\n",(int)total);
    printf("%f\n",hypLL64);
    
    return 0;
}
