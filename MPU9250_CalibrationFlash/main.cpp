#include "mbed.h"
#include "IAP.h"
#include "MPU9250.h"
#include "TextLCD.h"

#define MEM_SIZE      512	// memory buffer size must be either 256, 512, 1024 or 4096 when copying to flash
#define TARGET_SECTOR  29	//  use sector 29 as target sector if it is on LPC1768

DigitalOut led(LED1);
DigitalOut led2(LED2);
MPU9250 mpu9250(p9, p10);                	// SDA, SCL
TextLCD lcd(p26, p25, p24, p23, p22, p21);	// rs, e, d4-d7
Serial pc(USBTX, USBRX, 115200);
IAP iap;

volatile bool fKeyInput = false;

struct DATA
{
    float x1;
    float y1;
    float z1;
    float x2;
    float y2;
    float z2;
};

void Rx_interrrupt()
{
    led = !led;
    char c = pc.getc();

    if(!fKeyInput && (c == 'c' || c == 'C'))
    {
        led2 = !led2;
        fKeyInput = true;
    }
}

int eraseData()
{
    pc.printf("TARGET SECTOR is NOT BLANK!  Erasing...\r\n");
    iap.prepare( TARGET_SECTOR, TARGET_SECTOR );    // Always must prepare sector before erasing or writing
    int r = iap.erase( TARGET_SECTOR, TARGET_SECTOR );
    pc.printf( "erase result       = 0x%08X\r\n", r );
    return r;
}

int writeData(float *calib_data_1st, float *calib_data_2nd)
{
    static char mem[ MEM_SIZE ];
    char *p;
    int r;
    struct DATA dataA;

    pc.printf("DATA1:\tx = %f\ty = %f\tz = %f\r\n", calib_data_1st[0], calib_data_1st[1], calib_data_1st[2]);
    pc.printf("DATA1:\tx = %f\ty = %f\tz = %f\r\n", calib_data_2nd[0], calib_data_2nd[1], calib_data_2nd[2]);

    dataA.x1 = calib_data_1st[0];
    dataA.y1 = calib_data_1st[1];
    dataA.z1 = calib_data_1st[2];
    dataA.x2 = calib_data_2nd[0];
    dataA.y2 = calib_data_2nd[1];
    dataA.z2 = calib_data_2nd[2];

    // copy struct data into RAM mem buffer
    p = (char *)&dataA;

    memset(&mem[0], 0, sizeof(mem));    // Set all elements of mem array to 0

    // Copy data struct into mem array
    for(uint i = 0; i< sizeof(dataA);++i)
        mem[i] = *(p+i);

    // copy RAM to Flash
    iap.prepare( TARGET_SECTOR, TARGET_SECTOR );    // Always must prepare sector before erasing or writing
    r = iap.write( mem, sector_start_adress[ TARGET_SECTOR ], MEM_SIZE );

    return r;
}

int readData(float *calib_data_1st, float *calib_data_2nd)
{
    int r;
    struct DATA *Aptr;  // Pointer to structure with format of data
    
    // Check to see if TARGET_SECTOR is BLANK
    r = iap.blank_check( TARGET_SECTOR, TARGET_SECTOR );
    if ( r == SECTOR_NOT_BLANK ) {
        Aptr = (struct DATA *) FLASH_SECTOR_29;
        pc.printf("DATA1:\tx = %f\ty = %f\tz = %f\r\n",Aptr->x1,Aptr->y1,Aptr->z1);
        pc.printf("DATA2:\tx = %f\ty = %f\tz = %f\r\n",Aptr->x2,Aptr->y2,Aptr->z2);
    } else{
        pc.printf("TARGET SECTOR IS BLANK!  No data available\r\n");
    }

    calib_data_1st[0] = Aptr->x1;
    calib_data_1st[1] = Aptr->y1;
    calib_data_1st[2] = Aptr->z1;
    calib_data_2nd[0] = Aptr->x2;
    calib_data_2nd[1] = Aptr->y2;
    calib_data_2nd[2] = Aptr->z2;

    return r;
}

int main()
{
    pc.attach(&Rx_interrrupt, Serial::RxIrq);

    /* initial calibration */
    pc.printf( "\r\n=== Initial Calibration ===\r\n" );
    float calib_data_gyro[3];
    float calib_data_accel[2][3];
    if (SECTOR_NOT_BLANK != iap.blank_check( TARGET_SECTOR, TARGET_SECTOR ))
    {
        pc.printf("Not Found Calibration Data\r\n");
        mpu9250.calibrateMPU9250(calib_data_gyro, calib_data_accel[0]);
        mpu9250.calibrateMPU9250(calib_data_gyro, calib_data_accel[1]);
        pc.printf("Write Calibration Data to Flash...\r\n");
        writeData(calib_data_accel[0], calib_data_accel[1]);
        pc.printf("Complete\r\n");
    }
    else{
        pc.printf("Found Calibration Data\r\n");
        pc.printf("Read Calibration Data from Flash...\r\n");
        readData(calib_data_accel[0], calib_data_accel[1]);
        pc.printf("Complete\r\n");
    }
    pc.printf( "===========================\r\n\r\n" );

    while(1)
    {
        /* interrupt calibration */
        if(fKeyInput)
        {
            fKeyInput = false;
            pc.attach(NULL, Serial::RxIrq);

            pc.printf( "\r\n=== Interrupt Calibration ===\r\n" );
            if (SECTOR_NOT_BLANK == iap.blank_check( TARGET_SECTOR, TARGET_SECTOR ))
                eraseData();
            pc.printf("Get New Calibration Data\r\n");
	        mpu9250.calibrateMPU9250(calib_data_gyro, calib_data_accel[0]);
	        mpu9250.calibrateMPU9250(calib_data_gyro, calib_data_accel[1]);
            pc.printf("Write Calibration Data to Flash...\r\n");
            writeData(calib_data_accel[0], calib_data_accel[1]);
            pc.printf("Complete\r\n");
            pc.printf( "=============================\r\n\r\n" );

            pc.attach(&Rx_interrrupt, Serial::RxIrq);
        }

        /* get accell data*/
        int16_t acc[3];
        float acc_f[3];
        mpu9250.readAccelData(acc);
        for (int i = 0; i < 3; ++i)
        	acc_f[i] = float(acc[i]) * mpu9250.getAres() - (calib_data_accel[0][i] - calib_data_accel[1][i])/2.0;

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
        pc.printf("dx:%3.3f,%3.3f,", calib_data_accel[0][0], calib_data_accel[1][0]);
        pc.printf("dy:%3.3f,%3.3f,", calib_data_accel[0][1], calib_data_accel[1][1]);
        pc.printf("dz:%3.3f,%3.3f,", calib_data_accel[0][2], calib_data_accel[1][2]);
        pc.printf("\t");
        pc.printf("ax:%3.3f,", acc_f[0]);
        pc.printf("ay:%3.3f,", acc_f[1]);
        pc.printf("az:%3.3f,", acc_f[2]);
        pc.printf("\t");
        pc.printf("temp:%3.3f", temperature);
        pc.printf("\t");
        pc.printf("(Press 'c' or 'C' to calibrate)\r\n");

        wait(1.0);
    }
}
