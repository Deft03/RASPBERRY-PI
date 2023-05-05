//gcc -o BT5 BT5.c -lwiringPi

#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define led 17
#define button 23 

volatile uint8_t state = 0;
uint8_t randm ;
int count = 0;
int n =0, count1, cancle = 0 ;
float avg, times ;
int heSo =0;
int GetRandom(int min,int max){
    return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

void ngat_bt1(void){
	if(digitalRead(led) == 1)
	{
        if(count > count1 && count < (1000 + count1)){
            state = 1 ;
            delay(10);
        }
    }
}
int main(){
    printf("Wellcome to mah game: \n");
	wiringPiSetupGpio();
	//Khai bao port
    pinMode(led, OUTPUT);
	pinMode(button ,  INPUT);
	wiringPiISR(23, INT_EDGE_BOTH, &ngat_bt1);


    printf("%d \n", count1);

        while( cancle < 10 )
        {   
            count1= GetRandom(1000,3000);
            count++;
            delay(1);

            if(count > count1 ){
                digitalWrite(led, 1);
            }
            if(state == 1){
                digitalWrite(led, 0);
                times = (count - count1 );
                avg += times;
                printf("!!!Ban da thang!!!\n") ;
                printf("Thoi gian nhan nut la: %0.f ms \n", times);
                cancle++;
                count = 0;
                heSo++;
                delay(1000); 
                state = 0; 
    
            }
            if(count > count1 +1000 ){
                digitalWrite(led, 0);
                printf("...Ban da thua... \n");
                cancle++;
                count = 0;
                delay(1000);
                state = 0; 
            }
        }
        avg = avg / heSo;
        printf("Trung binh thoi gian: %0.f ms \n", avg);
	return 0;
	}
