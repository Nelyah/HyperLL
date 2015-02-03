#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef uint32_t word_t;
enum { BITS_PER_WORD = 32 };
typedef struct bitv {
    word_t *words; 
    int nwords; 
    int nbits; 
} bit_st;

static inline void check_bounds(struct bitv *b, int bit) {
    if (b->nbits < bit) {
        fprintf(stderr, "Attempted to access a bit out of range\n");
        exit(1);
    }
}

void bitv_set(struct bitv *b, int bit, int val) {
    check_bounds(b, bit);
    b->words[bit >> 5] |= val << (31 - (bit % BITS_PER_WORD));
}


int bitv_get(struct bitv *b, int bit) {
    check_bounds(b, bit);
    return (b->words[bit >> 5] & (1 << (31 - (bit % BITS_PER_WORD)))) >> (31 - (bit % BITS_PER_WORD));
}

int  bitv_read(bit_st *b, int index) {
    int value = 0, i, bit=index*6+5;
    
    for (i = 0; i < 6; i++) {
        value += (bitv_get(b, bit) << i);
        bit--;
    }

    return value;

}

void bitv_write(bit_st *b, int index, int value){
    // value must be < 2**6
    int bit = index*6+5, i;
    
    for (i = 0; i < 6; i++) {
        bitv_set(b, bit, (value & 1));
        bit--;
        value >>= 1;
    }

}


void bitv_free(struct bitv *b) {
    if (b != NULL) {
        if (b->words != NULL) free(b->words);
        free(b);
    }
}

void bitv_dump(struct bitv *b) {
    if (b == NULL) return;

    for(int i = 0; i < b->nwords; i++) {
        word_t w = b->words[i];
        for (int j = 0; j < BITS_PER_WORD; j++) {
            printf("%d", w & 1);
            w >>= 1;
        }
        printf(" ");
    }

    printf("\n");
}

bit_st* bitv_alloc(int bits) {
    
    bit_st *b = malloc(sizeof(bit_st));
    if (b == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    b->nwords = (bits >> 5) + 1;
    b->nbits  = bits;
    b->words  = malloc(b->nwords*sizeof(*b->words));

    if (b->words == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(b->words, 0, sizeof(*b->words) * b->nwords);

    return b;
}



int main(int argc, char *argv[]) {
    struct bitv *b = bitv_alloc(32);
    
    word_t a = 5;
    bitv_write(b,0,8);
    a = a << 29;
//    bitv_set(b, 5);
    a = bitv_read(b, 0);
    printf("a : %d\n",a);
//    bitv_set(b, 1*20+3);
//    bitv_set(b, 3);
/*    bitv_set(b, 3);
    bitv_set(b, 5);
    bitv_set(b, 7);
    bitv_set(b, 9);
    bitv_set(b, 32);*/
    bitv_dump(b);
    bitv_free(b);
                                    
    return 0;
}
