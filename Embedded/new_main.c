#include <24FJ48GA002.h>
#include "BL_Support.h"
#use delay(internal = 8 MHz, clock = 32MHz)
#PIN_SELECT U1RX = PIN_B12 //PIN_B14 //
#PIN_SELECT U1TX = PIN_B13 //PIN_B15 //
#use rs232(UART1, BAUD = 115200, XMIT = PIN_B13, RCV = PIN_B12)

#define do0 PIN_A2
#define do1 PIN_A4
#define do2 PIN_B2
#define do3 PIN_B3
#define di0 PIN_B4
#define di1 PIN_B5
#define di2 PIN_B6
#define di3 PIN_B7

char positionX[2] = {};
char positionY[2] = {};
char data[3] = {};
char check_data = 1, checksum = 0;
int getPackage = 0, theta = 0;

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

void SM_RxD(int c)
{
	if (check_data <= 2)
	{
		if (c == 255)
			check_data++;
		else
			check_data = 1;
	}
	else if (check_data <= 3 && check_data <= 5)
	{
		data[check_data - 3] = c;
		check_data++;
	}
	else if (check_data <= 6)
	{
		theta = c;
		check_data++;
	}
	else if (check_data <= 7 && check_data <= 8)
	{
		positionX[check_data - 7] = c;
		check_data++;
	}
	else if (check_data <= 9 && check_data <= 10)
	{
		positionY[check_data - 9] = c;
		check_data++;
	}
	else if (check_data > 10)
	{
		checksum = ~((theta + positionX[0] + positionX[1] + positionY[0] + positionY[1]) - (data[0] + data[1] + data[2])) + 1;
		check_data++;
		if (c == abs(checksum))
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
	int c = getc();
	SM_RxD(c);
}

void handdleData()
{
	if (getPackage >= 1)
	{
		float test;
		memcpy(&test, array, sizeof(test));
		printf("\nresult = %s, %s\n", print_float(test), print_float(test));

		if (data[0] == 0)
		{
			setPosition();	//ยังไม่ได้ทำ
		}
		else if (data[0] == 1)
		{
			gripper();		//ยังไม่ได้ทำ
		}
		else if (data[0] == 2)
		{
			move();			//ยังไม่ได้ทำ
		}
		getPackage = 0;
	}
}

void main()
{
	disable_interrupts(GLOBAL);

	clear_interrupt(INT_RDA); // recommend style coding to confirm everything clear before use
	enable_interrupts(INT_RDA);

	enable_interrupts(GLOBAL);
	printf("System Ready!\r\n");
	while (TRUE)
	{
		handdleData();
	}
}
