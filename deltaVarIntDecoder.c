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
  encoded = value;
  len=strlen(encoded);
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

/*int main(int argc, char *argv[]) {
  int s;
  uint32_t a = 659;
  uint32_t b = 722;
  uint32_t c = 881;
  uint32_t d = 933;
  uint32_t e = 1009;
  uint32_t f = 1013;

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

  char* chaine;

  chaine = data();

  deltaVarIntDecoder(chaine);

  int tmp;
  tmp = getNext();
  while (tmp != -1){
    printf("%d\n",tmp);
    tmp=getNext();
  }

  return 0;
}*/
