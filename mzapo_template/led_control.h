#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "utils.h"

typedef struct{
	bool change;
	bool flashing;
	bool continuous;
	bool c1_to_c2;
	bool last_color_f1;
	unsigned char *mem_base;
	HSV hsv;
	RGB rgb;
	
	HSV hsv_f1;
	RGB rgb_f1;
	HSV hsv_f2;
	RGB rgb_f2;
	
	HSV hsv_c1;
	RGB rgb_c1;
	HSV hsv_c2;
	RGB rgb_c2;
	HSV hsv_current;
	RGB rgb_current;
	unsigned long flash_time; //milisec
	unsigned long last_flash_time; //milisec
	unsigned long shift; //milisec
	
	unsigned long continuous_time; //milisec
	unsigned long last_continuous_time; //milisec
}LED;

LED led1;
LED led2;

void to_led_(LED led1, LED led2);
void static_lighting(LED *led);

void* led_thread(void *vargp);

#endif
