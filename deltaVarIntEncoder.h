#ifndef DEF_DELTAVARINT_ENCODER
#define DEF_DELTAVARINT_ENCODER

void appendInt32(int value, char* map);
int string_size();
void reset_string();
uint8_t* concat(uint8_t* old, uint8_t* new);
void reset_delta();
void appendInt32_char(int value, uint8_t*map);

#endif
