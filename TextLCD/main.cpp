#include "mbed.h"
#include "TextLCD.h"

//TextLCD lcd(p19, p20, p24, p23, p22, p21); // rs, e, d4-d7

//I2C i2c_lcd(p28,p27); // SDA, SCL
I2C i2c_lcd(p9,p10); // SDA, SCL
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);

Serial pc(USBTX, USBRX, 115200);

int main() 
{
	static int i;
	while(1)
	{
		lcd.cls();
//		lcd.printf("Hello World");
		lcd.putc(3);
		lcd.locate(0,1);
		lcd.putc('+');
//		lcd.printf("%d",i++);
		wait_ms(100);

		pc.printf("test\r\n");
	}
}
