#ifndef _BUTTONS_H_
#define _BUTTONS_H_

void buttons_init(void);

typedef enum {
	BTN_IDLE = 0,
	BTN_UNDEFINED = 1,
	BTN_UP = 2,
	BTN_UP_LONG = 3,
	BTN_DOWN = 4,
	BTN_DOWN_LONG = 5,
	BTN_BOTH = 6,
	BTN_RESET = 7
} button_state_t;

button_state_t buttons_get(void);

//extern unsigned char up_is, down_is;

#endif /*_BUTTONS_H_*/
