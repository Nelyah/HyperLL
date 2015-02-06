#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int necessary_size (uint32_t value){
  if (value >= (1 << 7)) {
    if (value >= (1 << 14)) {
      if (value >= (1 << 21)) {
        if (value >= (1 << 28)) return 5; else return 4;
      } else { return 3; }
    } else {
      return 2;
    }
  } else {
    return 1;
  }
} 

uint8_t* encode (uint32_t value){
  int s;
  s = necessary_size(value);
  uint8_t* target;
  target = malloc(s*sizeof(uint8_t));
  target[0] = (uint8_t)(value | 0x80);
  if (value >= (1 << 7)) {
    target[1] =(uint8_t)((value >>  7) | 0x80);
    if (value >= (1 << 14)) {
      target[2] = (uint8_t)((value >> 14) | 0x80);
      if (value >= (1 << 21)) {
        target[3] =(uint8_t)((value >> 21) | 0x80);
        if (value >= (1 << 28)) {
          target[4] = (uint8_t)(value >> 28);
        } else {
          target[3] &= 0x7F;
        }
      } else {
        target[2] &= 0x7F;
      }
    } else {
      target[1] &= 0x7F;
    }
  } else {
    target[0] &= 0x7F;
  }
  return target;
}

void print (uint8_t* target){
  int i;
  for (i = 0; i < 5; i++)
    printf("valeur encodée:%" SCNu8 "\n",target[i]);
}

uint32_t decode(uint8_t* encoded){
  uint8_t tmp, mask;
  int cpt = 0;
  uint32_t aux, res = 0;
  tmp = encoded[cpt];
  mask = (uint8_t)((1<<7) -1);
  while (tmp >= (1 << 7)){
    aux = (uint32_t) ((tmp & mask)<<(7*cpt));
    res += aux;
    cpt++;
    tmp = encoded[cpt];
  }
  aux = (uint32_t) ((tmp & mask)<<(7*cpt));
  res += aux;
  return res;
}

uint32_t decode2(uint8_t* encoded, int *len){
  uint8_t tmp, mask;
  int cpt = 0;
  uint32_t aux, res = 0;
  tmp = encoded[cpt];
  mask = (uint8_t)((1<<7) -1);
  while (tmp >= (1 << 7)){
    aux = (uint32_t) ((tmp & mask)<<(7*cpt));
    res += aux;
    cpt++;
    tmp = encoded[cpt];
  }
  aux = (uint32_t) ((tmp & mask)<<(7*cpt));
  res += aux;
  *len=cpt+1;
  return res;
}

/*
int main(int argc, char *argv[]) {
  uint32_t a = 981245893;
  uint8_t* varint;
  varint = encode(a);
  print(varint);
  uint32_t normal;
  normal = decode (varint);
  printf("valeur décodée: %"SCNu32"\n", normal);
  return 0;
}
*/
