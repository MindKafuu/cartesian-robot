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

#define type 1

#PIN_SELECT INT1 = di0
#PIN_SELECT OC1 = do2
#PIN_SELECT OC2 = do3

char head[5];

char data[6] = {};
int flag = 0, count = 1;
int direction;
char check = 0, id = 14, lenght = 4;

int encount = 0;

int conDuty = 0, duty = 0, perCent = 0, dutyValue = 0, perDuty = 0;

int s, p, e, kk;
int u, Kp, Ki, Kd, setPoint;
int current = 0;

/*void data(char c[], int size){
	for(int i = 0; i < size; i++){
		//head[i] = c[i];
		if(count <= 2){
			if(c[i] == 0xff)
				count++;
		}else if(count <= 3){
			if(c[2] == type){
				head[0] = c[2];
				count++;
			}
		}else if(count <= 4){
			head[1] = c[3];
			count++;
		}else if(count > 4){
			check = (~(head[0] + head[1])) + 1;
			if(c[4] == abs(check)){
				count++;
				if(count > 5){
					flag = 1;
					count = 1;
				}
			}
		}
	}	
}*/

int convertEncoder(int encount){
	return current = encount * 30.0 / 2650.0;
}

int findPercent(int u){
	perDuty = u * 100.0 / 30.0;
	return perDuty;
}

int findDuty(int duty){
	if(duty > 100)
		duty = 100;
	else if(duty < -100)
		duty = -100;

	dutyValue = (int)duty * 6250.0 / 100.0;
	return dutyValue;
}

void Pwm(int perCent){
	if(perCent >= 0){
		set_pwm_duty(1, findDuty(perCent));
		set_pwm_duty(2, 0);
	}else{
		conDuty = abs(perCent);
		set_pwm_duty(1, 0);
		set_pwm_duty(2, findDuty(conDuty));
	}	
}

#INT_EXT1
void INT_EXT_INPUT1 ( void ){
	if(input(di0) == 0){
		encount--;
	}else{
		encount++;
	}	
	convertEncoder(encount);
}

#INT_TIMER2
void TIMER2_isr(void){
	set_tris_a (get_tris_a() & 0xffeb);
	setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
	setup_compare(2, COMPARE_PWM | COMPARE_TIMER2);
	kk = findPercent(u);
	if(direction == -1){
		Pwm(-kk);
		if(input(di2) == 0){
			Pwm(0);
		}
	}else if(direction == 1){
		Pwm(kk);
		if(input(di1) == 0){
			Pwm(0);
		}
	}
}
/*
#INT_RDA              
void UART1_Isr() {
	char c[5];
	gets(c);
	data(c, 4);
}*/

void Init_Interrupts() {
	enable_interrupts( INT_EXT1 );
	ext_int_edge( 1, L_TO_H );
}

void Init_Timer2() {
	setup_timer2(TMR_INTERNAL | TMR_DIV_BY_256, 6250);
	enable_interrupts(INT_TIMER2);
}

void main() {
	s = 0;
	p = 0;
	Kp = 5;
	Ki = 1;
	Kd = 15;
	setPoint = 15;
	direction = -1;

	disable_interrupts(GLOBAL);	
    //clear_interrupt(INT_RDA);  
	Init_Interrupts();  
	Init_Timer2();
	//enable_interrupts(INT_RDA);
	enable_interrupts(GLOBAL);

	while(TRUE) {
		
		if(flag == 1){
			//printf("direction = %u, angle = %u\n", direction, angle);
			flag = 0;
		}
		
		e = setPoint - current;
		s = s + e;
		if (abs(e) > 3){
			u = Kp * e + Ki * s + Kd * (e - p);
		}else {
	 		u = 0;
		}
		p = e;
		printf("current: %d\n", current);
		printf("error: %d\n", e);
		printf("percent: %d\n", kk);
		printf("u: %d\n", u);
		printf("encoder: %d\n", encount);
		delay_ms(50);
	}
}