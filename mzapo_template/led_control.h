#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "utils.h"

typedef struct{
	bool change;
	bool flashing;
	unsigned char *mem_base;
	HSV hsv;
	RGB rgb;
	HSV hsv2;
	RGB rgb2;
	unsigned long cur_time;
}LED;

extern LED led1;
extern LED led2;

void to_led_(LED led1, LED led2);

void* led_thread(void *vargp);

#endif
