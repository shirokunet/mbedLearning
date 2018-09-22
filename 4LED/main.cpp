/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
*/
#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

int main() 
{
    while(1) 
    {
        led1 = 1;
        led2 = 1;
        led3 = 1;
        led4 = 1;
        wait(2.0);

        led1 = 0;
        led2 = 0;
        led3 = 0;
        led4 = 0;
        wait(2.0);
    }
}
