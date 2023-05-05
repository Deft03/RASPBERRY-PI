//gcc -o spiT spi_tri.c -lwiringPi
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdint.h>

unsigned char data[2] ;//16bit
unsigned char mssv[8]={2,0,1,4,6,4,1,2};
unsigned char data2[8] ={0x0D, 0x1D, 0x1C};

void send_data(unsigned char address, unsigned char value){
	data[0] = address;
	data[1] = value;
	wiringPiSPIDataRW(0,data,2);
}

void Init(void){
	//set decode mode
	send_data(0x09, 0xff); // 0X00
	//set intensitive
	send_data(0x0A,0x09);
	//set scan limit
	send_data(0x0B,0x07);
	//no shutdown, turn off display test
	send_data(0x0C,1);
	send_data(0x0F,0);
}
int main(void){
	wiringPiSPISetup(0,1000000);//spi0,speed = 10Mhz
	//setup operational mode for max7219
	Init();
	for(int i =0; i<8;i++){
		send_data(i+1,mssv[7-i]);
	}
	

	return 0;
}
