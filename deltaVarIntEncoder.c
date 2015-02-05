#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "varint.h"
#include <assert.h>

char* val = "";
uint32_t prev = 0;

void concat(char* new){
  char* tmp = malloc(strlen(val)+strlen(new)+1); 
  strcpy(tmp, val);
  strcat(tmp, new);
  val=tmp;
}

void appendInt32_nonDelta(int value){
  assert( value >= 0 );
  char* encoded;
  uint32_t v = (uint32_t) value;
  uint8_t* tab8 = encode(v);
  encoded =(char*) tab8;
  concat(encoded);
}

void appendInt32(int value){
  appendInt32_nonDelta(((uint32_t) value)-prev);
  prev = ((uint32_t) value);
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
