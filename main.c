
#include <msp430g2452.h>
#include "display.h"
#include "timer.h"
#include "buttons.h"
#include "pwm.h"

typedef enum {
	PRG_HELLO = 0,
	PRG_BASIC,
	PRG_SET_TIMER1,
	PRG_SET_POWER2,
	PRG_SET_TIMER2,
	PRG_SHOW_STOP
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

static prg_t show_stop_prg(void)
{
	static const char *pattern[] = {"00", "--"};
	unsigned char idx = 0;

	while (buttons_get() == BTN_IDLE)
	{
		display(pattern[idx++&1]);
		sleep();
	}
	return PRG_BASIC;
}

static signed char d_h[2] = {0,0};
static signed char d_m[2] = {0,0};
static int d_s = 0;
static unsigned char power[2] = {0,0};

static void next_timer(void)
{
	d_h[0] = d_h[1];
	d_m[0] = d_m[1];
	if (d_m[0] || d_h[0])
		power[0] = power[1];
	else
		power[0] = 0;

	d_h[1] = d_m[1] = power[1] = 0;

	pwm_set(power[0]);
}

static void power_inc(char idx)
{
	if (++power[idx] > 10)
		power[idx] = 0;

	if (!idx)
		pwm_set(power[0]);
}

static void power_dec(char idx)
{
	if (--power[idx] > 10)
		power[idx] = 10;

	if (!idx)
		pwm_set(power[0]);
}

#define MINU d_m[0]
#define HOUR d_h[0]

static prg_t basic_prg(void)
{
	char cnt = 0;

	for (;;)
	{
		if (HOUR || MINU)
		{
			if (!--d_s)
			{
				if (MINU)
					MINU--;
				else if (HOUR)
				{
					HOUR--;
					MINU = 59;
				}

				if (!MINU && !HOUR)
					next_timer();

				if (!power[0])
					return PRG_SHOW_STOP;

				d_s = 60*4;
			}
		}
		if ((MINU || HOUR) && cnt++ & 0x4)
			display_time(HOUR, MINU);
		else
			display_int(power[0]);

		switch (buttons_get())
		{
		case BTN_UP:
		case BTN_UP_LONG:
			power_inc(0);
			cnt = 0;
			break;

		case BTN_DOWN:
		case BTN_DOWN_LONG:
			power_dec(0);
			cnt = 0;
			break;

		case BTN_BOTH:
			return PRG_SET_TIMER1;

		case BTN_IDLE:
		case BTN_UNDEFINED:
		case BTN_RESET:
			break;
		}
		sleep();
	}
}

#define TIMEOUT 20 /* 5 s */

static prg_t set_power2_prg(void)
{
	int timeout = TIMEOUT;

	while (timeout)
	{
		if (timeout&1)
			display("  ");
		else
			display_int(power[1]);
		sleep();

		switch (buttons_get())
		{
		case BTN_UP:
		case BTN_UP_LONG:

			timeout = TIMEOUT;
			power_inc(1);
			break;

		case BTN_DOWN:
		case BTN_DOWN_LONG:
			timeout = TIMEOUT;
			power_dec(1);
			break;

		case BTN_BOTH:
			goto Exit;

		case BTN_IDLE:
		case BTN_UNDEFINED:
		case BTN_RESET:
			timeout--;
			break;
		}
	}

Exit:
	if (power[1])
		return PRG_SET_TIMER2;

	return PRG_BASIC;
}

static void timer_up(signed char *h, signed char *m)
{
	signed char delta;
	d_s = 60*4;

	delta = *h ? 10 : 5;
	*m /= delta;
	*m *= delta; // floor minutes
	*m += delta;

	if (*m >=60)
	{
		if (*h < 9)
		{
			++*h;
			*m = 0;
		}
		else
			*m = 50;
	}
}

static void timer_down(signed char *h, signed char *m)
{
	signed char delta;
	d_s = 60*4;

	delta = *h ? 10 : 5;
	*m /= delta;
	*m *= delta; // floor minutes
	*m -= delta;

	if (*m < 0)
	{
		if (*h)
		{
			--*h;
			*m = 60;
			*m -= *h ? 10 : 5;
		}
		else
			*m = 0;
	}
}

static prg_t set_timer(char idx)
{
	int timeout = TIMEOUT;

	while (timeout)
	{
		if (idx && timeout&1)
			display("  ");
		else
			display_time(d_h[idx], d_m[idx]);
		sleep();

		switch (buttons_get())
		{
		case BTN_UP:
		case BTN_UP_LONG:

			timeout = TIMEOUT;
			timer_up(&d_h[idx], &d_m[idx]);
			break;

		case BTN_DOWN:
		case BTN_DOWN_LONG:
			timeout = TIMEOUT;
			timer_down(&d_h[idx], &d_m[idx]);
			break;

		case BTN_BOTH:
			if (idx)
				return PRG_BASIC;

			if (d_h[0] || d_m[0])
				return PRG_SET_POWER2;
			else
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

static prg_t set_timer1_prg(void)
{
	return set_timer(0);
}

static prg_t set_timer2_prg(void)
{
	return set_timer(1);
}

prg_t(*programs[])(void) = {
		hello_world_prg,
		basic_prg,
		set_timer1_prg,
		set_power2_prg,
		set_timer2_prg,
		show_stop_prg,
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


