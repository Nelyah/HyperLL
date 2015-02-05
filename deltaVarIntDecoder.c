#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "varint.h"
#include "deltaVarIntEncoder.h"

char* encoded = NULL;
int idx=0;
int len=0;
int prevdecoded=0;

void deltaVarIntDecoder(char* value){
  encoded = NULL;
  encoded = value;
  idx=0;
  len=strlen(encoded);
}

uint32_t getNext_noDelta(){
  uint32_t res;
  int cpt=0;
  if (encoded != NULL && idx<len){
    res=decode2(((uint8_t*)encoded),&cpt);
    encoded += (cpt*sizeof(uint8_t));
    idx+=cpt;
    return res;
  }
  return -1;
}

uint32_t getNext(){
  uint32_t res;
  int cpt=0;
  if (encoded != NULL && idx<len){
    res=decode2(((uint8_t*)encoded),&cpt);
    encoded += (cpt*sizeof(uint8_t));
    idx+=cpt;
    prevdecoded+=res;
    return prevdecoded;
  }
  return -1;
}
