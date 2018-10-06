#include "mbed.h"

InterruptIn sw(p10);
DigitalOut led1(LED1);
Timer debounce;
Serial pc(USBTX, USBRX, 115200);

void on(void);
void off(void);

int main()
{
	debounce.start();
	sw.rise(&on);
	sw.fall(&off);
}

void toggle(bool signal)
{
	debounce.reset();
	while(true)
	{
		if (debounce.read_us() > 1500)
		{
			led1 = signal;
			pc.printf("(%d>%d):%d\r\n", !signal, signal, debounce.read_us());
			break;
		}
	}
}

void on(void)
{
	toggle(true);
}

void off(void)
{
	toggle(false);
}
