#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char **argv) {
    FILE* fileW = fopen("card2.txt","w");
    srand(time(NULL));
    int r=0;
    int i;
    for (i = 0; i < 10000000; i++) {
        r = random()%100000;
        fprintf(fileW,"%d\n",r);
    }
    fclose(fileW);

    return 0;
}
