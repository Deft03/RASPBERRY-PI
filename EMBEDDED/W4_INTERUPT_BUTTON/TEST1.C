// gcc -o ktra ktra_lan1
.c -lwiringPi

// Trần Thanh Sang  20146412


#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <softPwm.h>


uint8_t led[3] = {17, 27, 22};
uint8_t bt[3]  = {23, 24, 25};
volatile uint8_t state1 = 1, state2 =0;
uint8_t state = 10;
int count1, count2, count3;

void ngat_bt1(void){
	if(digitalRead(23) == 1)
	{
        state2 ++;
        softPwmWrite(17, 0);
        softPwmWrite(27, 0);
        softPwmWrite(22, 0);
        if(state2==3){
            state2=0;
        }
	}
}
/////////
void ngat_bt2(void){
	if(digitalRead(24) == 1)
	{
        softPwmWrite(17, 0);
        softPwmWrite(27, 0);
        softPwmWrite(22, 0);
        // Tang độ sáng
		state = state +10;
        state2 =2;
		if(state > 100){
			state=10;
		}
        softPwmWrite(17, state);
		delay(100);
	}
}

// Hàm random

int GetRandom(int min,int max){
    return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

int main(){
	wiringPiSetupGpio();
	//Khai báo chân và enable ngắt
	for(int i=0; i<3; i++){
		pinMode(led[i], OUTPUT);
		pinMode(bt[i] ,  INPUT);
		softPwmCreate(led[i], 0 , 100);
		}

	wiringPiISR(23, INT_EDGE_RISING, &ngat_bt1);
	wiringPiISR(24, INT_EDGE_RISING, &ngat_bt2);

    //Random ra 3 độ sáng khác nhay
    count1= GetRandom(0,100);
    count2= GetRandom(0,100);
    count3= GetRandom(0,100);

	while(1){
        /// Thay đổi qua lại giữa các màu 
        if(state2 == 1){  
            for(int i = 0; i<10; i++){   // vòng lặp for để sinh ra 10 màu khác nhau
                count1= GetRandom(0,100);
                count2= GetRandom(0,100);
                count3= GetRandom(0,100);

                softPwmWrite(17, count1);
                softPwmWrite(27, count2);
                softPwmWrite(22, count3);
                delay(500);

                softPwmWrite(17, 0);
                softPwmWrite(27, 0);
                softPwmWrite(22, 0);
                // So sánh để thoát khỏi vòng lặp for
                if(state2 == 0){
                    break;
                }
            }     
		}
        // Chớp tắt led đang hiển thị
        if(state2==0){
			softPwmWrite(17, 0);
            softPwmWrite(27, 0);
            softPwmWrite(22, 0);
			delay(333);
            softPwmWrite(17, count1);
            softPwmWrite(27, count2);
            softPwmWrite(22, count3);
            delay(333);

           // delay(10);

        }
    }
	return 0;
	
	}
