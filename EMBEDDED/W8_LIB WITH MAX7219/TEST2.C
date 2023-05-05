
// gcc -o bktra2 TEST2.c -lwiringPi -lm

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
unsigned int led_7seg[10] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};

// Mảng lưu trữ các số và ký tự hiển thị
unsigned char font[][2] = {
  {'A',0b1110111},{'B',0b1111111},{'C',0b1001110},{'D',0b1111110},{'E',0b1001111},{'F',0b1000111},       
  {'G',0b1011110},{'H',0b0110111},{'I',0b0110000},{'J',0b0111100},{'L',0b0001110},{'N',0b1110110},       
  {'O',0b1111110},{'P',0b1100111},{'R',0b0000101},{'S',0b1011011},{'T',0b0001111},{'U',0b0111110},       
  {'Y',0b0100111},{'[',0b1001110},{']',0b1111000},{'_',0b0001000},{'a',0b1110111},{'b',0b0011111},       
  {'c',0b0001101},{'d',0b0111101},{'e',0b1001111},{'f',0b1000111},{'g',0b1011110},{'h',0b0010111},       
  {'i',0b0010000},{'j',0b0111100},{'l',0b0001110},{'n',0b0010101},{'o',0b0011101},{'p',0b1100111},       
  {'r',0b0000101},{'s',0b1011011},{'t',0b0001111},{'u',0b0011100},{'y',0b0100111},{'-',0b0000001},
  {' ',0b0000000},{'0',0b1111110},{'1',0b0110000},{'2',0b1101101},{'3',0b1111001},{'4',0b0110011},
  {'5',0b1011011},{'6',0b1011111},{'7',0b1110000},{'8',0b1111111},{'9',0b1111011},{'V',0b0111110},
  {'k',0b0001111},{'m',0b0010101},{'q',0b1110011},{'v',0b0111110},{'.',0b10000000},{'?',0b1100101},
  {'\0',0b0000000},};


void sendData(uint8_t address, uint8_t data){
    buf[0] = address;
    buf[1] = data;
    wiringPiSPIDataRW(spi0, buf, 2);
}
void clear_data(){
	for (int i = 1; i < 9; i ++){
		sendData(i, 0);	
	}
}
void Init(void){
    // set decode mode: 0x09FF
    sendData(0x09,0x00);
    //sendData(0x09,0xff);
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

void display(float alphaY){
    uint8_t LEDCODE[] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0X7B};
    
    int32_t integerPartY = alphaY;

    uint8_t dec = 1;   // 

    int32_t fractionalPartY = (alphaY - integerPartY) * pow(10,dec);
    int32_t numberY = integerPartY*pow(10,dec) + fractionalPartY;

    // set scanlimit
    sendData(0x0B,7);
    // dislay char degree
    //sendData(1,0x63);
    //sendData(5,0x63);
    //display number
    
    // if(integerPartX <10){
    //     sendData(4,0x00);
    //     for(int i=1; i<3;i++){          
    //         if(i==(dec+1))
    //             sendData(i+1,(LEDCODE[(numberX%10)]|0x80)); // turn on dot segment 
    //         else
    //             sendData(i+1,LEDCODE[numberX%10]);
    //     numberX = numberX/10;
    //     } 
    // }
    // else{
    //     for(int i=1; i<4;i++){
            
    //         if(i==(dec+1))
    //             sendData(i+1,(LEDCODE[(numberX%10)]|0x80)); // turn on dot segment 
    //         else
    //             sendData(i+1,LEDCODE[numberX%10]);
    //     numberX = numberX/10;
    //     }
    // }

    if(integerPartY<10){
        sendData(8,0x00);
        for(int i=5; i<7;i++){
            if(i==(dec+5))
                sendData(i+1,(LEDCODE[(numberY%10)]|0x80)); // turn on dot segment 
            else
                sendData(i+1,LEDCODE[numberY%10]);
            numberY = numberY/10;
        }  
    }
    else{
        for(int i=5; i<8;i++){
            if(i==(dec+5))
                sendData(i+1,(LEDCODE[(numberY%10)]|0x80)); // turn on dot segment 
            else
                sendData(i+1,LEDCODE[numberY%10]);
            numberY = numberY/10;
    }
}
}

