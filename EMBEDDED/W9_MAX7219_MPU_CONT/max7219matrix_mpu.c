#include<wiringPi.h>
#include<wiringPiI2C.h>
#include<stdio.h>
#include<stdint.h>
#include<math.h>
#include <wiringPiSPI.h>
#define Sample_rate   25 // 0x19
#define Config   	  26 //0x1A
#define Gyro_config   27
#define Acc_config    28
#define Interrupt     56
#define PWR_Managment 107
#define ACC_X 		  59
#define ACC_Y 		  61
#define ACC_Z		  63
#define pi 3.14159	
int mpu;

int16_t read_sensor(unsigned char sensor){
	int16_t high, low, data;
	high = wiringPiI2CReadReg8(mpu, sensor); 	 
	low = wiringPiI2CReadReg8(mpu, sensor + 1);			
	data = (high<<8) | low; 																							
	return data;
}
void Init_6050(void){
	//register 25-28, 56, 107	
	//Sample_rate 500Hz
	wiringPiI2CWriteReg8(mpu,Sample_rate,15);
	//Không sử dụng nguồn xung ngoài, tắt DLFP
	wiringPiI2CWriteReg8(mpu,Config,0);
	//gyro FS: +- 500 o/s
	wiringPiI2CWriteReg8(mpu,Gyro_config,0x08);
	//acc FS: +- 8g
	wiringPiI2CWriteReg8(mpu,Acc_config,0x10);
	//mở interrupt của data ready 
	wiringPiI2CWriteReg8(mpu,Interrupt,0x01);
	// chọn nguồn xung Gyro X
	wiringPiI2CWriteReg8(mpu,PWR_Managment,0x01);
}

unsigned char data [2];
uint8_t right[8] = {0x00,0x08,0x04,0x7E,0x04,0x08,0x00,0x00};
uint8_t up[8] = {0x00,0x08,0x1C,0x2A,0x08,0x08,0x08,0x00};
uint8_t left[8] = {0x00,0x10,0x20,0x7E,0x20,0x10,0x00,0x00};
uint8_t down[8] = {0x00,0x08,0x08,0x8,0x2A,0x1C,0x08,0x00};
void send_data(unsigned char address, unsigned char value)
{
    data[0] = address;
    data[1] = value;
    wiringPiSPIDataRW(0,data,2);
}
void Init(void)
{
    send_data(0x09,0x00);
    send_data(0x0A,0x09);
    send_data(0x0B,0x07);
    send_data(0x0C,1);
    send_data(0x0F,0);
}

int main(void){
	//setup giao thuc I2C	
	mpu = wiringPiI2CSetup(0x68); // slide15, ban đầu ở chế độ AD0 = 0, dia chi 0x68
	wiringPiSPISetup(0,8000000);
    Init();
	//Thiet lap che do do cho MPU6050, khoi tao ham Init 
	Init_6050();
	while(1) // cứ 1s đo góc nghiên 1 lần
	{
		// đọc giá trị đo
		float Ax = (float)read_sensor(ACC_X)/4096.0;
		float Ay = (float)read_sensor(ACC_Y)/4096.0;
		float Az = (float)read_sensor(ACC_Z)/4096.0;
		
		float pitch = atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/pi;
		float roll = atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/pi;
        if(pitch>0 & roll >0)
        {
            delay(300);
            for (int i=0;i<8;i++)
                send_data(i+1,up[7-i]);
        }


        else if(pitch>0 & roll <0)
        {
            delay(300);
            for (int i=0;i<8;i++)
                send_data(i+1,left[7-i]);
        } 

        else if(pitch<0 & roll >0)
        {
            delay(300);
            for (int i=0;i<8;i++)
                send_data(i+1,right[7-i]);
        }

        else if(pitch<0 & roll <0)
        {
            delay(300);
            for (int i=0;i<8;i++)
                send_data(i+1,down[7-i]);
        } 		
	//	printf("Gia tri do la: Pitch %f va Roll %f\n",pitch, roll);
		
	//	delay(3000);  //1s 1 lần thấy được giá trị đo				
	}			
	return 0;	
}
