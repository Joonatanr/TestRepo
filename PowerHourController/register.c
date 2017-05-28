/*
 * register.c
 *
 *  Created on: 05.07.2016
 *      Author: Joonatan
 */

#include "register.h"
#include <msp430.h>


#define MCU_CLOCK			1000000
#define PWM_FREQUENCY		50		// In Hertz, ideally 50Hz.

//Maybe still needs to check if this works?
//#define PWM_PERIOD              20000
#define PWM_PERIOD  (U32)((U32)MCU_CLOCK / (U32)PWM_FREQUENCY)
#define PWM_INITIAL_DUTY		1100


#define DIR_IN 	(U8)0u
#define DIR_OUT (U8)1u

typedef enum
{
	PORT_1,
	PORT_2,
	PORT_3
}port_enum;

typedef enum
{
	IO_PORT_CS,
	IO_PORT_RS,
	IO_PORT_SI,
	IO_PORT_CLK,
	IO_PORT_BACKLIGHT,
	IO_PORT_PWM,
	IO_PORT_BTN_SET,
	IO_PORT_BTN_START,
	NUMBER_OF_DEFINED_IOS
}io_port_enum;

typedef struct{
	io_port_enum io_name;
	port_enum port_num;

	U8 bit_mask;
	U8 dir;
}ioport_struct;

ioport_struct ports[NUMBER_OF_DEFINED_IOS] =
{
		{IO_PORT_CS, 		PORT_2, BIT_5, DIR_OUT	},
		{IO_PORT_RS,  		PORT_1, BIT_6, DIR_OUT  },
		{IO_PORT_SI,  		PORT_2, BIT_3, DIR_OUT  },
		{IO_PORT_CLK, 		PORT_2, BIT_4, DIR_OUT  },
		{IO_PORT_BACKLIGHT, PORT_1, BIT_0, DIR_OUT	},
		{IO_PORT_PWM,       PORT_2, BIT_1, DIR_OUT  },
		{IO_PORT_BTN_START, PORT_1, BIT_5, DIR_IN   },
		{IO_PORT_BTN_SET,   PORT_2, BIT_0, DIR_IN   }
};


static U16 count_1sec = 0;

static timer_callback callback10msec;
static timer_callback callback1sec;

U8 bReady = 0;

void register_init(timer_callback timer10msec, timer_callback timer1sec)
{
    /* Disable watchdog timer */
	WDTCTL = WDTPW + WDTHOLD;

	/* Set up internal clock */
    /* Check if 8MHz Calibration is present */
    if (CALBC1_8MHZ != 0xFF)
    {
        DCOCTL = 0; // Select lowest DCOx and MODx
        BCSCTL1 = CALBC1_8MHZ; // Set range
        DCOCTL = CALDCO_8MHZ; // Set DCO step + modulation
    }

	/* Set port directions. */
	ports_init ();

	/* Set internal resistor for buttons */

	SETBIT(P1REN, BIT_5);
	SETBIT(P2REN, BIT_0);

    /* Initialize main timer. */
	/*Set callback function pointers. */
	callback10msec = timer10msec;
	callback1sec = timer1sec;

    //Initialize timekeeping timer (timer A0).
	timer_init();

	/*Initialise timer for PWM for servo. */
	pwm_init();


    bReady = 1;
    /* Enable interrupts */
    _enable_interrupt();
}


void ports_init (void)
{
	U8 x;
	P1DIR = 0x00u;
	P2DIR = 0x00u;
	P3DIR = 0x00u;

	P1OUT = 0x00u;
	P2OUT = 0x00u;
	P3OUT = 0x00u;

	for (x = 0u; x < NUMBER_OF_DEFINED_IOS; x++)
	{
		switch (ports[x].port_num)
		{
			case (PORT_1):
					if (ports[x].dir){
						SETBIT (P1DIR,ports[x].bit_mask);
					}else{
						CLRBIT (P1DIR,ports[x].bit_mask);
					}
				break;
			case (PORT_2):
					if (ports[x].dir){
						SETBIT (P2DIR,ports[x].bit_mask);
					}else{
						CLRBIT (P2DIR,ports[x].bit_mask);
					}
				break;
			case (PORT_3):
				if (ports[x].dir){
						SETBIT (P3DIR,ports[x].bit_mask);
				}else{
						CLRBIT (P3DIR,ports[x].bit_mask);
				}
				break;
			default:
				break;
		}
	}
}


void timer_init (void)
{
	TA0CCTL0 = CM_0 + CCIE;	//Enable Timer A0 interrupts, bit 4=1
	TA0CCR0 = 2000;		//1000 Hz
	TA0CTL = TASSEL_2 + MC_1 + ID_2;        // Timer A0 with ACLK,
}

static U8 count_10msec = 0;

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
	if (++count_10msec < 10u)
	{
		return;
	}

	count_10msec = 0u;
	callback10msec();

	if (++count_1sec < 100u)
	{
		return;
	}

	callback1sec();
	count_1sec = 0;
}

void wait_msec (U16 ms)
{
	U16 t;
	for (t = ms; t > 0; t--)
	{
		/* For 8MHz calibration */
		__delay_cycles (8000);
	}
}


void pwm_init (void)
{
	// Setup the PWM, etc.
	WDTCTL	= WDTPW + WDTHOLD;     // Kill watchdog timer
	TA1CCTL1= OUTMOD_7;            // TACCR1 reset/set
	TA1CTL	= TASSEL_2 + MC_1 + ID_3;     // SMCLK, upmode
	TA1CCR0	= PWM_PERIOD - 1;        // PWM Period
	TA1CCR1	= PWM_INITIAL_DUTY;            // TACCR1 PWM Duty Cycle

	P2DIR |= BIT1;
	P2SEL |= BIT1;
}

void set_pwm_duty (U16 duty)
{
	//Sets duty value directly for testing.
	TA1CCR1 = duty;
}

void PORT_BACKLIGHT	(U8 b){if (b){SETBIT (P1OUT, BIT_0);}else{CLRBIT (P1OUT, BIT_0);}}

void set_si(U8 b)	{if(b){SETBIT(P2OUT, BIT3);}else{CLRBIT(P2OUT, BIT3);}}
void set_clk(U8 b)	{if(b){SETBIT(P2OUT, BIT4);}else{CLRBIT(P2OUT, BIT4);}}
void set_rs(U8 b)	{if(b){SETBIT(P1OUT, BIT6);}else{CLRBIT(P1OUT, BIT6);}}
void set_cs(U8 b)	{if(b){SETBIT(P2OUT, BIT5);}else{CLRBIT(P2OUT, BIT5);}}

U8 isBtnSet 	(void){if (ISBIT(P1IN, BIT_5)){return 0u;} else {return 1u;}}
U8 isBtnStart 	(void){if (ISBIT(P2IN, BIT_0)){return 0u;} else {return 1u;}}


