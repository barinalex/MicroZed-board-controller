#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
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

void continuously_changing(LED *led){
	unsigned long cur_time = get_cur_time_in_mlsec();
	if( (cur_time - (led->continuous_time / fabs((double)(led->hsv_c1.h - led->hsv_c2.h)))) > led->last_continuous_time){
		if(led->hsv_c1.h < led->hsv_c2.h){
			if(led->c1_to_c2){
				led->hsv_current.h ++;
				if(led->hsv_current.h >= led->hsv_c2.h){
					led->c1_to_c2 = !led->c1_to_c2;
					led->hsv_current.h = led->hsv_c2.h;
				}	
			}
			else{
				led->hsv_current.h = (led->hsv_current.h == 0) ? 0: led->hsv_current.h - 1;	
				if(led->hsv_current.h <= led->hsv_c1.h){
					led->c1_to_c2 = !led->c1_to_c2;
					led->hsv_current.h = led->hsv_c1.h;
				}
			}
		}
		else{
			if(!led->c1_to_c2){
				led->hsv_current.h ++;
				if(led->hsv_current.h >= led->hsv_c2.h){
					led->c1_to_c2 = !led->c1_to_c2;
					led->hsv_current.h = led->hsv_c2.h;
				}	
			}
			else{
				led->hsv_current.h = (led->hsv_current.h == 0) ? 0: led->hsv_current.h - 1;	
				if(led->hsv_current.h <= led->hsv_c1.h){
					led->c1_to_c2 = !led->c1_to_c2;
					led->hsv_current.h = led->hsv_c1.h;
				}
			}
		}
		led->last_continuous_time = get_cur_time_in_mlsec();
	}
	printf("%d %d %d\n", led->hsv_current.h, led->hsv_current.s, led->hsv_current.v);
	led->rgb_current = HsvToRgb(led->hsv_current);	
	*(led->mem_base) = led->rgb_current.b;
	*(led->mem_base + 1) = led->rgb_current.g;
	*(led->mem_base + 2) = led->rgb_current.r;
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
	led1.c1_to_c2 = true;
	led1.c1_to_c2 = true;
	led1.shift = 0;
	led2.shift = 0;
		
	while(true){
		if(led1.flashing){
			flashing(&led1);
		}
		else if(led1.continuous){
			continuously_changing(&led1);
		}
		else{
			static_lighting(&led1);
		}
		if(led2.flashing){
			flashing(&led2);
		}
		else if(led2.continuous){
			continuously_changing(&led2);
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

