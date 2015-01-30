#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "hypLL.h"
#include <time.h>


int main(int argc, const char **argv) {
    int cpt = 0;
    clock_t start, end, total;
    int* valTab = NULL;

    valTab = malloc(2*pow(10,9)*sizeof(int));

    init();
    start = clock();
    while(cpt < pow(10,9)){ 
        addItem(valTab[cpt]);
        cpt++;
    }
    end = clock();
    total = (double)(end - start)*1000 / CLOCKS_PER_SEC;
    printf("%d\n",(int)total);
    
    return 0;
}
