//gcc -o BT3 BT3.c -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdint.h>
#include <stdio.h>


uint8_t led[3] = {17, 27, 22};
uint8_t bt[3]  = {23, 24, 25};
volatile uint8_t state1 = 0;
uint8_t state = 50;


void ngat_bt1(void){
	if(digitalRead(23) == 1)
	{
		state1++;
		if(state1==4){
			state1= 0;
		}delay(100);
		
	}
}
void ngat_bt2(void){
	if(digitalRead(24) == 1)
	{
		state = state +10;
		printf("%d", state);
		if(state > 100){
			state=100;
		}
		delay(100);
	}
}
void ngat_bt3(void){
	if(digitalRead(25) == 1)
	{
		state = state -10;
		printf("%d", state);
		if(state < 10){
			state=10;
		}
		delay(100);
	}
}

int main(){
	wiringPiSetupGpio();
	//enable ngat
	for(int i=0; i<3; i++){
		pinMode(led[i], OUTPUT);
		pinMode(bt[i] ,  INPUT);
		softPwmCreate(led[i], 0 , 100);
		}

	wiringPiISR(23, INT_EDGE_RISING, &ngat_bt1);
	wiringPiISR(24, INT_EDGE_RISING, &ngat_bt2);
	wiringPiISR(25, INT_EDGE_RISING, &ngat_bt3);

	while(1){
	///Button 1
		if(state1==0){
			softPwmWrite(17, 0);
			softPwmWrite(27, 0);
			softPwmWrite(22, 0);
			delay(10);

		}
		if(state1==1){
			softPwmWrite(17, state);
			softPwmWrite(27, 0);
			softPwmWrite(22, 0);
			delay(10);
			
		}
		if(state1==2){
			softPwmWrite(17, 0);
			softPwmWrite(27, state);
			softPwmWrite(22, 0);
			delay(10);

		}
		if(state1==3){
			softPwmWrite(17, 0);
			softPwmWrite(27, 0);
			softPwmWrite(22, state);
			delay(10);
		}



	//Button2
		
	


	}
	return 0;
	
	}
