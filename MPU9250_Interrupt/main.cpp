#include "mbed.h"
#include "MPU9250.h"
#include "TextLCD.h"

InterruptIn mpu9250_IntPin(p27);
MPU9250 mpu9250(p9, p10);					// SDA, SCL
TextLCD lcd(p26, p25, p24, p23, p22, p21); 	// rs, e, d4-d7
Serial pc(USBTX, USBRX, 115200);

volatile bool interrupt_flag = false;

void detect_shock(void)
{
	interrupt_flag = true;
}

int main() 
{
	mpu9250_IntPin.rise(&detect_shock);

	mpu9250.resetMPU9250();
	mpu9250.setWakeOnMotion();

	while(1)
	{
		if (interrupt_flag)
		{
			lcd.cls();
			lcd.printf("detect");
			pc.printf("detect\r\n");
			wait_ms(1000);
			interrupt_flag = false;
		}

		/* get accell data*/
		int16_t acc[3];
		mpu9250.readAccelData(acc);
		float acc_f[3];
		for (int i = 0; i < 3; ++i)
			acc_f[i] = float(acc[i]) * mpu9250.getAres();

		/* get temperature */
		float temperature = ((float) mpu9250.readTempData()) / 333.87 + 21.0;

		/* lcd */
		lcd.cls();
		lcd.printf("%d,", int(mpu9250_IntPin));
		lcd.printf("x:%1.2f,", acc_f[0]);
		lcd.printf("y:%1.2f", acc_f[1]);
		lcd.locate(0,1);
		lcd.printf("z:%1.2f,", acc_f[2]);
		lcd.printf("t:%2.1f", temperature);

		/* serial */
		pc.printf("int:%d\t", int(mpu9250_IntPin));
		pc.printf("ax:%3.3f\t", acc_f[0]);
		pc.printf("ay:%3.3f\t", acc_f[1]);
		pc.printf("az:%3.3f\t", acc_f[2]);
		pc.printf("temp:%3.3f\t", temperature);
		pc.printf("\r\n");

		wait_ms(100);
	}
}
