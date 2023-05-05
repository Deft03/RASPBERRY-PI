//gcc -o b1 b1.c -lwiringPi -lm
#include <wiringPiSPI.h>

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

int mpu;
#define INT_pin      4
#define spi0         0
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47
#define TEMPERATURE  0X41



void send_data(uint8_t address, uint8_t value)
{
    uint8_t data[2];
    data[0]=address;
    data[1]=value;
    wiringPiSPIDataRW(spi0, data, 2);
    //return 0; //gui tra du lieu tu ic ve (neu ic co chuc nang gui tra du lieu)
}

void Initmax7219()
{
    //decode mode: 0x09FF
    send_data(0x09, 0xFF);  //chon mode dung so thap phan
    //intensity: 0x0A08
    send_data(0x0A,0x08); // chon cuong do sang cua led
    //scanlimit:
    send_data(0x0B,7); // chon so luong digit (so led)
    //no shutdown, display test off
    send_data(0x0C,1);
    send_data(0x0F,0);
}

void initMpu(void){
	wiringPiI2CWriteReg8(mpu, 0x19, 9);		//sample rate
	wiringPiI2CWriteReg8(mpu, 0x1A, 0x02);	//DLPF
	wiringPiI2CWriteReg8(mpu, 0x1B, 0x08);	//Gyro
	wiringPiI2CWriteReg8(mpu, 0x1C, 0x10);	//ACC
	wiringPiI2CWriteReg8(mpu, 0x38, 1);		//Interrupt
	wiringPiI2CWriteReg8(mpu, 0x6B, 1);		//CLK source
    wiringPiI2CWriteReg8(mpu, 0x41, 1);

}

void display_float(float num, uint8_t dec){
    int32_t integerPart = num;
    int32_t fractionalPart = (num - integerPart) * pow(10,dec);
    int32_t number = integerPart*pow(10,dec) + fractionalPart;
    // count the number of digits
    uint8_t count=1;
    int32_t n = number;
    while(n/10){
        count++;
        n = n/10;
    }
    // set scanlimit
    send_data(0x0B, count-1);
    // dislay number
    for(int i=0; i<count;i++){
        if(i==dec)
            send_data(i+1,(number%10)|0x80); // turn on dot segment 
        else
            send_data(i+1,number%10);
        number = number/10;
    }
}
int16_t readMPU(uint8_t address){
    int16_t value, high, low;
    high = wiringPiI2CReadReg8(mpu, address);
    low = wiringPiI2CReadReg8(mpu, address+1);

    value = (high <<8) | low;
    return value;
}
void dataReady(void){
	// clear interrupt flag
	wiringPiI2CReadReg8(mpu, 0x3A);
	// read sensor data
    readMPU(0x41);
    
}
void ms_delay(int val){
	int i,j;
	for(i=0;i<=val;i++)
		for(j=0;j<1200;j++);
}

     
int main(void){
	// setup i2c interface
    wiringPiSetupGpio();
    wiringPiSPISetup(spi0, 10000000);

	mpu = wiringPiI2CSetup(0x68);
    pinMode(4,  INPUT);

    Initmax7219;
    
	// check connection
	if(wiringPiI2CReadReg8(mpu, 0x75)!= 0x68){
		printf("Connection fail. \n");
		exit(1);
	}
    
	// setup operational mode for mpu6050
	initMpu();
	// setup interrupt for INT pin
	pinMode(INT_pin, INPUT);
	wiringPiISR(INT_pin, INT_EDGE_RISING, &dataReady);
    int temp;
    temp = readMPU(0x41);
    temp = (temp/340.0) + 36.53;
    //display_float(temp, 2);
	return 0;
}