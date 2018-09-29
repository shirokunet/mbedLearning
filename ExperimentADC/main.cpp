#include "mbed.h"

AnalogIn ain(p15);
PwmOut pwm(p26);

Serial pc(USBTX, USBRX, 115200);

#define BUFFERSIZE 4096

int main()
{
	float buffer[BUFFERSIZE];
	int t=0;
	int value = 0;
	int value_z1 = 0;
	int store[2] = {0};

	/* logging */
	pwm = 0.5;
	for (t=0; t<BUFFERSIZE; t++)
	{
		buffer[t] = ain.read();
	}

	/* analyze */
	t = 0;
	for (int i=0; i<2; i++)
	{
		while (t < BUFFERSIZE)
		{
			value = buffer[t];
			if (value_z1 == 0 && value == 1)
			{
				store[i] = t;
				pc.printf("%d: %d\r\n", i, t);
				value_z1 = value;
                break;
			}
			t++;
			value_z1 = value;
		}
	}

	/* result */
	double adcCycle = 0.02 / double(store[1] - store[0]);
	pc.printf("cycle[s]: %f\r\n", adcCycle);
	pc.printf("sampling[kHz]: %f\r\n", 0.001/adcCycle);
}
