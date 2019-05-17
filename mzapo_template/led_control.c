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
	if( (cur_time - (led->cont.change_time / fabs((double)(led->cont.hsv.h - led->cont.hsv2.h)))) > led->cont.last_change_time){
		if(led->cont.hsv.h < led->cont.hsv2.h){
			if(led->cont.to_2){
				led->cont.hsv_cur.h ++;
				if(led->cont.hsv_cur.h >= led->cont.hsv2.h){
					led->cont.to_2 = !led->cont.to_2;
					led->cont.hsv_cur.h = led->cont.hsv2.h;
				}	
			}
			else{
				led->cont.hsv_cur.h = (led->cont.hsv_cur.h == 0) ? 0: led->cont.hsv_cur.h - 1;	
				if(led->cont.hsv_cur.h <= led->cont.hsv.h){
					led->cont.to_2 = !led->cont.to_2;
					led->cont.hsv_cur.h = led->cont.hsv.h;
				}
			}
		}
		else{
			if(!led->cont.to_2){
				led->cont.hsv_cur.h ++;
				if(led->cont.hsv_cur.h >= led->cont.hsv2.h){
					led->cont.to_2 = !led->cont.to_2;
					led->cont.hsv_cur.h = led->cont.hsv2.h;
				}	
			}
			else{
				led->cont.hsv_cur.h = (led->cont.hsv_cur.h == 0) ? 0: led->cont.hsv_cur.h - 1;	
				if(led->cont.hsv_cur.h <= led->cont.hsv.h){
					led->cont.to_2 = !led->cont.to_2;
					led->cont.hsv_cur.h = led->cont.hsv.h;
				}
			}
		}
		led->cont.last_change_time = get_cur_time_in_mlsec();
	}
	printf("%d %d %d\n", led->cont.hsv_cur.h, led->cont.hsv_cur.s, led->cont.hsv_cur.v);
	led->cont.rgb_cur = HsvToRgb(led->cont.hsv_cur);	
	*(led->mem_base) = led->cont.rgb_cur.b;
	*(led->mem_base + 1) = led->cont.rgb_cur.g;
	*(led->mem_base + 2) = led->cont.rgb_cur.r;
}

void color_flashing(LED *led){
	if((get_cur_time_in_mlsec() - led->color_flash.last_change_time > led->color_flash.change_time) && 
	(get_cur_time_in_mlsec() - led1.color_flash.last_change_time > led->color_flash.shift)){
		if(led->color_flash.to_2){
			*(led->mem_base) = led->color_flash.rgb2.b;
			*(led->mem_base + 1) = led->color_flash.rgb2.g;
			*(led->mem_base + 2) = led->color_flash.rgb2.r;
		}
		else{
			*(led->mem_base) = led->color_flash.rgb.b;
			*(led->mem_base + 1) = led->color_flash.rgb.g;
			*(led->mem_base + 2) = led->color_flash.rgb.r;
		}
		led->color_flash.to_2 = !led->color_flash.to_2;
		led->color_flash.last_change_time = get_cur_time_in_mlsec();
	}
}

void* led_thread(void *vargp){
	led1.flash.last_change_time = 0;
	led2.flash.last_change_time = 0;
	led1.flash.on = false;
	led2.flash.on = false;
	led1.flash.shift = 0;
	led2.flash.shift = 0;
	led1.color_flash.last_change_time = 0;
	led2.color_flash.last_change_time = 0;
	led1.color_flash.on = false;
	led2.color_flash.on = false;
	led1.color_flash.shift = 0;
	led2.color_flash.shift = 0;
	led1.cont.on = false;
	led2.cont.on = false;
		
	while(true){
		if(led1.color_flash.on){
			color_flashing(&led1);
		}
		else if(led1.cont.on){
			continuously_changing(&led1);
		}
		else{
			static_lighting(&led1);
		}
		if(led2.color_flash.on){
			color_flashing(&led2);
		}
		else if(led2.cont.on){
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

