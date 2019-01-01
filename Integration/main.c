#include <24FJ48GA002.h>
#include "BL_Support.h"
#use delay(internal = 8 MHz, clock = 32Mhz)
#PIN_SELECT U1RX = PIN_B12
#PIN_SELECT U1TX = PIN_B13
#use rs232(UART1, BAUD = 115200, XMIT = PIN_B13, RCV = PIN_B12)

#define sensorX PIN_B8
#define sensorY PIN_B9
#define sensorZ PIN_B10
/////////SENSOR////////////////
#define motorX1 PIN_A0
#define motorX2 PIN_A1
#define motorX_Pwm PIN_B0
#define motorX_Encoder PIN_B7
///////// X //////////////////
#define motorY1 PIN_A2
#define motorY2 PIN_A4
#define motorY_Pwm PIN_B2
#define motorY_Encoder PIN_B6
/////////// Y ////////////////
#define motorZ1 PIN_B1
#define motorZ2 PIN_B3
#define motorZ_Pwm PIN_B4
#define motorZ_Encoder PIN_B5
/////////// Z /////////////////
#define servo_down PIN_B14
#define servo_up PIN_B15

#PIN_SELECT OC1 = motorX_Pwm
#PIN_SELECT OC2 = motorY_Pwm
#PIN_SELECT OC3 = motorZ_Pwm
#PIN_SELECT INT1 = motorY_Encoder
#PIN_SELECT INT2 = motorZ_Encoder

long encountX = 0, encountY = 0, encountZ = 0;
int directionX = 0, directionY = 0, directionZ = 0;

unsigned int8 positionX[2] = {};
unsigned int8 positionY[2] = {};
unsigned int8 data[2] = {};
unsigned int8 check[2] = {};
int check_data = 1, checksum = 0;
unsigned int8 getPackage = 0, theta = 0;

long setpointX = 0, setpointY = 0;
long errorMotorX = 0, sumErrorX = 0, pwmX = 0, pX = 0;

typedef struct ModeSystem
{
    unsigned int8 mode;
} Mode;

Mode modeSystem;

void FowardX()
{
    output_high(motorX1);
    output_low(motorX2);
    directionX = 1;
}

void BackWardX()
{
    output_low(motorX1);
    output_high(motorX2);
    directionX = 0;
}

void StopX()
{
    output_low(motorX1);
    output_low(motorX2);
}

void FowardY()
{
    output_high(motorY1);
    output_low(motorY2);
    directionY = 1;
}

void BackWardY()
{
    output_low(motorY1);
    output_high(motorY2);
    directionY = 0;
}

void StopY()
{
    output_low(motorY1);
    output_low(motorY2);
}

void FowardZ()
{
    output_high(motorZ1);
    output_low(motorZ2);
    directionZ = 1;
}

void BackWardZ()
{
    output_low(motorZ1);
    output_high(motorZ2);
    directionZ = 0;
}

void StopZ()
{
    output_low(motorZ1);
    output_low(motorZ2);
}

void SM_RxD(unsigned int8 c)
{
    if (check_data <= 2)
    {
        if (c == 255)
        {
            check_data++;
        }
        else
            check_data = 1;
    }
    else if (check_data <= 3)
    {
        modeSystem.mode = c;
        check_data++;
    }
    else if (check_data > 3 && check_data <= 5)
    {
        data[check_data - 4] = c;
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
        checksum = (((int)theta + (int)positionX[0] + (int)positionX[1] + (int)positionY[0] + (int)positionY[1]) - ((int)modeSystem.mode + (int)data[0] + (int)data[1])) + 1;
        if (checksum < 0)
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

void SetZero()
{
    if(input(sensorZ) == 0)
    {
        BackWardZ();
        set_pwm_duty(3, 2000);
    }
    else if(input(sensorZ) == 1)
    {
        StopZ();
        set_pwm_duty(3, 0);
        if(input(sensorY) == 1)
        {
            BackWardY();
            set_pwm_duty(2, 2000);
        }
        else if(input(sensorY) == 0)
        {
            StopY();
            set_pwm_duty(2, 0);
            if(input(sensorX) == 1)
            {
                BackWardX();
                set_pwm_duty(1, 500);
            }
            else if(input(sensorX) == 0)
            {
                StopX();
                set_pwm_duty(1, 0);
            }
        }
    }
}

void SetX(int pwmX)
{
    if (pwmX > 2000)
    {
        pwmX = 2000;
    }
    if (pwmX < -2000)
    {
        pwmX = -2000;
    }
    
    if (errorMotorX > 0)
    {
        FowardX();
        set_pwm_duty(1, pwmX);
    }
    else if (errorMotorX < 0)
    {
        BackWardX();
        set_pwm_duty(1, pwmX);
    }
    else if (errorMotorX == 0)
    {
        StopX();
        set_pwm_duty(1, 0);
    }
}

#INT_EXT0
void INT_EXT_INPUT0(void)
{
    if (directionX == 0)
    {
        encountX--;
    }
    else
    {
        encountX++;
    }
}

#INT_EXT1
void INT_EXT_INPUT1(void)
{
    if (directionY == 0)
    {
        encountY--;
    }
    else
    {
        encountY++;
    }
}

#INT_EXT2
void INT_EXT_INPUT2(void)
{
    if (directionZ == 0)
    {
        encountZ--;
    }
    else
    {
        encountZ++;
    }
}

#INT_TIMER1
void TIMER1_ist()
{
    errorMotorX = setPointX - encountX;
    sumErrorX = sumErrorX + errorMotorX;
    if (abs((int)errorMotorX) > 300)
    {
        pwmX = (int)10 * errorMotorX + 1 * sumErrorX + 5 * (errorMotorX - pX);
    }
    else
    {
        pwmX = 0;
    }
    pX = errorMotorX;

}

#INT_RDA
void UART1_Isr()
{
    unsigned int8 c = getc();
    SM_RxD(c);
}

void Init_Interrupts()
{
    enable_interrupts(INT_EXT0);
    ext_int_edge(0, L_TO_H);
    enable_interrupts(INT_EXT1);
    ext_int_edge(1, L_TO_H);
    enable_interrupts(INT_EXT2);
    ext_int_edge(2, L_TO_H);

    clear_interrupt(INT_RDA);
    enable_interrupts(INT_RDA);

    setup_timer1(TMR_INTERNAL | TMR_DIV_BY_256, 625);
    enable_interrupts(INT_TIMER1);
}

void main()
{
    disable_interrupts(GLOBAL);
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 2000);
    set_tris_a(get_tris_a() & 0xffeb);
    setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(2, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(3, COMPARE_PWM | COMPARE_TIMER2);

    Init_Interrupts();
    enable_interrupts(GLOBAL);
    
    setPointX = 1500;

    while(TRUE)
    {
        SetZero();
        delay_ms(1000);
        // SetX(pwmX);
        // if(getPackage >= 1)
        // {
        //     setpointX = (long)positionX[1] * 1500 / 15;
        //     setpointY = (long)positionY[1] * 2000 / 4;

        //     getPackage = 0;
        // }

        printf("pwmX: %d\n", (int)pwmX);

        // printf("encoderX: %d\n", (int)encountX);
        // printf("encoderY: %d\n", (int)encountY);
        // printf("encoderZ: %d\n", (int)encountZ);
        // printf("sensorX: %d\n", input(sensorX));
        // printf("sensorY: %d\n", input(sensorY));
        // printf("sensorZ: %d\n", input(sensorZ));

        delay_ms(50);
    }
    
}