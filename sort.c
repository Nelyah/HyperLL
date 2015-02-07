#include "sort.h"
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>

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

int partition_idx_val(uint32_t* tab, uint32_t* Tval, int first, int last, int pivot){
    float tmp,tmp2;

    tmp = tab[pivot];
    tmp2 = Tval[pivot];
    tab[pivot] = tab[last];
    Tval[pivot] = Tval[last];
    tab[last] = tmp;
    Tval[last] = tmp2;

    int j = first, i;
    for (i = first; i < last; i++) {
        if (tab[i] <= tab[last]) {
            tmp = tab[i];
            tmp2 = Tval[i];
            tab[i] = tab[j];
            Tval[i] = Tval[j];
            tab[j] = tmp;
            Tval[j] = tmp2;
            j++;
        }
    }

    tmp = tab[last];
    tmp2 = Tval[last];
    tab[last] = tab[j];
    Tval[last] = Tval[j];
    tab[j] = tmp;
    Tval[j] = tmp2;
    return j;
}

uint32_t* quickSort_idx_val(uint32_t* tab, uint32_t* Tval, int first, int last){
    int pivot = 0;
    srand(time(NULL));

    if (first < last) {
        pivot = rand() % (last-first);
        pivot += first;
        pivot = partition_idx_val(tab, Tval, first, last, pivot);
        quickSort_idx_val(tab, Tval, first, pivot-1);
        quickSort_idx_val(tab, Tval, pivot+1, last);
    }
    return tab;
}

