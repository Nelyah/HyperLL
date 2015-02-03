

typedef uint32_t word_t;
enum { BITS_PER_WORD = 32 };

typedef struct bitv {
    word_t *words; 
    int nwords; 
    int nbits;  // number of used bits
    int nbAlloc; // number of allocated bits
    int mode;
} bit_st;

bit_st* bitv_realloc(bit_st* b, int bits);
bit_st* bitv_alloc(int bits);
void bitv_append(bit_st *b, int index, int value);
void bitv_free(struct bitv *b);
int bitv_get(struct bitv *b, int bit);
int bitv_read(bit_st *b, int index);
void bitv_set(struct bitv *b, int bit, int val);
void bitv_write(bit_st *b, int index, int value);
int bytesUsed(bit_st *b);
void updateMax(bit_st* b, int index, int value);
void bitv_readBits(bit_st* b, int *index, int *value, int bit);
bit_st* getDense(bit_st* b);
bit_st* merge(bit_st* b, uint64_t* Mval, uint64_t* Midx);


int main(int argc, char *argv[]);
