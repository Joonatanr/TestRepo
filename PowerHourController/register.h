#ifndef REGISTER_H

#include "typedefs.h"
#include <msp430g2553.h>

#define SPECIAL_TASK_INTERVAL 5u


extern volatile U8 pwm_test;
extern U8 number_of_minutes;
extern char sY[12];

typedef void (*timer_callback)(void);

typedef struct
{
	U8 min;
	U8 sec;
} timekeeper_struct;

typedef enum{
	display_idle,
	display_special_text,
	display_drinking_text,
	display_counting
}display_state;


void register_init (timer_callback timer10msec, timer_callback timer1sec);
void timer_init (void);
void ports_init (void);
void wait_msec (U16 m_secs);
void pwm_init(void);

void set_pwm_duty (U16 duty);

void PORT_BACKLIGHT(U8 b);

void set_si(U8 b);
void set_clk(U8 b);
void set_rs(U8 b);
void set_cs(U8 b);

U8 isBtnSet (void);
U8 isBtnStart(void);

/* Servo */
//extern volatile U16 pwm_set_value;
//extern volatile U8 servo_flag;
void servo_init(void);
void servo_cyclic10msec(void);
void servo_cyclic_1sec(timekeeper_struct t);

/* Display */
void display_init(void);
void display_cyclic_1sec(timekeeper_struct t);
void display_cyclic_10msec(void);
void display_set_state(U8 new_state);

//End of double inclusion protection.
#define REGISTER_H
#endif
