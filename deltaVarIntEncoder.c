#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "varint.h"
#include "types.h"
#include "bitStruct.h"
#include <assert.h>

char* val = "";
uint32_t prev = 0;
bit_st* bs_tmp = NULL;

void reset_delta(){
    prev = 0;
}

char* concat(char* old, char*new){
    strcat(old, new);
    return old;
}


void appendInt32_char(int value, char* map){
  assert( (uint32_t)value >= 0 );
  char* encoded;
  uint32_t v = (uint32_t) value;
  encoded= (char*)encode(v);
  int s = necessary_size(v);
  memcpy(&map[bs_tmp->cptW], encoded, s);
  bs_tmp->cptW += s;
  bs_tmp->nbits += s*8;
  free(encoded);

}

void appendInt32_prev(int value, char* map){
  appendInt32_char(((uint32_t) value)-prev, map);
  prev = value;
}

void appendInt32(int value, bit_st* b){
    bs_tmp = b;
    if (b->nbAlloc < b->nbits+5*SIZE_OF_VALUE) {
        bitv_realloc(b,b->nbAlloc*2);
    }
    appendInt32_prev(value,b->words);
    bs_tmp = NULL;
}

char* data(){
  return val;
}

int string_size(){
  return strlen(val);
}

void reset_string(){
  val = NULL;
  val="";
}
