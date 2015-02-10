#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "fromInput.h"
#include "hypLL64.h"
#include "murmur3.h"


#define BUF_SIZE 4096
#define HASH_SEED 0


char *buffer = NULL;

int main(int argc, const char **argv) {
/* Fonction main qui permet de lire depuis l'entrée standard */
    
    uint64_t hashVal[4]; // Only hashVal[0] will be used for a 64bit return value

    init();
    buffer = malloc(BUF_SIZE*(sizeof(char)));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    hyperLL_64bits();

    while(scanf("%s\n",buffer) != EOF){ // reads from standard input
        MurmurHash3_x64_128(buffer, strlen(buffer), HASH_SEED, hashVal);
        addItem(hashVal[0]);
    }
    merge_tabs();
    float estim = count();
    printf("%f\n",estim);
    free(buffer);
    return 0;
}
