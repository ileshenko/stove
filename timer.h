/*
 * timer.h
 *
 *  Created on: Feb 9, 2013
 *      Author: Igor
 */

#ifndef _TIMER_H_
#define _TIMER_H_

void timer_init(void);

static inline void sleep(void)
{
	_BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
}

void timer_reg_1ms(void (*cb)(void));
void timer_reg_20ms(void (*cb)(void));
void timer_reg_100ms(void (*cb)(void));
void timer_reg_250ms(void (*cb)(void));



#endif /* _TIMER_H_ */
