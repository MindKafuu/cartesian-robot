#include <24FJ48GA002.h>
#include "BL_Support.h"
#use delay(internal = 8 MHz, clock = 32MHz)
#PIN_SELECT U1RX = PIN_B12 //PIN_B14 //
#PIN_SELECT U1TX = PIN_B13 //PIN_B15 //
#use rs232(UART1, BAUD = 115200, XMIT = PIN_B13, RCV = PIN_B12)

unsigned int8 positionX[2] = {};
unsigned int8 positionY[2] = {};
unsigned int8 data[3] = {};
unsigned int8 check[2] = {};
int check_data = 1, checksum = 0;
unsigned int8 getPackage = 0, theta = 0;

char *print_float(float data)
{
	int intDist = data / 1;
	int dotDist = (((intDist >> 15) * -2) + 1) * ((data * 1000.0f) - (intDist * 1000));
	char stringFloat[20];
	sprintf(stringFloat, "%d.%d", intDist, dotDist);
	return stringFloat;
}

void print_float(char *stringResult, float data)
{
	int intDist = data / 1;
	int dotDist = (((intDist >> 15) * -2) + 1) * ((data * 1000.0f) - (intDist * 1000));
	sprintf(stringResult, "%d.%d", intDist, dotDist);
}

void SM_RxD(unsigned int8 c)
{
	// printf("%c", check_data);
	if (check_data <= 2)
	{
		if (c == 255)
		{
			check_data++;
		}
		else
			check_data = 1;
	}
	else if (check_data > 2 && check_data <= 5)
	{
		data[check_data - 3] = c;
		check_data++;
	}
	else if (check_data <= 6)
	{
		theta = c;
		check_data++;
	}
	else if (check_data > 6 && check_data <= 8)
	{
		positionX[check_data - 7] = c;
		check_data++;
	}
	else if (check_data > 8 && check_data <= 10)
	{
		positionY[check_data - 9] = c;
		check_data++;
	}
	
	else if (check_data > 10 && check_data <= 12)
	{
		check[check_data - 11] = c;
		check_data++;
	}
	if (check_data > 12)
	{
		unsigned int8 checksum_1, checksum_2;
		checksum = (((int)theta + (int)positionX[0] + (int)positionX[1] + (int)positionY[0] + (int)positionY[1]) - ((int)data[0] + (int)data[1] + (int)data[2])) + 1;
		if(checksum < 0)
			checksum = checksum * -1;
		checksum_1 = (checksum >> 8) & 0xFF;
		checksum_2 = checksum & 0xFF;
		check_data++;

		if (checksum_1 == check[0] && checksum_2 == check[1])
		{
			check_data++;
			if (check_data > 11)
			{
				getPackage = 1;
				check_data = 1;
			}
		}
	}
}

#INT_RDA // receive data interrupt one time per one
void UART1_Isr()
{
	unsigned int8 c = getc();
	SM_RxD(c);
}

void handdleData()
{
	if (getPackage >= 1)
	{
		printf("%d\n", check_data);
		printf("end\n");
		getPackage = 0;
	}
}

void main()
{
	disable_interrupts(GLOBAL);

	clear_interrupt(INT_RDA); // recommend style coding to confirm everything clear before use
	enable_interrupts(INT_RDA);

	enable_interrupts(GLOBAL);
	printf("System Ready!\n");
	while (TRUE)
	{
		handdleData();
	}
}
