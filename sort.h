
#ifndef DEF_SORT
#define DEF_SORT
#include <inttypes.h>
int partition(float* tab, int first, int last, int pivot);
float* quickSort(float* tab, int first, int last);
int partition_idx_val(uint32_t* tab, uint32_t* Tval, int first, int last, int pivot);
uint32_t* quickSort_idx_val(uint32_t* tab, uint32_t* Tval, int first, int last);

#endif
