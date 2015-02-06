#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "varint.h"
#include <assert.h>

char* val = "";
uint32_t prev = 0;

void reset_delta(){
    prev = 0;
}

char* concat(char* old, char*new){
    char* tmp = malloc(strlen(old)+strlen(new)+1); 
    strcpy(tmp, old);
    strcat(tmp, new);
    old=tmp;
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
