#include "register.h"

#define SERVO_MIN 1500
#define SERVO_MAX 1100


#define SERVO_INTERVAL_STRIKE 50u /* Strike length */
#define SERVO_INTERVAL_ACTIVATE   60u /* Interval of striking */

#define SERVO_INTERVAL_MULTIRING 100u
#define SERVO_INTERVAL_MULTIRING_BUF 50u

U16 servo_count = 0; /* Keep track of interval between up and down movement */
U16 servo_activate_cnt = 0u; /* Keep track of rings */
U16 servo_ring_cnt = 0u;
U16 multiring_buf_cnt = 0u;



void servo_init()
{
	set_pwm_duty(SERVO_MAX);
}

Private void ringBell(U8 cnt)
{
	//ring bell the amount of times.
	servo_ring_cnt = cnt;
}


void servo_cyclic10msec(void)
{

	/* Here begins hardware handling */

	if((servo_ring_cnt > 0u) && (servo_count == 0u) && (multiring_buf_cnt == 0u))
	{
		set_pwm_duty(SERVO_MIN);
		servo_count = SERVO_INTERVAL_STRIKE;
		servo_ring_cnt--;
	}

//Control servo cycle.

	if(multiring_buf_cnt > 0u)
	{
		multiring_buf_cnt--;
	}

	if(servo_count > 0)
	{
		servo_count--;
	}

	if(servo_count == 1)
	{
		set_pwm_duty(SERVO_MAX);
		multiring_buf_cnt = SERVO_INTERVAL_MULTIRING_BUF;
	}
}


void servo_cyclic_1sec(timekeeper_struct t)
{
	if(t.min == number_of_minutes)
	{
		//Game finished.
		ringBell(4u);
	}
	else if (t.min > number_of_minutes)
	{
		//Game is already finished. Should not ring afterwards.
		return;
	}
	else if((t.sec == 0u) && (t.min % SPECIAL_TASK_INTERVAL == 0u) &&( t.min > 0u))
	{
		ringBell(3u);
	}
	else if(t.sec == 0u)
	{
		servo_activate_cnt = 0;
		ringBell(1u);
	}
}


