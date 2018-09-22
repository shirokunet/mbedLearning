#include "mbed.h"

DigitalOut led(p5);
DigitalIn sw(p10);

int main() 
{
	sw.mode(PullUp);
	int fhz = 5;
	double sleeptime = 0.0;
	while(1) 
	{
		if(!sw)
			fhz = 10;
		else
			fhz = 5;
		sleeptime = 1.0/double(fhz)/2.0;

		led = 1;
		wait(sleeptime);
		led = 0;
		wait(sleeptime);
	}
}
