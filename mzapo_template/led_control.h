#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "utils.h"

typedef struct{
	unsigned char *mem_base;
	mode_ color_flash;
	mode_ flash;
	mode_ cont;
	mode_ st;
	
	bool change;
	bool illuminate;
}LED;

LED led1;
LED led2;
LED led1_saved;
LED led2_saved;
LED led1_empty;
LED led2_empty;

void static_lighting(LED *led);

void* led_thread(void *vargp);

void set_last_time();

void cont_on(LED *led, bool to_2);

#endif
