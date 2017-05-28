#include "register.h"
#include "display.h"
#include "time.h"
#include "stdlib.h"


#define FLASH_CYCLE 50



typedef struct
{
	const char * upper_text;
	const char * lower_text;
} MultiString;


Private const MultiString SpecialTaskArray[] =
{
//		  0123456789012345       0123456789012345
		{"Mehed joovad 2x ", 	" "					},
		{"Naised joovad 2x", 	" "					},
		{"K6ik joovad 3x  ", 	" "					},
		{"Neiu suurimate  ",    "rindadega joob2x"	},
		{"Mees suurima    ",    "schlangiga 3x"		},
		{"Toosti ytleja   ",    "r22gib anekdoodi" 	},
		{"Toosti ytleja   ",    "joob 2x"           },
		{"WATERFALL"       ,    " "                 },
		{"Ainult mehed "   ,    "joovad"            },
		{"Ainule naised"   ,    "joovad"            },
		{"Toosti ytlejal"  ,    "-1 riideese     "  },
		{"Eriline toost",       "ainult naistele"   },
		{"Naised joovad 3x",    ""                  },
		{"Keegi ei tohi   ",    "naerda 60 sec  "   },
		{"Toosti ytleja   ",    "I have never"      },
//		  0123456789012345       0123456789012345
		{"K6ige ilusamini ",    "riides inimene2x"  },
		{"Tugevaim mees   ",    "joob 3x"           },
		{"Viinashoti ring ",    " "                 },
		{"Juuakse ilma"    ,    "k2si kasutamata"   },
		{"Toosti ytleja"   ,    "r22gib nilbe loo"  },
		{"K6ige seksikama ",    "h22lega neiu 2x "  },
		{"K6ige sekskam   ",    "mees joob 4x"      },
};


#define NUMBER_OF_SPECIAL_TASKS (sizeof(SpecialTaskArray) / sizeof(MultiString))
#define NUMBER_OF_SPECIAL_TOASTS (sizeof(toastArray) / sizeof(const char *))


Private const char * toastArray[] =
{
		{"Proosit!"},{ "V6tame"}, {"Terviseks!"}, {"TerviSEKS"}, {"Joogem!"}, {"P6hjani"}, {"Pitsid tyhjaks!"}, {"Joome"}, {"TOOST!"}
};

Private const char EndText[] = "GAME OVER!";

//#define RAND_MAX (NUMBER_OF_SPECIAL_TASKS - 1)

Private display_state my_state;

Private void convertTimerString(timekeeper_struct * t, char * dest_str);
Private void displaySpecialTask(void);
Private void displayToast(void);
Private void start_flash(U8 cycles);

Private volatile U8 flash_counter = 0u;
Private volatile U8 flash_timer = 0u;
Private U8 led_state = 1u;


char sY[12];

void display_init(void)
{
	srand(time(NULL));
	//my_state = display_counting;

	my_state = display_idle;
}

void display_cyclic_1sec(timekeeper_struct t)
{

	/*check if finished*/
	if(t.min >= number_of_minutes)
	{
		display_set_state(display_idle);
		disp_clear_high();
		disp_write_string(EndText, 0u, 1u);

	}

	if((t.sec == 0u) && (t.min % SPECIAL_TASK_INTERVAL == 0u) && (t.min > 0u))
	{
		display_set_state(display_special_text);
		displaySpecialTask();
		start_flash(6u);
	}

	switch(my_state)
	{
		case display_drinking_text:
		case display_counting:
			if(t.sec == 40u)
			{
				display_set_state(display_drinking_text);
				/*todo : replace with random array member. */
				disp_write_string("Toost valmis!", 0u, 0u);
			}

			if(t.sec == 50u)
			{
				display_set_state(display_counting);
			}

			if(t.sec == 0u)
			{
				/* TODO : Replace with random array member */
				//disp_write_string("PROOSIT!", 0u, 0u)
				displayToast();
				display_set_state(display_drinking_text);
			}

			if(t.sec == 5u)
			{
				display_set_state(display_counting);
			}

			//Update time on lower part of display.
			convertTimerString(&t, sY);
			disp_write_string(sY, 0u, 1u);
		break;

		case display_special_text:

			if(t.sec >= 10u)
			{
				display_set_state(display_counting);
			}

			break;
		default:
			break;
	}
}

void display_cyclic_10msec(void)
{
	if(flash_timer > 0u)
	{
		flash_timer--;
		if(flash_timer == 0u)
		{
			flash_counter--;
			led_state = !led_state;

			if(flash_counter > 0u)
			{
				flash_timer = FLASH_CYCLE;
			}
			else
			{
				led_state = 1u;
			}
		}
	}
	PORT_BACKLIGHT(led_state);
}

void display_set_state(U8 new_state)
{
	U8 clear_hi = 0u;
	U8 clear_lo = 0u;

	switch(my_state)
	{
		case display_counting:
		case display_drinking_text:
			if(new_state == display_counting)
			{
				clear_hi = 1;
			}
			else if(new_state == display_special_text)
			{
				clear_hi = clear_lo = 1;
			}
			break;
		case display_idle:
			clear_hi = clear_lo = 1;
			break;
		case display_special_text:
			clear_hi = clear_lo = 1;
			break;

	}
	if(clear_hi)
	{
		disp_clear_high();
	}

	if(clear_lo)
	{
		disp_clear_low();
	}

	my_state = (display_state)new_state;
}


Private void displaySpecialTask(void)
{
	U8 index;

	index = rand() % NUMBER_OF_SPECIAL_TASKS;    //returns a pseudo-random integer between 0 and RAND_MAX

	disp_write_string(SpecialTaskArray[index].upper_text, 0u, DISP_HIGH);
	disp_write_string(SpecialTaskArray[index].lower_text, 0u, DISP_LOW);
}

Private void displayToast(void)
{
	U8 index;
	index = rand() % NUMBER_OF_SPECIAL_TOASTS;
	disp_write_string(toastArray[index], 0u, DISP_HIGH);
}


Private void convertTimerString(timekeeper_struct * t, char * dest_str)
{
	dest_str[0] = '0' + (t->min / 10u);
	dest_str[1] = '0' + (t->min % 10u);
	dest_str[2] = ':';
	dest_str[3] = '0' + (t->sec / 10u);
	dest_str[4] = '0' + (t->sec % 10u);
	dest_str[5] = 0;
}

Private void start_flash(U8 cycles)
{
	flash_counter = cycles;
	flash_timer = FLASH_CYCLE;
}
