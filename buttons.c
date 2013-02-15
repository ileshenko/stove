
#include <msp430g2452.h>
#include "buttons.h"
#include "timer.h"

#define PIN_UP BIT6
#define PIN_DOWN BIT7

static unsigned char up_cnt = 0, down_cnt = 0;
unsigned char up_is = 0, down_is = 0;
button_state_t btn_state;

static void buttons_pool(void)
{
	unsigned char is_up, is_down;

	is_up = !(P2IN & PIN_UP);
	is_down = !(P2IN & PIN_DOWN);

	switch (btn_state)
	{
	case BTN_IDLE:
		if (is_up && is_down)
			btn_state = BTN_BOTH;
		else if (is_up)
		{
			up_cnt++;
			btn_state = BTN_UNDEFINED;
		}
		else if (is_down)
		{
			down_cnt++;
			btn_state = BTN_UNDEFINED;
		}
		break;

	case BTN_UNDEFINED:
		if (is_up && is_down)
			btn_state = BTN_BOTH;
		else if (up_cnt)
		{
			if (is_up)
			{
				if (++up_cnt >= 10)
					btn_state = BTN_UP_LONG;
			}
			else
			{
				if (up_cnt >=2)
					btn_state = BTN_UP;
				else
					btn_state = BTN_RESET;
			}
		}
		else if (down_cnt)
		{
			if (is_down)
			{
				if (++down_cnt >= 10)
					btn_state = BTN_DOWN_LONG;
			}
			else
			{
				if (down_cnt >=2)
					btn_state = BTN_DOWN;
				else
					btn_state = BTN_RESET;
			}
		}
		break;

	case BTN_UP:
	case BTN_DOWN:
	case BTN_BOTH:
		break;
	case BTN_UP_LONG:
		if (!is_up)
			btn_state = BTN_RESET;
		break;

	case BTN_DOWN_LONG:
		if (!is_down)
			btn_state = BTN_RESET;
		break;

	case BTN_RESET:
		if (!is_up && !is_down)
		{
			up_cnt = down_cnt = 0;
			btn_state = BTN_IDLE;
		}
		break;

	}
}

button_state_t buttons_get(void)
{
	button_state_t ret = btn_state;

	switch (btn_state)
	{
	case BTN_UP:
	case BTN_DOWN:
	case BTN_BOTH:
		btn_state = BTN_RESET;
		break;
	default:
		break;
	}
	return ret;
}

void buttons_init(void)
{
	P2SEL &= ~(PIN_UP | PIN_DOWN);
	P2SEL2 &= ~(PIN_UP | PIN_DOWN);

	P2DIR &= ~(PIN_UP | PIN_DOWN);					// configure as IN

	P2REN |= PIN_UP | PIN_DOWN;						// Poll UP resistor
	P2OUT |= PIN_UP | PIN_DOWN;


//	P1IE |= BTN_TOGGLE;                             // interrupt enabled
//	P1IES |= BTN_TOGGLE;                            // Hi/lo edge
//	P1IFG &= ~BTN_TOGGLE;                           // IFG cleared
	btn_state = BTN_IDLE;
	timer_reg_100ms(buttons_pool);
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
//	sm_toggle();
//	P1OUT ^= 0x01;
//	P1IFG &= ~BTN_TOGGLE;
}
