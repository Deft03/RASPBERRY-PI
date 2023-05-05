#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "max7219.h"
unsigned char data1[2];
uint8_t up7[8] = {0x03};
uint8_t left7[8] = {0x03};
uint8_t right7[8] = {0x01};

void sent_data(unsigned char address, unsigned char value){
	data1[0] = address;
	data1[1] = value;
	wiringPiSPIDataRW(0, data1, 2);
}

void Init(void){
	//set decode mode
	sent_data(0x09, 0xff);
	//set intensity
	sent_data(0x0A, 0x09);
	//set scan limit
	sent_data(0x0B, 0x07);
	//no shutdown,turn off display test
	sent_data(0x0C, 1);
	sent_data(0x0F, 0);
	
}
void display(uint8_t *arr){
	wiringPiSPISetup(0, 10000000); 
	Init();
	//display student number
	for(int i = 0; i < 8; i++){
		sent_data(i + 1, arr[7 - i]);
	}

}
void trai(void){
	uint8_t left7[8] = {0x03};
	display(left7);
}
void phai(void){
	//uint8_t left7[8] = {0x03};
	display(right7);
}



//wiringPiSPISetup(0, 10000000); //spi0, speed = 10MHz  PHAI GOI HAM NAY TRONG FILE CODE.C
