#include "mbed.h"

DigitalOut led(p5);

int main() 
{
	int fhz = 5;
	double sleeptime = 1.0/double(fhz);
    while(1) 
    {
        led = 1;
        wait(sleeptime);
        led = 0;
        wait(sleeptime);
    }
}
