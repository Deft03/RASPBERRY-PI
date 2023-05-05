#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define spi0   0
uint8_t buf[2];

void sendData(uint8_t address, uint8_t data){
    buf[0] = address;
    buf[1] = data;
    wiringPiSPIDataRW(spi0, buf, 2);
}

void Init(void){
    // set decode mode: 0x09FF
    sendData(0x09,0xFF);
    // set intensity: 0x0A09
    sendData(0x0A, 9);
    // scan limit: 0x0B07
    sendData(0x0B, 7);
    // no shutdown, turn off display test
    sendData(0x0C, 1);
    sendData(0x0F, 0);
}

void display_float(float num, uint8_t dec){
    int32_t integerPart = num;
    int32_t fractionalPart = (num - integerPart) * pow(10,dec);
    int32_t number = integerPart*pow(10,dec) + fractionalPart;
    // count the number of digits
    uint8_t count=1;
    int32_t n = number;

    printf("\t%d",integerPart);
    printf("\t%d",fractionalPart );
    printf("\t%f",num );

    while(n/10){
        count++;
        n = n/10;
    }
    // set scanlimit
    sendData(0x0B, count-1);
    // dislay number
    for(int i=0; i<count;i++){
        if(i==dec)
            sendData(i+1,(number%10)|0x80); // turn on dot segment 
        else
            sendData(i+1,number%10);
        number = number/10;
    }
}


int main(void){
    // setup SPI interface
    wiringPiSPISetup(spi0, 10000000);
    // set operational mode for max7219
    Init();
    display_float(15653.2323,2);
    
    return 0;
}