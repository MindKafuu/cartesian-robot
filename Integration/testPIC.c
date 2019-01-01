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
#PIN_SELECT OC4 = servo_down
#PIN_SELECT OC5 = servo_up
#PIN_SELECT INT1 = motorY_Encoder
#PIN_SELECT INT2 = motorZ_Encoder

long encountX = 0, encountY = 0, encountZ = 0;
int directionX = 0, directionY = 0, directionZ = 0;

unsigned int8 positionX[2] = {};
unsigned int8 positionY[2] = {};
// unsigned int8 data[2] = {};
unsigned int8 check[2] = {};
int check_data = 1, checksum = 0;
unsigned int8 getPackage = 0, theta = 0;

long setpointX = 0, setpointY = 0;
int count1 = 1, count2 = 1, count3 = 1;
int offset = 0, offset1 = 0, offset3 = 0;

typedef struct ModeSystem
{
    unsigned int8 mode;
    unsigned int8 type;
    unsigned int8 setType;
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
    else if(check_data <= 4)
    {
        modeSystem.type = c;
        check_data++;
    }
    else if(check_data <= 5)
    {
        modeSystem.setType = c;
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
        checksum = (((int)positionX[0] + (int)positionX[1] + (int)positionY[0] + (int)positionY[1]) - ((int)theta + (int)modeSystem.mode + (int)modeSystem.type + (int)modeSystem.setType)) + 1;
        if (checksum < 0)
            checksum = checksum * -1;
        checksum_1 = (checksum >> 8) & 0xFF;
        checksum_2 = checksum & 0xFF;
        check_data++;

        if (checksum_1 == check[0] && checksum_2 == check[1])
        {
            check_data++;
            if (check_data > 13)
            {
                getPackage = 1;
                check_data = 1;
            }
        }
    }
}

void servo(int degree)
{

	float time_servo = (8.7 * degree) + 700; // y = mx + c
	int pwm_servo = time_servo/16;

	// float time_servo2 = (7.41 * set_degree) + 500; 
	// int pwm_servo2 = time_servo2/16;

	set_pwm_duty(5, pwm_servo);
	// set_pwm_duty(servo2, pwm_servo2); 
}

void SetX(int setX)
{
    int check = 0;
    while (check == 0)
    {
        int errX = setX - encountX;
        int sumX = 0;
        int pX = 0;
        sumX = sumX + errX;
        int dutyX = errX * 10 + 1 * sumX + 5 * (errX - pX);
        pX = errX;

        if (dutyX > 2000)
        {
            dutyX = 2000;
        }
        if (dutyX < -2000)
        {
            dutyX = -2000;
        }

        if (errX > 0)
        {
            FowardX();
            set_pwm_duty(1, dutyX);
        }
        else if (errX < 0)
        {
            BackWardX();
            set_pwm_duty(1, -dutyX);
        }
        else if (errX == 0)
        {
            StopX();
            set_pwm_duty(1, 2000);
            delay_ms(200);
        }

        if (abs(errX) < 20)
        {
            StopX();
            set_pwm_duty(1, 2000);
            delay_ms(200);
            check = 1;
        }
    }
}

void SetY(int setY)
{
    int check = 0;
    while (check == 0)
    {
        int errY = setY - encountY;
        int dutyY = errY * 10;
        if (dutyY > 2000)
        {
            dutyY = 2000;
        }
        if (dutyY < -2000)
        {
            dutyY = -2000;
        }

        if (errY > 0)
        {
            FowardY();
            set_pwm_duty(2, dutyY);
        }
        else if (errY < 0)
        {
            BackWardY();
            set_pwm_duty(2, -dutyY);
        }
        else if (errY == 0)
        {
            StopY();
            set_pwm_duty(2, 2000);
            delay_ms(200);
        }
        if (abs(errY) < 20)
        {
            StopY();
            set_pwm_duty(2, 2000);
            delay_ms(200);
            check = 1;
        }
    }
}

void SetZ(int setZ)
{
    int check = 0;
    while (check == 0)
    {
        int errZ = setZ - encountZ;
        int dutyZ = errZ * 40;
        if (dutyZ > 2000)
        {
            dutyZ = 2000;
        }
        if (dutyZ < -2000)
        {
            dutyZ = -2000;
        }

        if (errZ > 0)
        {
            FowardZ();
            set_pwm_duty(3, dutyZ);
        }
        else if (errZ < 0)
        {
            BackWardZ();
            set_pwm_duty(3, -dutyZ);
        }
        else if (errZ == 0)
        {
            StopZ();
            set_pwm_duty(3, 2000);
            delay_ms(200);
        }

        if (abs(errZ) < 45)
        {
            StopZ();
            set_pwm_duty(3, 2000);
            delay_ms(200);
            check = 1;
        }
    }
}

