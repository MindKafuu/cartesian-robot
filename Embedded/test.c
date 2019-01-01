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

#PIN_SELECT INT1 = di0
#PIN_SELECT OC1 = do2
#PIN_SELECT OC2 = do3

char data[6] = {};
int flag = 0, count = 1;
int direction;
char check = 0, id = 14, lenght = 4;

int encount = 0;

int conDuty = 0, duty = 0, perCent = 0, dutyValue = 0, perDuty = 0;

int s, p, e, kk;
int u, Kp, Ki, Kd, setPoint;
int current = 0;


/*void receive(int c){
	if(count <= 2){
		if(c = 255)
			count++;
	}else if (count <= 3){
		if (c == id){
			count++;
		}
	}else if (count <= 4){
		if (c == lenght){
			count++;
		}
	}else if (count > 4 && count <= 8){
		data1[count - 5] = c;
		count++;
	}else if (count > 8 && count <= 10){
		data2[count - 9] = c;
		count++;
	}else if (count > 10){
		check = (~(id + lenght + data1[0] + data1[1]+ data1[2] + data1[3] + data2[0] + data2[1])) + 1;
		sum = c;
		count++;	
		if (c == abs(check)) 
		{
			count++;
			if(count > 11){
				flag = 1;
				count = 1;
			}
		}
	}
}*/

int convertEncoder(int encount){
	return current = encount * 40.0 / 6000.0;
}

int findPercent(int u){
	perDuty = u * 100.0 / 40.0;
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

/*#INT_RDA              
void UART1_Isr() {
	int c = getc();
	receive(c);
}
*/
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
	setPoint = 20;
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
		//printf("%d\n", encount);
		//printf("SENSOR: %d\n", sensor);
		printf("current: %d\n", current);
		printf("error: %d\n", e);
		printf("percent: %d\n", kk);
		printf("u: %d\n", u);
		printf("encoder: %d\n", encount);
		delay_ms(50);
	}
}