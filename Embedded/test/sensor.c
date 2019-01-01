#include<24FJ48GA002.h>
#include "BL_Support.h"
#use delay(internal = 8 MHz, clock = 32Mhz)
#PIN_SELECT U1RX = PIN_B12
#PIN_SELECT U1TX = PIN_B13
#use rs232(UART1, BAUD = 115200, XMIT = PIN_B13, RCV = PIN_B12)

#define do0 PIN_A2
#define do1 PIN_A4
#define do2 PIN_B2
#define do3 PIN_B3
#define di0 PIN_B4
#define di1 PIN_B5
#define di2 PIN_B6
#define di3 PIN_B7

#PIN_SELECT INT1 = di1

int sensor = 0;

#INT_EXT1
void INT_EXT_INPUT1 ( void ){
	if(input(di1) == 0){
		sensor = 1;
	}else{
		sensor = 0;
	}	
}

void Init_Interrupts() {
	enable_interrupts( INT_EXT1 );
	ext_int_edge( 1, L_TO_H );
}

void main() {
	encount = 0;
	disable_interrupts(GLOBAL);	 
	Init_Interrupts();  
	enable_interrupts(GLOBAL);

	while(TRUE) {
		printf("%d\n", sensor);
		delay_ms(500);
	}
}