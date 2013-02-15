#ifndef _DISPLAY_H_
#define _DISPLAY_H_

void display_init(void);

void display(const char *txt);
void display_int(unsigned char val);
void display_time(signed char d_h, signed char d_m);

#endif /*_DISPLAY_H_*/
