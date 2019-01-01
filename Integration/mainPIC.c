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
#define something PIN_B14
#define set_zero 0
#define move 1
#define pick_up 2

#PIN_SELECT OC1 = motorX_Pwm
#PIN_SELECT OC2 = motorY_Pwm
#PIN_SELECT OC3 = something
#PIN_SELECT INT1 = motorY_Encoder
#PIN_SELECT INT2 = motorZ_Encoder

long encountX = 0, encountY = 0, encountZ = 0;

unsigned int8 positionX[2] = {};
unsigned int8 positionY[2] = {};
unsigned int8 data[2] = {};
unsigned int8 check[2] = {};
int check_data = 1, checksum = 0;
unsigned int8 getPackage = 0, theta = 0;

float errorMotorX = 0, errorMotorY = 0, errorMotorZ = 0, setPointX = 0, setPointY = 0, setPointZ = 0, currentX = 0, currentY = 0, currentZ = 0;
float pwmX = 0, pwmY = 0, pwmZ = 0, Kp_X = 0, Kp_Y = 0, Kp_z = 0, Ki_X = 0, Ki_Y = 0, Ki_z = 0, Kd_X = 0, Kd_Y = 0, Kd_Z = 0, pX = 0, pY = 0, pZ = 0;
float sumErrorX = 0, sumErrorY = 0, sumErrorZ = 0;

// enum checkState {Zero, Move, Pick};
// int state = checkState.Zero;
int state, checkCurrentX = 0, checkCurrentY = 0, checkCurrentZ = 0, key = 0;
int  count = 0;

typedef struct ModeSystem
{
    unsigned int8 mode;
} Mode;

Mode modeSystem;

void FowardX()
{
    output_high(motorX1);
    output_low(motorX2);
}

void BackWardX()
{
    output_low(motorX1);
    output_high(motorX2);
}

void StopX()
{
    output_high(motorX1);
    output_high(motorX2);
}

void FowardY()
{
    output_high(motorY1);
    output_low(motorY2);
}

void BackWardY()
{
    output_low(motorY1);
    output_high(motorY2);
}

void FowardZ()
{
    output_high(motorZ1);
    output_low(motorZ2);
}

void BackWardZ()
{
    output_low(motorZ1);
    output_high(motorZ2);
}

void StopZ()
{
    output_high(motorZ1);
    output_high(motorZ2);
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

long EncoderX(long encountX)
{
    //41 cm
    long curX;
    curX = (encountX * 41.0) / 3685.0;
    return curX;
}

long EncoderY(long encountY)
{
    //41 cm
    long curY;
    curY = (encountY * 41.0) / 6400.0;
    return curY;
}

long EncoderZ(long encountZ)
{
    //25 cm
    long curZ;
    curZ = (encountZ * 11.0) / 9700.0;
    return curZ;
}

void PwmZero()
{
    if (state == set_zero)
    {
        if (input(sensorX) == 1)
        {
            FowardX();
            set_pwm_duty(1, 500);
        }
        if (input(sensorX) == 0)
        {
            set_pwm_duty(1, 0);
            StopX();
        }
        if (input(sensorY) == 1)
        {
            BackWardY();
            set_pwm_duty(2, 1000);
        }
        if (input(sensorY) == 0)
        {
            set_pwm_duty(2, 0);
        }
        if (input(sensorZ) == 1)
        {
            BackWardZ();
        }
        if (input(sensorZ) == 0)
        {
            StopZ();
        }
    }
}

void PwmMotorZ()
{
	if(currentZ <= 5)
    {
        FowardZ();
    }
    else 
        StopZ();
}

void PwmMotorX(float pwmX)
{
    float dutyX;

    if (state == move)
    {
        if ((int)pwmX > 100.0)
        {
            pwmX = 100.0;
        }
        else if ((int)pwmX < -100.0)
            pwmX = -100.0;

        dutyX = (int)pwmX * 20.0;
        // printf("%d\n", (int)pwm);
        BackWardX();
        set_pwm_duty(1, (int)dutyX);
    }
}

void PwmMotorY(float pwmY)
{
    float dutyY;
    if (state == move)
    {
        if ((int)pwmY > 100.0)
        {
            pwmY = 100.0;
        }
        else if ((int)pwmY < -100.0)
            pwmY = -100.0;

        dutyY = (int)pwmY * 20.0;
        // printf("%d\n", (int)pwm);
        // printf("end\n");
        FowardY();
        set_pwm_duty(2, (int)dutyY);
    }
}

#INT_EXT0
void INT_EXT_INPUT0(void)
{
    if (input(motorX_Encoder) == 0)
    {
        encountX--;
    }
    else
    {
        encountX++;
    }
    currentX = EncoderX(encountX);
}

#INT_EXT1
void INT_EXT_INPUT1(void)
{
    if (input(motorY_Encoder) == 0)
    {
        encountY--;
    }
    else
    {
        encountY++;
    }
    currentY = EncoderY(encountY);
}

#INT_EXT2
void INT_EXT_INPUT2(void)
{
    if (input(motorZ_Encoder) == 0)
    {
        encountZ--;
    }
    else
    {
        encountZ++;
    }
    currentZ = EncoderZ(encountZ);
}

#INT_TIMER1
void TIMER1_ist()
{

    if (checkCurrentX == 2)
    {
        if ((28 - currentX) < 0)
            setPointX = abs(28 - currentX);
        else
            setPointX = 28 - currentX;
        currentX = 0;
    }

    if (checkCurrentY == 1)
    {
        setPointY = currentY - 5;
        currentY = 0;
    }

    errorMotorX = setPointX - currentX;
    errorMotorY = setPointY - currentY;
    errorMotorZ = setPointZ - currentZ;

    sumErrorX = sumErrorX + errorMotorX;
    if (abs((int)errorMotorX) > 0.01)
    {
        pwmX = (int)Kp_X * errorMotorX + Ki_X * sumErrorX + Kd_X * (errorMotorX - pX);
    }
    else
    {
        pwmX = 0.0;
    }
    pX = errorMotorX;

    sumErrorY = sumErrorY + errorMotorY;
    if (abs((int)errorMotorY) > 0.01)
    {
        pwmY = (int)Kp_Y * errorMotorY + Ki_Y * sumErrorY + Kd_Y * (errorMotorY - pY);
    }
    else
    {
        pwmY = 0.0;
    }
    pY = errorMotorY;

    // printf("pwmY: %d\n", (int)pwmY);
    // printf("errorY: %d\n", (int)errorMotorY);
    // printf("setpointY: %d\n", (int)setPointY);
    // printf("encoderY: %d\n", (int)encountY);

    sumErrorZ = sumErrorZ + errorMotorZ;
    if (abs(errorMotorZ) > 3)
    {
        pwmZ = Kp_Z * errorMotorZ + Ki_Z * errorMotorZ + Kd_Z * (errorMotorZ - pZ);
    }
    else
    {
        pwmZ = 0;
    }
    pZ = errorMotorZ;
}

#INT_RDA
void UART1_Isr()
{
    unsigned int8 c = getc();
    SM_RxD(c);
    // key = getc();
}

void Init_Interrupts()
{
    enable_interrupts(INT_EXT0);
    ext_int_edge(0, L_TO_H);
    enable_interrupts(INT_EXT1);
    ext_int_edge(1, L_TO_H);
    enable_interrupts(INT_EXT2);
    ext_int_edge(2, L_TO_H);

    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 2000);
    setup_timer3(TMR_INTERNAL | TMR_DIV_BY_256, 1250);
    setup_timer4(TMR_INTERNAL | TMR_DIV_BY_256, 62500);
    set_timer4(0);
}

