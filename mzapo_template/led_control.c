#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "led_control.h"


void to_led_(LED led1, LED led2){
	*led1.mem_base = led1.rgb.b;
	*(led1.mem_base + 1) = led1.rgb.g;
	*(led1.mem_base + 2) = led1.rgb.r;
	
	*led2.mem_base = led2.rgb.b;
	*(led2.mem_base + 1) = led2.rgb.g;
	*(led2.mem_base + 2) = led2.rgb.r;
}


void* led_thread(void *vargp){
	while(true){
		*led1.mem_base = led1.rgb.b;
		*(led1.mem_base + 1) = led1.rgb.g;
		*(led1.mem_base + 2) = led1.rgb.r;
		
		*led2.mem_base = led2.rgb.b;
		*(led2.mem_base + 1) = led2.rgb.g;
		*(led2.mem_base + 2) = led2.rgb.r;
	}
	return NULL;
}

