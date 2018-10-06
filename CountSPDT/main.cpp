#include "mbed.h"

InterruptIn sw(p10);
DigitalOut led1(LED1);
Timer debounce;
Serial pc(USBTX, USBRX, 115200);

volatile int counter = 0;
bool signal = false;
void count(void);

int main()
{
	debounce.start();
	sw.rise(&count);
	sw.fall(&count);

	while(true)
	{
		if (counter >= 10)
		{
			pc.printf("%d>%d\r\n", int(led1), int(!led1));
			led1 = !led1;
			counter = 0;
		}
	}
}

void count()
{
	debounce.reset();
	while(sw.read() == signal)
	{
		if (debounce.read_us() > 1500)
		{
			pc.printf("signal:%d,counter:%d,timer:%d\r\n", signal, counter, debounce.read_us());
			signal = !signal;
			counter ++;
			break;
		}
	}
}
