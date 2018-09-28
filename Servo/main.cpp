#include "mbed.h"

PwmOut servo(p26);

Serial pc(USBTX, USBRX, 115200);

int main()
{
	int pos = 0;
	const int change = 10;
	const double sleep = 0.5;
	double pulse = 0.0;

	servo.period(0.020);

	while(1)
	{
		for (pos = 0; pos <= 90; pos += change) {
			pulse = 0.001 + 0.001/90.0*double(pos);
			servo.pulsewidth(pulse);
			pc.printf("percentage: %3.4f%%\r\n", pulse);
			wait(sleep);
	  	}
		for (pos = 90; pos >= 0; pos -= change) {
			pulse = 0.001 + 0.001/90.0*double(pos);
			servo.pulsewidth(pulse);
			pc.printf("percentage: %3.4f%%\r\n", pulse);
			wait(sleep);
		}
	}
}
