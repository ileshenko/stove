#include <msp430g2452.h>
#include "display.h"
#include "timer.h"

/*
 *   a
 * f   b
 *   g
 * e   c
 *   d    p
 * */

#define LEFT_DGT()do {\
    P1OUT |= BIT5; \
	P2OUT &= ~BIT0; \
	} while (0)

#define RIGHT_DGT() do { \
	P2OUT |= BIT0; \
	P1OUT &= ~BIT5; \
	} while (0)


#define PIN(n) (1<<n)

static unsigned char ind[2];

const struct {
	volatile unsigned char *pout;
	unsigned char pin;
} seg_map[] = {
		{ &P2OUT, BIT2}, //a
		{ &P2OUT, BIT1}, //b
		{ &P2OUT, BIT3}, //c
		{ &P1OUT, BIT6}, //d
		{ &P2OUT, BIT5}, //e
		{ &P1OUT, BIT4}, //f
		{ &P1OUT, BIT7}, //g
		{ &P2OUT, BIT4}, //p
};

#define SEG_A	0x01
#define SEG_B	0x02
#define SEG_C	0x04
#define SEG_D	0x08
#define SEG_E	0x10
#define SEG_F	0x20
#define SEG_G	0x40
#define SEG_P	0x80

const unsigned char digimap[] = {
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,		// 0
	SEG_B|SEG_C,								// 1
	SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,				// 2
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,				// 3
	SEG_B|SEG_C|SEG_F|SEG_G,					// 4
	SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,				// 5
	SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,		// 6
	SEG_A|SEG_B|SEG_C,							// 7
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,	// 8
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G,		// 9
	SEG_G,										// -
	0,											// " "
};

static void seg_off(unsigned char seg)
{
	*seg_map[seg].pout &= ~(seg_map[seg].pin);
}

static void seg_on(unsigned char seg)
{
	*seg_map[seg].pout |= seg_map[seg].pin;
}

static void display_pulse(void)
{
	static unsigned char curr;
	unsigned char digit, seg;

	seg_off(curr & 0x7);
	curr++;

	seg = curr & 0x7;
	digit = !!(curr & 0x8);

	if (!seg)
	{
		if (digit)
			RIGHT_DGT();
		else
			LEFT_DGT();
	}
	if (ind[digit] & (1<<seg))
		seg_on(seg);
}

static unsigned char translate(char ch)
{
	int idx;

	switch (ch)
	{
	case '-':
		idx = 10;
		break;
	case ' ':
		idx = 11;
		break;
	default:
		idx = ch - '0';
		break;
	}
	return digimap[idx];
}

void display(const char *txt)
{
	ind[0] = translate(txt[0]);
	ind[1] = translate(txt[1]);
}

void display_int(unsigned char val)
{
	if (val >= 10)
		ind[0] = digimap[val/10];
	else
		ind[0] = digimap[11];
	ind[1] = digimap[val%10];

}

void display_time(signed char h, signed char m)
{
	if (h)
	{
		ind[0] = digimap[h];
		ind[0] |= SEG_P;
		ind[1] = digimap[m/10];
	}
	else
	{
		display_int(m);
		ind[1] |= SEG_P;
	}
}

void display_init(void)
{
	P1DIR |= PIN(4)+ PIN(5)+ PIN(6) + PIN(7);
	P2DIR |= PIN(0)+ PIN(1)+ PIN(2)+ PIN(3)+ PIN(4)+ PIN(5);
//	P2OUT = 0x00;
	
//	P2SEL &= ~(PIN(7) + PIN(6));
//	P2SEL2 &= ~(PIN(7) + PIN(6));

	timer_reg_1ms(display_pulse);
	ind[0] = 0x0f;
	ind[1] = 0xf0;
}
# if 0
static char dd[2] = {0,0};
static char point;

inline static void show_digit(char digit, char idx)
{
	P1OUT &= ~DP1_MSK;
	P2OUT &= ~DP2_MSK;
	
	P1OUT |= digimap[digit].p1;
	P2OUT |= digimap[digit].p2 | (idx ? LFT : (RGH | (point ? D_P : 0))) ;
//	P2OUT |= D_A;
//	P2OUT |= RGH;
}

void display_pulse(void)
{
	static char idx = 0;

	show_digit(dd[idx], idx);
	idx = !idx;
}

void display_set(char val)
{
	point = val & 0x80;
	val &= 0x7f;
	
	if (val > 99)
		dd[0] = dd[1] = 10;
	else
	{
		dd[0] = val%10;
		dd[1] = val /10;
	}
}

void display_stat(display_stat_t stat)
{
	switch (stat)
	{
	case DISP_NONE:
		dd[0] = dd[1] = 10;
		break;
	case DISP_DOWN:
		display_set(0x80 | temp_bottom);
		break;
	case DISP_UP:
		display_set(temp_up);
		break;
	case DISP_CTL:
		display_set(temp_ctl);
		break;
	default:
			dd[0] = dd[1] = 11;
	}
}


void display_test(void)
{
//	static int qq = 0;
//	qq++;
//	if (qq >= 10)
//		qq = 0;
//	P2OUT ^= D_E;
//	P1OUT ^= D_C;
}
#endif
