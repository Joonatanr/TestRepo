#include <msp430.h>
#include "typedefs.h"
#include "register.h"
#include "display.h"
#include "misc.h"

/* main.c */

/* Private typedefs */
/* Function prototypes */
Private void incMainTimer(void);
Private void timer_10msec(void);
Private void timer_1sec(void);

/* Private variables */
Private volatile U8 timer_flag;
Private volatile U8 timer_special_flag;

Private timekeeper_struct main_timer = {0u, 0u};

Private disp_config_struct disp_config =
{
		.delay_func = wait_msec,
		.port_clk = set_clk,
		.port_csb = set_cs,
		.port_rs = set_rs,
		.port_si = set_si
};


const char menu1[] = "START ->BEGIN";
const char menu2[] = "SET   ->set time";

const char welcome_message[] = "POWER HOUR !!!";
const char version_str[] = "Ver 1.0";

U8 number_of_minutes = 60u;


Private void init_routine(void)
{
	//disp_write_string("Power Hour !!!", 0u, 0u);
	//wait_msec(2000);

	disp_write_string(welcome_message, 0u, 0u);
	disp_write_string(version_str, 0u, 1u);
	wait_msec(2000);

	disp_write_string(menu1, 0u, 0u);
	disp_write_string(menu2, 0u, 1u);
}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    register_init(timer_10msec, timer_1sec);
    disp_set_conf(&disp_config);
    disp_init();

    servo_init();
    display_init();

    wait_msec(500);

    PORT_BACKLIGHT(1);

BEGIN:

    init_routine();
    while(!isBtnStart())
    {
    	/* Set button control */
    	U8 pressed;

    	if(isBtnSet())
    	{
    		pressed = 1u;
    	}
    	else if(pressed == 1u)
    	{
    		pressed = 0u;
    		number_of_minutes += 5u;
    		if(number_of_minutes >= 120u)
    		{
    			number_of_minutes = 5u;
    		}
    		long2string(number_of_minutes, sY);
    		disp_write_string(sY, 0u, 1u);
    	}
    }

    main_timer.min = main_timer.sec = 0u;
    display_set_state(display_counting);

    while(1)
    {
    	if(timer_flag == 1u)
    	{
    		timer_flag = 0u;
    		display_cyclic_1sec(main_timer);
    		servo_cyclic_1sec(main_timer);

    		if(main_timer.min >= number_of_minutes)
    		{
    			/* Game over */
    			break;
    		}
    	}

    }

    wait_msec(5000);

    // Goto in this particular case just makes things a lot easier.
    // Program has finished in any case.
    goto BEGIN;

	return 0;
}

Private void incMainTimer(void)
{
	main_timer.sec++;

	if(main_timer.sec >= 60u)
	{
		main_timer.sec = 0u;
		main_timer.min++;

		if(main_timer.min > 0 && (main_timer.min % 5 == 0))
		{
			timer_special_flag = 1u;
		}
	}
}

Private void timer_10msec(void)
{
	display_cyclic_10msec();
	servo_cyclic10msec();
}


Private void timer_1sec(void)
{
	timer_flag = 1;
	//Increment main timer struct by 1 second.
	incMainTimer();
}