void PwmZero()
{
    int check = 0;
    int state = 0;
    while (check == 0)
    {
        if (state == 0)
        {
            if (input(sensorX) == 1)
            {
                BackWardX();
                set_pwm_duty(1, 700);
            }
            else if (input(sensorX) == 0)
            {
                StopX();
                set_pwm_duty(1, 2000);
                delay_ms(200);
                encountX = 0;
                state = 1;
            }
        }
        else if (state == 1)
        {
            if (input(sensorY) == 1)
            {
                BackWardY();
                set_pwm_duty(2, 1000);
            }
            else if (input(sensorY) == 0)
            {
                StopY();
                set_pwm_duty(2, 2000);
                delay_ms(200);
                encountY = 0;
                state = 2;
            }
        }
        else if(state == 2)
        {
            if (input(sensorZ) == 0)
            {
                BackWardZ();
                set_pwm_duty(3, 700);
            }
            else if (input(sensorZ) == 1)
            {
                StopZ();
                set_pwm_duty(3, 2000);
                delay_ms(200);
                encountZ = 0;
                check = 1;
                state = 3;
            }
        }
    }
    delay_ms(1000);
    setZ(500);
    servo(90);
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
}

void Pattern1(int posX, int posY)
{
    if(count1 == 4)
        offset1 = 0;

    if(count1 <= 3)
    {
        setX(abs(setpointX));
        delay_ms(500);
        setY(abs(setpointY));
        servo(theta);
        delay_ms(500);
        setZ(6000);
        delay_ms(500);
        set_pwm_duty(4, 140);
        delay_ms(10000);
        set_pwm_duty(4, 92);
        delay_ms(500);
        setZ(3000);
        delay_ms(500);
        setX(3400);
        delay_ms(500);
        setY(0);
        delay_ms(500);
        servo(90);
        setZ(11000 - offset1);
        offset1 = offset1 + 1500;
        set_pwm_duty(4, 80);
        delay_ms(16000);
        set_pwm_duty(4, 92);
        setZ(500);
        delay_ms(1000);
        PwmZero();
    }
    else
    {
        setX(abs(setpointX));
        delay_ms(500);
        setY(abs(setpointY));
        servo(theta);
        delay_ms(500);
        setZ(6000);
        delay_ms(500);
        set_pwm_duty(4, 140);
        delay_ms(10000);
        set_pwm_duty(4, 92);
        delay_ms(500);
        setZ(3000);
        delay_ms(500);
        setX(3400);
        delay_ms(500);
        setY(2200);
        delay_ms(500);
        servo(90);
        setZ(11000 - offset1);
        offset1 = offset1 + 1500;
        set_pwm_duty(4, 80);
        delay_ms(16000);
        set_pwm_duty(4, 92);
        setZ(500);
        delay_ms(500);
        PwmZero();
    }
    count1++;
}

void Pattern2(int posX, int posY)
{
    setX(abs(setpointX));
    delay_ms(500);
    setY(abs(setpointY));
    servo(theta);
    delay_ms(500);
    setZ(6000);
    delay_ms(500);
    set_pwm_duty(4, 140);
    delay_ms(10000);
    set_pwm_duty(4, 92);
    delay_ms(500);
    setZ(3000);
    delay_ms(500);
    if(count2 == 1)
    {
        setX(2500);
        delay_ms(500);
        setY(0);
        delay_ms(500);
        servo(0);
    }
    else if(count2 == 2)
    {
        setX(3400);
        delay_ms(500);
        setY(0);
        delay_ms(500);
        servo(90);
    }
    else if(count2 == 3)
    {
        setX(3500);
        delay_ms(500);
        setY(2200);
        delay_ms(500);
        servo(0);
    }
    else if(count2 == 4)
    {
        setX(2600);
        delay_ms(500);
        setY(2100);
        delay_ms(500);
        servo(90);
    }
    setZ(11000);
    set_pwm_duty(4, 80);
    delay_ms(16000);
    set_pwm_duty(4, 92);
    setZ(500);
    delay_ms(500);
    PwmZero();
    count2++;
}

