/*
 * timer.c
 *
 *  Created on: Feb 9, 2013
 *      Author: Igor
 */
#include <msp430g2452.h>
#include "timer.h"

static void (*timer_1ms)(void) = 0;
static void (*timer_20ms)(void) = 0;
static void (*timer_100ms)(void) = 0;

void timer_reg_1ms(void (*cb)(void))
{
	timer_1ms = cb;
}

void timer_reg_20ms(void (*cb)(void))
{
	timer_20ms = cb;
}

void timer_reg_100ms(void (*cb)(void))
{
	timer_100ms = cb;
}

void timer_reg_200ms(void (*cb)(void))
{

}

void timer_init(void)
{
	CCTL0 = CCIE;                             // CCR0 interrupt enabled
	CCR0 = 50000;

	TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	static int t20_cnt = 3;
	static int t100_cnt = 5;
	static int t250_cnt = 7;

	CCR0 += 1000; // Add Offset to CCR0

	if (timer_1ms)
		timer_1ms();

	if (!--t20_cnt)
	{
		t20_cnt = 20;
		if (timer_20ms)
			timer_20ms();
	}

	if (!--t100_cnt)
	{
		t100_cnt = 100;
		if (timer_100ms)
			timer_100ms();
	}

	if (!--t250_cnt)
	{
		t250_cnt = 250;

		_BIC_SR_IRQ(LPM0_bits);					// wake up
	}
}