void Init_Timer1()
{
    setup_timer1(TMR_INTERNAL | TMR_DIV_BY_256, 625);
    enable_interrupts(INT_TIMER1);
}

void main()
{
    disable_interrupts(GLOBAL);
    Init_Timer1();
    set_tris_a(get_tris_a() & 0xffeb);
    setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(2, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(3, COMPARE_PWM | COMPARE_TIMER3);
    Init_Interrupts();
    clear_interrupt(INT_RDA);
    enable_interrupts(INT_RDA);
    enable_interrupts(GLOBAL);

    Kp_X = 10.0;
    Ki_X = 0.03;
    Kd_X = 5.0;

    Kp_Y = 50.0;
    Ki_Y = 1.0;
    Kd_Y = 30.0;

    Kp_Z = 1.0;
    Ki_Z = 0.0;
    Kd_Z = 0.0;

    pX = 0.0;
    pY = 0.0;
    pZ = 0.0;
    currentX = 0.0;
    encountX = 0.0;
    currentY = 0.0;
    encountY = 0.0;
    currentZ = 0.0;
    encountZ = 0.0;
    checkCurrentZ = 1;

    while (TRUE)
    {
        // FowardX();
        // BackWardY();
        // set_pwm_duty(2, 1000);
        // set_pwm_duty(1, 500);
        // set_pwm_duty(3, 105);
        // BackWardZ();
        // if(input(sensorZ) == 0)
        //     StopZ();

        // switch(key)
        // {
        //     case set_zero:
        //         encountZ = 0.0;
        //         currentZ = 0.0;
        //         StopZ();
        //         break;
        //     case move:
        //         FowardZ();
        //         if(input(sensorZ) == 0)
        //             StopZ();
        //         break;
        //     case pick_up:
        //         BackWardZ();
        //         break;
        // }

        // int now = get_timer4();

        // if (now > 6250)
        // {
        //     printf("state: %d\n", state);
        //     printf("mode: %d\n", modeSystem.mode);
        //     printf("positionX: %d\n", positionX[1];
        //     printf("positionY: %d\n", positionY[1]);
        //     // printf("errorMotorX: %d\n", (int)errorMotorX);
        //     // printf("currentX: %d\n", (int)currentX);
        //     // printf("errorMotorX: %d\n", (int)errorMotorX);
        //     // printf("currentX: %d\n", (int)currentX);
        //     set_timer4(0);
        // }
        if (getPackage >= 1)
        {
            setPointX = positionX[1];
            setPointY = positionY[1];
            getPackage = 0;
        }

        switch (modeSystem.mode)
        {
        case set_zero:
            currentX = 0;
            currentY = 0;
            encountX = 0;
            encountY = 0;
            pwmX = 0;
            pwmY = 0;
            errorMotorX = 0;
            errorMotorY = 0;
            state = set_zero;
            PwmZero();
            break;

        case move:
            checkCurrentX = 0;
            checkCurrentY = 0;
            state = move;
            PwmMotorX(pwmX);
            PwmMotorY(pwmY);
            break;

        case pick_up:
            PwmMotorZ();
            break;

        default:
            setPointX = 0;
            setPointY = 0;
            setPointZ = 0;
            break;
        }

        // printf("sensorY: %d\n", input(sensorY));
        // printf("count: %d\n", count);

        delay_ms(50);
    }
}