void Pattern3(int posX, int posY)
{
    if(count3 == 4)
        offset3 = 0;

    if(count3 <= 3)
    {
        setX(abs(setpointX));
        delay_ms(500);
        setY(abs(setpointY));
        servo(theta);
        delay_ms(500);
        setZ(6000);
        delay_ms(500);
        set_pwm_duty(4, 140);
        delay_ms(10000);
        set_pwm_duty(4, 92);
        delay_ms(500);
        setZ(3000);
        delay_ms(500);
        setX(3400 - offset3);
        offset3 = offset3 + 270;
        delay_ms(500);
        setY(0);
        delay_ms(500);
        servo(90);
        setZ(11000);
        set_pwm_duty(4, 80);
        delay_ms(16000);
        set_pwm_duty(4, 92);
        setZ(500);
        delay_ms(500);
        PwmZero();
    }
    else
    {
        setX(abs(setpointX));
        delay_ms(500);
        setY(abs(setpointY));
        servo(theta);
        delay_ms(500);
        setZ(6000);
        delay_ms(500);
        set_pwm_duty(4, 140);
        delay_ms(10000);
        set_pwm_duty(4, 92);
        delay_ms(500);
        setZ(3000);
        delay_ms(500);
        setX(3400 - offset3);
        offset3 = offset3 + 270;
        delay_ms(500);
        setY(2200);
        delay_ms(500);
        servo(90);
        setZ(11000);
        set_pwm_duty(4, 80);
        delay_ms(16000);
        set_pwm_duty(4, 92);
        setZ(500);
        delay_ms(500);
        PwmZero();
    }
    count3++;
}

void Difference(int posX, int posY)
{
    setX(abs(setpointX));
    delay_ms(500);
    setY(abs(setpointY));
    servo(theta);
    delay_ms(500);
    setZ(6000);
    delay_ms(500);
    set_pwm_duty(4, 140);
    delay_ms(10000);
    set_pwm_duty(4, 92);
    delay_ms(500);
    setZ(3000);
    delay_ms(500);
    setX(500);
    delay_ms(500);
    setY(0 + offset);
    offset = offset + 1000;
    delay_ms(500);
    servo(0);
    setZ(11000);
    set_pwm_duty(4, 80);
    delay_ms(16000);
    set_pwm_duty(4, 92);
    setZ(500);
    delay_ms(500);
    PwmZero();
}

int main()
{
    disable_interrupts(GLOBAL);
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 2000);
    setup_timer3(TMR_INTERNAL | TMR_DIV_BY_256, 1250);
    //set_tris_a(get_tris_a() & 0xffeb);
    setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(2, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(3, COMPARE_PWM | COMPARE_TIMER2);
    setup_compare(4, COMPARE_PWM | COMPARE_TIMER3);
    setup_compare(5, COMPARE_PWM | COMPARE_TIMER3);

    Init_Interrupts();
    enable_interrupts(GLOBAL);

    set_pwm_duty(1, 0);
    set_pwm_duty(2, 0);
    set_pwm_duty(3, 0);
    set_pwm_duty(4, 92);
    set_pwm_duty(5, 0);

    delay_ms(1000);
    PwmZero();
    // set_pwm_duty(4, 140);   //หุบเข้า
    // set_pwm_duty(4, 80); //คายออก
    // delay_ms(5000);
    // set_pwm_duty(4, 92);

    delay_ms(500);
    setX(1500);
    // delay_ms(500);
    // setX(500);
    // servo(0);

    while (TRUE)
    {   
        //max X = 3500
        //max Y = 6000
        //max Z = 11000
        //คีบถุง 6000
        //servo ตัวล่าง น้อยกว่า 92 หมุนทวนเข็ม มากกว่า หมุนตามเข็ม
        //servo 140 ก็พอ
        if(getPackage >= 1)
        {
            setpointX = ((long)positionX[1] - 3) * 1500 / 13;
            setpointY = ((long)positionY[1] - 4) * 1500 / 7;

            if(setpointX > 3500)
                setpointX = 3500;
            if(setpointY > 6000)
                setpointY = 6000;

            switch (modeSystem.mode)
            {
                case 1:
                    if(modeSystem.type == modeSystem.setType)
                        Pattern1(setpointX, setpointY);
                    else
                        Difference(setpointX, setpointY);
                    break;
                case 2:
                    if(modeSystem.type == modeSystem.setType)
                        Pattern2(setpointX, setpointY);
                    else
                        Difference(setpointX, setpointY);
                    break;
                case 3:
                    if(modeSystem.type == modeSystem.setType)
                        Pattern3(setpointX, setpointY);
                    else
                        Difference(setpointX, setpointY);
                    break;
            }
            
            printf("mode: %d\n", modeSystem.mode);
            printf("end\n");
            getPackage = 0;
        }

        // printf("sensorX: %d\n", input(sensorX));
        // printf("sensorZ: %d\n", input(sensorZ));

        // printf("X: %d\n", (int)errX);
        // printf("Y: %d\n", (int)encountY);
        // printf("Z: %d\n", (int)encountZ);
        // delay_ms(50);
    }
    return 0;
}