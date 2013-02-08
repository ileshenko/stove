	//******************************************************************************
	//  MSP430G2xx2 Demo - Timer_A, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
	//
	//  Description: Toggle P1.0 using software and TA_0 ISR. Toggles every
	//  50000 SMCLK cycles. SMCLK provides clock source for TACLK.
	//  During the TA_0 ISR, P1.0 is toggled and 50000 clock cycles are added to
	//  CCR0. TA_0 ISR is triggered every 50000 cycles. CPU is normally off and
	//  used only during TA_ISR.
	//  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO
	//
	//           MSP430G2xx2
	//         ---------------
	//     /|\|            XIN|-
	//      | |               |
	//      --|RST        XOUT|-
	//        |               |
	//        |           P1.0|-->LED
	//
	//  D. Dang
	//  Texas Instruments Inc.
	//  December 2010
	//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
	//******************************************************************************

#include <msp430g2452.h>

static int mmm;

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	P1DIR |= 0xff;                            // P1.0 output
	P2DIR |= 0xff;                            // P1.0 output
	CCTL0 = CCIE;                             // CCR0 interrupt enabled
	CCR0 = 50000;
	mmm = 0;

	TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

	for (;;)
	{
#if 1
		P1OUT |= 1<<5;
		P2OUT &= ~(1<<0);

		P2OUT |= 1<<1;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<1);

		P2OUT |= 1<<2;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<2);

		P2OUT |= 1<<3;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<3);

		P2OUT |= 1<<4;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<4);

		P2OUT |= 1<<5;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<5);

		P1OUT |= 1<<4;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<4);

		P1OUT |= 1<<6;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<6);

		P1OUT |= 1<<7;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<7);

#endif

		P2OUT |= 1<<0;
		P1OUT &= ~(1<<5);

		P2OUT |= 1<<1;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<1);

		P2OUT |= 1<<2;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<2);

		P2OUT |= 1<<3;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<3);

		P2OUT |= 1<<4;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<4);

		P2OUT |= 1<<5;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P2OUT &= ~(1<<5);

		P1OUT |= 1<<4;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<4);

		P1OUT |= 1<<6;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<6);

		P1OUT |= 1<<7;
		  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
		P1OUT &= ~(1<<7);
	}


	  TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

	  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	CCR0 += 500; //0; //*4;  // 5ms*4 = 20 ms                           // Add Offset to CCR0
//	  P1OUT ^= 0x10;                            // Toggle P1.4


	if (!mmm--)
	{
		P1OUT ^= 0x1;
		mmm = 2000;
	}

	_BIC_SR_IRQ(LPM0_bits);                 // Enter LPM0 w/ interrupt
}
