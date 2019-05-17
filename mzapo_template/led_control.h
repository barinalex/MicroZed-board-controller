#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "utils.h"

typedef struct{
	unsigned char *mem_base;
	mode_ color_flash;
	mode_ flash;
	mode_ cont;
	
	bool change;
	bool illuminate;

	HSV hsv;
	RGB rgb;
}LED;

LED led1;
LED led2;


void to_led_(LED led1, LED led2);
void static_lighting(LED *led);

void* led_thread(void *vargp);

#endif
