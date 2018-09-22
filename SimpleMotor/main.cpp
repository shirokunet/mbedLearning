#include "mbed.h"

DigitalOut led(LED1);
DigitalOut motor(p14);
DigitalIn sw(p10);

int main() 
{
	sw.mode(PullUp);
	while(1) 
	{
		if(!sw){
			motor = 1;
			led = 1;
		}
		else{
			motor = 0;
			led = 0;
		}
	}
}