///////////////////////////////////////////
void display_char(char c, int pos){  //0x00
    // Nếu vị trí không hợp lệ, thoát hàm
    if (pos < 1 || pos > 8) return;
    
    // Tìm kiếm trong bảng font để lấy đúng giá trị của ký tự
    unsigned char value = 0;
    for (int i = 0; i < sizeof(font)/sizeof(font[0]); i++){
        if (font[i][0] == c){
            value = font[i][1];
            break;
        }
    }
    sendData(0x09,  0x00);
    // Hiển thị giá trị ký tự tại vị trí trên module
    sendData(pos, value);
}

// Hàm hiển thị số nguyên ở vị trí bất kỳ 0x00
void display_number(unsigned int pos, unsigned int songuyen){
	unsigned int dvi = 0, chuc = 0, tram = 0, ngan = 0;
			
	ngan = songuyen / 1000;
	tram = (songuyen - ngan*1000) / 100;
	chuc = (songuyen - ngan*1000 - tram*100) / 10;
	dvi =  songuyen%10;
			
	sendData(0x09, 0x00);
	sendData(pos, led_7seg[dvi]);
	sendData(pos + 1, led_7seg[chuc]);
}
void ms_delay(int val){
	int i,j;
	for(i=0;i<=val;i++)
		for(j=0;j<1200;j++);
}

int check(float cur, float past){
    past = cur;
    ms_delay(100);
    if(cur-1<past<cur+1){
        return 1;
    }
    else
    return 0;

}
int main(){
	
	uint8_t Acc_x,Acc_y,Acc_z;
	float Gyro_x,Gyro_y,Gyro_z;
	float Ax=0, Ay=0, Az=0;
	float Gx=0, Gy=0, Gz=0;
	fd = wiringPiI2CSetup(Device_Address);   /*Initializes I2C with device Address*/
	MPU6050_Init();		                 /* Initializes MPU6050 */
    wiringPiSPISetup(0,1000000);
    // set operational mode for max7219
    Init();
    int count = 0;
    int16_t LSB = 4096;  // low pass filter
    clear_data();
    time_t rawtime;
    struct tm *ct;
    time (&rawtime);
    ct = localtime (&rawtime);
    

    // time1=millis();
    // time2=millis();
    int8_t v0 = 0, a0=0 ;
    
	while(1)
	{

        float Ax = (float)read_raw_data(Acc_X)/LSB;
        float Ay = (float)read_raw_data(Acc_Y)/LSB;
        float Az = (float)read_raw_data(Acc_Z)/LSB;
        
        float Gyro_x = (float)read_raw_data(GYRO_XOUT_H)/LSB;
		float Gyro_y = (float)read_raw_data(GYRO_YOUT_H)/LSB;
		float Gyro_z = (float)read_raw_data(GYRO_ZOUT_H)/LSB;

        float pitch = absV(atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/3.14);
		float roll = absV(atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/3.14);
        printf(" goc: %f  ", Gyro_x );

    // time1 = ct -> tm_min;

    //  v=v0+(aym+a0)/2*(time2-time1); 
    //  v0=v;
    //  a0=Ay;
    // ms_delay(1000);
    // time2 = time1
        float  G_pX, G_pY;
        G_pX = Gyro_x;
        G_pY = Gyro_y;
        //printf(" cur : %f %f ", cAX, cAY);
        //printf("Ax: %f", pitch);
        if( roll<10 ){
            clear_data();
            display_char('C',5);
            display_char('L',4);
            display_char('O',3);
            display_char('S',2);
            display_char('E',1);

        }
        else{
            clear_data();
            display_char('O',4);
            display_char('P',3);
            display_char('E',2);
            display_char('N',1);

        }

        printf("\nGia tri roll :%f\n",roll);
        printf("Gia tri pitch :%f\n",pitch);


		display(roll);

		delay(500);
	}
	return 0;
}