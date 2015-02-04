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
  char* tmp = malloc(strlen(val)+strlen(new)); 
  strcpy(tmp, val);
  strcat(tmp, new);
  val=tmp;
}

void appendInt32_nonDelta(int value){
  assert( value >= 0 );
  char* encoded;
  uint32_t v = (uint32_t) value;
  encoded =(char*) encode(v);
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
  val="";
}

/*int main(int argc, char *argv[]) {
  int s;
  uint32_t a = 15;
  uint32_t b = 16;
  uint32_t c = 17;
  uint32_t d = 18;
  uint32_t e = 19;
  uint32_t f = 20;

  appendInt32(a);
  appendInt32(b);
  appendInt32(c);
  appendInt32(d);
  appendInt32(e);
  appendInt32(f);

  appendInt32_nonDelta(a);
  appendInt32_nonDelta(b);
  appendInt32_nonDelta(c);
  appendInt32_nonDelta(d);
  appendInt32_nonDelta(e);
  appendInt32_nonDelta(f);

  s = string_size();
  printf("size: %d\n",s);

  return 0;
}*/
