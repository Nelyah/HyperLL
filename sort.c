#include "sort.h"
#include <time.h>
#include <stdlib.h>


int partition(float* tab, int first, int last, int pivot){
    float tmp;

    tmp = tab[pivot];
    tab[pivot] = tab[last];
    tab[last] = tmp;

    int j = first, i;
    for (i = first; i < last; i++) {
        if (tab[i] <= tab[last]) {
            tmp = tab[i];
            tab[i] = tab[j];
            tab[j] = tmp;
            j++;
        }
    }

    tmp = tab[last];
    tab[last] = tab[j];
    tab[j] = tmp;
    return j;
}

float* quickSort(float* tab, int first, int last){
    int pivot = 0;
    srand(time(NULL));

    if (first < last) {
        pivot = rand() % (last-first);
        pivot += first;
        pivot = partition(tab, first, last, pivot);
        quickSort(tab, first, pivot-1);
        quickSort(tab, pivot+1, last);
    }
    return tab;
}

