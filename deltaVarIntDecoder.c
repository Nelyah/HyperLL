#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "varint.h"
#include "deltaVarIntEncoder.h"

uint8_t* encoded = NULL;
int idx=0;
int len=0;
int prevdecoded=0;

void deltaVarIntDecoder(uint8_t* b, int size){
/* init the general variable
 */
  encoded = NULL;
  encoded = b;
  idx=0;
  len = size;
  prevdecoded = 0;
}

void reset_varintDecoder(){
    encoded = NULL;
    idx=0;
    len=0;
    prevdecoded=0;
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
/* Returns the next value 
 */
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
