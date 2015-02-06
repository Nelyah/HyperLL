#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "varint.h"
#include <assert.h>

char* val = "";
uint32_t prev = 0;
long len_map = 64;

void reset_delta(){
    prev = 0;
}

char* concat(char* old, char*new){
    while ((strlen(old)+strlen(new)+1) > len_map) {
        realloc(old, len_map*2*sizeof(char));
        len_map *= 2;
    }
    strcat(old, new);
    return old;

}
/*
void appendInt32_nonDelta(int value){
  assert( value >= 0 );
  char* encoded;
  uint32_t v = (uint32_t) value;
  uint8_t* tab8 = encode(v);
  encoded =(char*) tab8;
  concat(encoded);
}*/

void appendInt32_char(int value, char* map){
  assert( value >= 0 );
  char* encoded;
  uint32_t v = (uint32_t) value;
  uint8_t* tab8 = encode(v);
  encoded =(char*) tab8;
  concat(map, encoded);
}

void appendInt32(int value, char* map){
  appendInt32_char(((uint32_t) value)-prev, map);
  prev = value;
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
