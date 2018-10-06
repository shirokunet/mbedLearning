#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

Timer tm1;
Timer tm2;
Ticker tk1;

#define LED1_cycle_ms 1000
#define LED2_cycle_ms 2000
#define LED3_cycle_ms 3000

void task_LED1();
void task_LED2();
void task_LED3();

int main() 
{
    tm1.start();
    tm2.start();
    
    tk1.attach(&task_LED3, float(LED3_cycle_ms)/1000.0);

    led1 = led2 = led3 = 1;

    while(1) 
    {
        if(tm1.read_ms()>=LED1_cycle_ms)
        {
            task_LED1();
            tm1.reset();
        }
        if(tm2.read_ms()>=LED2_cycle_ms)
        {
            task_LED2();
            tm2.reset();
        }
    }
}

void task_LED1(void)
{
    led1 = !led1;
}

void task_LED2(void)
{
    led2 = !led2;
}

void task_LED3(void)
{
    led3 = !led3;
}
