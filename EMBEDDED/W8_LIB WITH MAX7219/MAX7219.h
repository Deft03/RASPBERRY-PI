#ifndef __MAX7219_LIBRARY__
#define __MAX7219_LIBRARY__

#include <stdint.h>
//unsigned char data[2]; //16bit

//uint8_t right[8] = {0x01};      //LED7= 1

//unsigned char data1[2]; //16bit

void Init(void);

void sent_data(unsigned char address, unsigned char value);

void display(uint8_t *arr);
void trai(void);

void phai(void);
#endif // __MAX7219_LIBRARY__





