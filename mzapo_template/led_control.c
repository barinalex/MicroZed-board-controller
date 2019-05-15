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

void flashing(LED *led){
	if((get_cur_time_in_mlsec() - led->last_flash_time > led->flash_time) && 
	(get_cur_time_in_mlsec() - led1.last_flash_time > led->shift)){
		if(led->last_color_f1){
			*(led->mem_base) = led->rgb_f2.b;
			*(led->mem_base + 1) = led->rgb_f2.g;
			*(led->mem_base + 2) = led->rgb_f2.r;
		}
		else{
			*(led->mem_base) = led->rgb_f1.b;
			*(led->mem_base + 1) = led->rgb_f1.g;
			*(led->mem_base + 2) = led->rgb_f1.r;
		}
		led->last_color_f1 = !led->last_color_f1;
		led->last_flash_time = get_cur_time_in_mlsec();
	}
}

void* led_thread(void *vargp){
	led1.last_flash_time = 0;
	led2.last_flash_time = 0;
	led1.last_color_f1 = false;
	led2.last_color_f1 = false;
	led1.shift = 0;
	led2.shift = 0;
		
	while(true){
		if(led1.flashing){
			flashing(&led1);
		}
		else{
			static_lighting(&led1);
		}
		if(led2.flashing){
			flashing(&led2);
		}
		else{
			static_lighting(&led2);
		}
	}
	return NULL;
}

void static_lighting(LED *led){
	*(led->mem_base) = led->rgb.b;
	*(led->mem_base + 1) = led->rgb.g;
	*(led->mem_base + 2) = led->rgb.r;
}

