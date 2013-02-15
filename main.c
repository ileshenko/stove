
#include <msp430g2452.h>
#include "display.h"
#include "timer.h"
#include "buttons.h"
#include "pwm.h"

typedef enum {
	PRG_HELLO = 0,
	PRG_BASIC,
	PRG_SET_TIMER
} prg_t;

static prg_t hello_world_prg(void)
{
	static const char *pattern[] = {"0 ", " 0"};
	unsigned char idx = 0;

	while (buttons_get() == BTN_IDLE)
	{
		display(pattern[(idx++&0x2)>>1]);
		sleep();
	}
	return PRG_BASIC;
}


static signed char d_h = 0;
static signed char d_m = 0;
static int d_s = 0;
static unsigned char power = 0;

static prg_t basic_prg(void)
{
	for (;;)
	{
		if (d_m)
		{
			if (!--d_s)
			{
				if (!--d_m)
				{
					power = 0;
					pwm_set(power);
				}
				else
					d_s = 60*4;
			}
		}
		if (d_m && d_s & 0x4)
			display_time(d_h, d_m);
		else
			display_int(power);

		switch (buttons_get())
		{
		case BTN_UP:
		case BTN_UP_LONG:
			if (++power > 10)
				power = 0;
			pwm_set(power);
			break;

		case BTN_DOWN:
		case BTN_DOWN_LONG:
			if (--power > 10)
				power = 10;
			pwm_set(power);
			break;

		case BTN_BOTH:
			return PRG_SET_TIMER;

		case BTN_IDLE:
		case BTN_UNDEFINED:
		case BTN_RESET:
			break;
		}

		sleep();
	}
}

#define TIMEOUT 20 /* 5 s */
static prg_t set_timer_prg(void)
{
	int timeout;

	timeout = TIMEOUT;

	while (timeout)
	{
		display_time(d_h, d_m);
		sleep();

		switch (buttons_get())
		{
		case BTN_UP:
		case BTN_UP_LONG:

			timeout = TIMEOUT;
			d_s = 60*4;

			d_m += 5;
			if (d_m > 99)
				d_m = 99;
			break;

		case BTN_DOWN:
		case BTN_DOWN_LONG:
			timeout = TIMEOUT;
			d_s = 60*4;

			d_m -= 5;
			if (d_m < 0)
				d_m = 0;
			break;

		case BTN_BOTH:
			return PRG_BASIC;

		case BTN_IDLE:
		case BTN_UNDEFINED:
		case BTN_RESET:
			timeout--;
			break;
		}
	}
	return PRG_BASIC;
}

prg_t(*programs[])(void) = {
		hello_world_prg,
		basic_prg,
		set_timer_prg,
};

void main(void)
{
	prg_t prg = PRG_HELLO;

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	timer_init();
	display_init();
	buttons_init();
	pwm_init();

	for (;;)
		prg = programs[prg]();
}


