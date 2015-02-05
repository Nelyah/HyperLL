#ifndef DEF_DELTAVARINT_ENCODER
#define DEF_DELTAVARINT_ENCODER

void appendInt32(int value);
char* data();
int string_size();
void reset_string();

#endif
