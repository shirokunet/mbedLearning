#include "mbed.h"

DigitalIn sw(p10);
PwmOut motor(p26);

int main()
{
	sw.mode(PullUp);
	Serial pc(USBTX, USBRX, 115200);

	const double change = 0.2;
	const double sleep = 2.0;
	int mode = 0;
	double duty = 0.0;

	while(1)
	{
		if(!sw)
		{
			switch (mode)
			{
				case 0:
					duty += change;
					if(duty >= 1.0)
						mode = 1;
					break;
				case 1:
					duty -= change;
					if(duty <= 0.0)
						mode = 0;
					break;
			}
		}
		else
		{
			mode = 0;
			duty = 0;
		}

		motor = duty;
		wait(sleep);

		pc.printf("sw:%d, duty:%f%%\r\n", int(sw),duty);
	}
}
