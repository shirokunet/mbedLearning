#include "mbed.h"
#include "MPU9250.h"
#include "TextLCD.h"

MPU9250 mpu9250(p9, p10);					// SDA, SCL
TextLCD lcd(p26, p25, p24, p23, p22, p21); 	// rs, e, d4-d7
Serial pc(USBTX, USBRX, 115200);

int main() 
{
	float dest1[3];
	float dest2[3];
	mpu9250.calibrateMPU9250(dest1, dest2);

	while(1)
	{
		/* get accell data*/
		int16_t acc[3];
		float acc_f[3];
		mpu9250.readAccelData(acc);
		for (int i = 0; i < 3; ++i) acc_f[i] = float(acc[i]) * mpu9250.getAres() - dest2[i];

		/* get temperature */
		float temperature = ((float) mpu9250.readTempData()) / 333.87 + 21.0;

		/* lcd */
		lcd.cls();
		lcd.printf("x:%1.2f,", acc_f[0]);
		lcd.printf("y:%1.2f", acc_f[1]);
		lcd.locate(0,1);
		lcd.printf("z:%1.2f,", acc_f[2]);
		lcd.printf("t:%2.1f", temperature);

		/* serial */
		pc.printf("dx:%3.3f\t", dest2[0]);
		pc.printf("dy:%3.3f\t", dest2[1]);
		pc.printf("dz:%3.3f\t", dest2[2]);
		pc.printf("ax:%3.3f\t", acc_f[0]);
		pc.printf("ay:%3.3f\t", acc_f[1]);
		pc.printf("az:%3.3f\t", acc_f[2]);
		pc.printf("temp:%3.3f\t", temperature);
		pc.printf("\r\n");

		wait_ms(100);
	}
}
