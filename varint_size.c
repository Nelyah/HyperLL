#include "../varint.h"
#include "../deltaVarIntEncoder.h"
#include "../deltaVarIntDecoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define VALTAB_SIZE 10000
#define STEP 10

void compute_varint_size(){
  int* values;
  values = malloc(VALTAB_SIZE*sizeof(int));

  int* size_nodelta;
  size_nodelta = malloc(VALTAB_SIZE*sizeof(int));
  int* size_delta;
  size_delta = malloc(VALTAB_SIZE*sizeof(int));

  int i;
  int val;
  val = 0;
  for (i=0; i<VALTAB_SIZE; i++){
    values[i] = val;
    val+=STEP;
  }

  int j;
  for (j=0; j<VALTAB_SIZE; j++){
    appendInt32_nonDelta(values[j]);
    size_nodelta[j] = string_size();
  }

  char* res = data();

  reset_string();

  for (i=0; i<VALTAB_SIZE; i++){
    appendInt32(values[i]);
    size_delta[i] = string_size();
  }

  char* res2=data();

  FILE* f = fopen("varint_size.txt","w");
  for (i = 0; i<VALTAB_SIZE; i++) {
    int a, b, c;
    a= values[i];
    b= size_nodelta[i];
    c= size_delta[i];
    fprintf(f,"%d %d %d \n", a, b, c);
    if (i%100==0) printf("%d/10000\n",i);
  }
}

int main(int argc, const char **argv) {
  compute_varint_size();
  return 0;
}
