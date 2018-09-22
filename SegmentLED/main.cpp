#include "mbed.h"

#define DIGIT_ALL			0b100000001000000010000010
#define DIGIT_NUM_ALL		0b1000001110000001101000001
#define DIGIT_DP			0b0000000000000000001000000

const int digit_mask[4] = {
							0b100000000000000000000000,
							0b000000001000000000000000,
							0b000000000000000000000010,
							0b000000000000000010000000
						};

const int digit_num[10] = {
							0b0000001110000001100000001,
							0b0000000110000000000000000,
							0b1000000100000001100000001,
							0b1000000110000000100000001,
							0b1000001110000000000000000,
							0b1000001010000000100000001,
							0b1000001010000001100000001,
							0b0000001110000000000000001,
							0b1000001110000001100000001,
							0b1000001110000000000000001
						};

PortOut digit_port(Port0, DIGIT_ALL);
PortOut digitnum_port(Port0, DIGIT_NUM_ALL);

Serial pc(USBTX, USBRX, 115200);

int main() {
	int count = 0;
	int digit[4] = {0};
	while(1) {
		count += 1;
		for (int i=sizeof(digit)/sizeof(digit[0])-1; i>=0; i--){
			/* pow function */
			int denominator = 1;
			for (int j=0; j<i; j++){
				denominator = denominator * 10;
			}

			/* store digit number */
			digit[i] = (count/denominator)%10;

			/* blink LED */
			digit_port = digit_mask[i];
			digitnum_port = digit_num[digit[i]];

			pc.printf("%d,", digit[i]);
		}
		pc.printf(": %d\r\n", count);
		wait(0.5);
	}
}
