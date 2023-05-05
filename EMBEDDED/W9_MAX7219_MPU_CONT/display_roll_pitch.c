/*
	MPU6050 Interfacing with Raspberry Pi
	http://www.electronicwings.com
*/
// gcc -o b4 b4.c -lwiringPi -lm
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <stdint.h>
#include <time.h>
#include <wiringPiSPI.h>
#include <math.h>

#define Device_Address 0x68	/*Device Address/Identifier for MPU6050*/

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47
#define pi           3.14
#define Acc_X       59
#define Acc_Y       61
#define Acc_Z       63
int fd;

#define spi0   0
uint8_t buf[2];

void sendData(uint8_t address, uint8_t data){
    buf[0] = address;
    buf[1] = data;
    wiringPiSPIDataRW(spi0, buf, 2);
}

void Init(void){
    // set decode mode: 0x09FF
    sendData(0x09,0x00);
    // set intensity: 0x0A09
    sendData(0x0A, 9);
    // scan limit: 0x0B07
    sendData(0x0B, 7);
    // no shutdown, turn off display test
    sendData(0x0C, 1);
    sendData(0x0F, 0);
}

void MPU6050_Init(){
	
	wiringPiI2CWriteReg8 (fd, SMPLRT_DIV, 0x07);	/* Write to sample rate register */
	wiringPiI2CWriteReg8 (fd, PWR_MGMT_1, 0x01);	/* Write to power management register */
	wiringPiI2CWriteReg8 (fd, CONFIG, 0);		/* Write to Configuration register */
	wiringPiI2CWriteReg8 (fd, GYRO_CONFIG, 24);	/* Write to Gyro Configuration register */
	wiringPiI2CWriteReg8 (fd, INT_ENABLE, 0x01);	/*Write to interrupt enable register */
} 

short read_raw_data(int addr){
	short high_byte,low_byte,value;
	high_byte = wiringPiI2CReadReg8(fd, addr);
	low_byte = wiringPiI2CReadReg8(fd, addr+1);
	value = (high_byte << 8) | low_byte;
	return value;
}

float absV(float number){
    if(number<0){
        number= number*(-1);
    }
    return number;
}

void display(float alphaX , float alphaY){
    uint8_t LEDCODE[] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0X7B};
    
    int32_t integerPartX = alphaX;
    int32_t integerPartY = alphaY;

    uint8_t dec = 1;   // 
    
    int32_t fractionalPartX = (alphaX - integerPartX) * pow(10,dec);
    int32_t numberX = integerPartX*pow(10,dec) + fractionalPartX;
    int32_t fractionalPartY = (alphaY - integerPartY) * pow(10,dec);
    int32_t numberY = integerPartY*pow(10,dec) + fractionalPartY;

    // set scanlimit
    sendData(0x0B,7);
    // dislay char degree
    sendData(1,0x63);
    sendData(5,0x63);
    //display number
    
    if(integerPartX <10){
        sendData(4,0x00);
        for(int i=1; i<3;i++){          
            if(i==(dec+1))
                sendData(i+1,(LEDCODE[(numberX%10)]|0x80)); // turn on dot segment 
            else
                sendData(i+1,LEDCODE[numberX%10]);
        numberX = numberX/10;
        } 
    }
    else{
        for(int i=1; i<4;i++){
            
            if(i==(dec+1))
                sendData(i+1,(LEDCODE[(numberX%10)]|0x80)); // turn on dot segment 
            else
                sendData(i+1,LEDCODE[numberX%10]);
        numberX = numberX/10;
        }
    }

//     if(integerPartY<10){
//         sendData(8,0x00);
//         for(int i=5; i<7;i++){
//             if(i==(dec+5))
//                 sendData(i+1,(LEDCODE[(numberY%10)]|0x80)); // turn on dot segment 
//             else
//                 sendData(i+1,LEDCODE[numberY%10]);
//             numberY = numberY/10;
//         }  
//     }
//     else{
//         for(int i=5; i<8;i++){
//             if(i==(dec+5))
//                 sendData(i+1,(LEDCODE[(numberY%10)]|0x80)); // turn on dot segment 
//             else
//                 sendData(i+1,LEDCODE[numberY%10]);
//             numberY = numberY/10;
//     }
// }
}
int main(){
	
	uint8_t Acc_x,Acc_y,Acc_z;
	//float Gyro_x,Gyro_y,Gyro_z;
	float Ax=0, Ay=0, Az=0;
	float Gx=0, Gy=0, Gz=0;
	fd = wiringPiI2CSetup(Device_Address);   /*Initializes I2C with device Address*/
	MPU6050_Init();		                 /* Initializes MPU6050 */
    wiringPiSPISetup(0,1000000);
    // set operational mode for max7219
    Init();
    int count = 0;
    int16_t LSB = 4096;  // low pass filter
	while(1)
	{
        
        float Ax = (float)read_raw_data(Acc_X)/LSB;
        float Ay = (float)read_raw_data(Acc_Y)/LSB;
        float Az = (float)read_raw_data(Acc_Z)/LSB;
        
        float pitch = absV(atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/3.14);
		float roll = absV(atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/3.14);

        printf("\nGia tri roll :%f\n",roll);
        printf("Gia tri pitch :%f\n",pitch);
		display(pitch,roll);

		delay(500);
	}
	return 0;
}