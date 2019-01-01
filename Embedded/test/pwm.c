#include<24FJ48GA002.h>
#include "BL_Support.h"
#include <math.h>
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

#PIN_SELECT OC1 = do2

long count = 0;
int dutyValue = 0, duty, perCent;

void setRight() {
	output_high(do0);
	output_high(do1);
}

void setLeft() {
	output_low(do0);
	output_high(do1);
}

int findDuty(int duty){
	dutyValue = duty * 6250.0 / 100.0;
	return dutyValue;
}

void Pwm(int perCent){
	setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
	set_pwm_duty(1, findDuty(perCent));
}

#INT_TIMER2
void TIMER2_isr(void){
	set_tris_a (get_tris_a() & 0xffeb);
	Pwm(50);
}

void Init_Timer2() {
	setup_timer2(TMR_INTERNAL | TMR_DIV_BY_256, 625);
	enable_interrupts(INT_TIMER2);
}

int main() {
	disable_interrupts(GLOBAL);
	Init_Timer2();
	enable_interrupts(GLOBAL);
	setLeft();

	while(TRUE) {
	}
	return(0);
}