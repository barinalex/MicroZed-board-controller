#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "utils.h"

typedef struct{
	bool change;
	bool flashing;
	bool last_color_f1;
	unsigned char *mem_base;
	HSV hsv;
	RGB rgb;
	HSV hsv_f1;
	RGB rgb_f1;
	HSV hsv_f2;
	RGB rgb_f2;
	unsigned long flash_time; //milisec
	unsigned long last_flash_time; //milisec
	unsigned long shift; //milisec
}LED;

LED led1;
LED led2;

void to_led_(LED led1, LED led2);
void static_lighting(LED *led);

void* led_thread(void *vargp);

#endif
