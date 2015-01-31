
#ifndef HYP_LOG_LOG
#define HYP_LOG_LOG


#define a_16 0.673
#define a_32 0.697
#define a_64 0.709
#define a_128(m) 0.7213/(1+0.079/m) 

uint64_t lastBits(int n, uint64_t val);
float hyperLL_64bits(void);
void addItem(uint64_t hashVal);
void init(void);
void reset(void);
void freeAll(void);
float count(void);
float count_linearCounting(void);
float count_raw(void);
float count_file(char* filename);
void loadFile(char* filename);



#endif
