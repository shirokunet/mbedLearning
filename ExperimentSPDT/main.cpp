#include "mbed.h"

DigitalIn sw(p10);

Serial pc(USBTX, USBRX, 115200);

#define BUFFERSIZE 2048
#define SWMODE PullDown

bool signal[2] = {true, false};

Timer timer;

int main()
{
	pc.printf("----------\r\n");
	timer.start();

	/* swmode */
	sw.mode(SWMODE);
	if(SWMODE == PullUp)
		pc.printf("SWMODE:PullUp\r\n");
	else if(SWMODE == PullDown)
		pc.printf("SWMODE:PullDown\r\n");

	timer.reset();
	for (int i=0; i<2; i++)
	{
		int buffer[BUFFERSIZE] = {0};
		int timerus[BUFFERSIZE] = {0};

		/* wait */
		while(sw.read() == signal[i]);

		/* logging */
		for (int t=0; t<BUFFERSIZE; t++)
		{
			buffer[t] = sw.read();
			timerus[t] = timer.read_us();
		}

		/* analyze */
		int edge_count = 0;
		bool value_z1 = signal[i];
		int startus = 0;
		int endus = 0;
		for (int t=0; t<BUFFERSIZE; t++)
		{
			if (value_z1 != buffer[t])
			{
				if(t==0)
				{
					startus = timerus[t];
				}
				endus = timerus[t];
				pc.printf("%d-%d-%d\r\n", buffer[t], t, timerus[t]);
				edge_count ++;
			}
			value_z1 = buffer[t];
		}

		/* result */
		pc.printf("result(%d>%d),count:%d,us:%d\r\n", signal[i], !signal[i], edge_count, endus-startus);
	}
}
