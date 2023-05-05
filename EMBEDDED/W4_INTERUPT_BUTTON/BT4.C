//gcc -o B44 BT4.c -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

uint8_t led[3] = {17, 27, 22};
uint8_t bt[3]  = {23, 24, 25};
volatile uint8_t state = 3;
uint8_t randm ;
int count = 0;
int thoiGian = 0 ;
int n =0 ;

int GetRandom(int min,int max){
    return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

void ngat_bt1(void){
	if(digitalRead(23) == 1)
	{
		state = 0 ;
        delay(300);
	}
}
void ngat_bt2(void){
	if(digitalRead(24) == 1)
	{
        state = 1; 
        delay(300);
	} 
}
void ngat_bt3(void){
	if(digitalRead(25) == 1)
	{
		state = 2; 
        delay(300);
	}
}


int main(){
    printf("Wellcome to mah game: \n");
	wiringPiSetupGpio();
	//enable ngat
	for(int i=0; i<3; i++){
		pinMode(led[i], OUTPUT);
		pinMode(bt[i] ,  INPUT);
		softPwmCreate(led[i], 0 , 100);
		}

	wiringPiISR(23, INT_EDGE_BOTH, &ngat_bt1);
	wiringPiISR(24, INT_EDGE_BOTH, &ngat_bt2);
	wiringPiISR(25, INT_EDGE_BOTH, &ngat_bt3);
    srand((unsigned int)time(NULL));
	///Button 1	
        //digitalWrite(led[GetRandom(1,6)], LOW);
        randm = GetRandom(0,2);
        while( n == 0 )
        {
            count++;
            delay(1);
            if(count < 500){
                thoiGian = 0;
                softPwmWrite(led[randm], 100);
            }
            if(count >3000){
                thoiGian = 1;
                softPwmWrite(led[0], 0);
                softPwmWrite(led[1], 0);
                softPwmWrite(led[2], 0);
                printf("Ban da thua\n");
                delay(1000);
                break;
            }
            if(thoiGian == 0 && randm == state ){
                softPwmWrite(led[0], 0);
                softPwmWrite(led[1], 0);
                softPwmWrite(led[2], 0);
                delay(1000);
                softPwmWrite(led[0], 100);
                softPwmWrite(led[1], 100);
                softPwmWrite(led[2], 100);
                delay(1000);
                softPwmWrite(led[0], 0);
                softPwmWrite(led[1], 0);
                softPwmWrite(led[2], 0);

                printf("Ban da thang \n");
                delay(1000);
                n=2;
                break;
            }
        }

	return 0;
	
	}
