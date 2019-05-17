#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "led_control.h"

void change_hsv_(uint16_t *cur_color_param, uint16_t color1_param, uint16_t color2_param, int *decrement, bool *to_2);

void light_off(LED *led);

void to_led_(LED led1, LED led2){
	*led1.mem_base = led1.rgb.b;
	*(led1.mem_base + 1) = led1.rgb.g;
	*(led1.mem_base + 2) = led1.rgb.r;
	
	*led2.mem_base = led2.rgb.b;
	*(led2.mem_base + 1) = led2.rgb.g;
	*(led2.mem_base + 2) = led2.rgb.r;
}

void change_hsv_16(uint16_t *cur_color_param, uint16_t color1_param, uint16_t color2_param, int *decrement, bool *to_2){
	if(*to_2){
		if((*cur_color_param) == color2_param){
			(*to_2) = !(*to_2);
			(*decrement) *= -1;
		}
	}else{
		if((*cur_color_param) == color1_param){
			(*to_2) = !(*to_2);
			(*decrement) *= -1;
		}
	}
	(*cur_color_param) += (*decrement);
}

void change_hsv_8(uint8_t *cur_color_param, uint8_t color1_param, uint8_t color2_param, int *decrement, bool *to_2){
	if(*to_2){
		if((*cur_color_param) == color2_param){
			(*to_2) = !(*to_2);
			(*decrement) *= -1;
		}
	}else{
		if((*cur_color_param) == color1_param){
			(*to_2) = !(*to_2);
			(*decrement) *= -1;
		}
	}
	(*cur_color_param) += (*decrement);
}

void continuously_changing(LED *led){
	if( (get_cur_time_in_mlsec() - (led->cont.change_time / fabs((double)(led->cont.hsv.h - led->cont.hsv2.h)))) > led->cont.h_last_change_time){
		change_hsv_16(&(led->cont.hsv_cur.h), led->cont.hsv.h, led->cont.hsv2.h, &(led->cont.h_decrement), &(led->cont.h_to_2));
		led->cont.h_last_change_time = get_cur_time_in_mlsec();
	}
	if( (get_cur_time_in_mlsec() - (led->cont.change_time / fabs((double)(led->cont.hsv.s - led->cont.hsv2.s)))) > led->cont.s_last_change_time){
		change_hsv_8(&(led->cont.hsv_cur.s), led->cont.hsv.s, led->cont.hsv2.s, &(led->cont.s_decrement), &(led->cont.s_to_2));
		led->cont.s_last_change_time = get_cur_time_in_mlsec();
	}
	if( (get_cur_time_in_mlsec() - (led->cont.change_time / fabs((double)(led->cont.hsv.v - led->cont.hsv2.v)))) > led->cont.v_last_change_time){
		change_hsv_8(&(led->cont.hsv_cur.v), led->cont.hsv.v, led->cont.hsv2.v, &(led->cont.v_decrement), &(led->cont.v_to_2));
		led->cont.v_last_change_time = get_cur_time_in_mlsec();
	}
	printf("%d %d %d\n", led->cont.hsv_cur.h, led->cont.hsv_cur.s, led->cont.hsv_cur.v);
	led->cont.rgb_cur = HsvToRgb(led->cont.hsv_cur);	
	
	if(led->illuminate){
		*(led->mem_base) = led->cont.rgb_cur.b;
		*(led->mem_base + 1) = led->cont.rgb_cur.g;
		*(led->mem_base + 2) = led->cont.rgb_cur.r;
	}else{
		light_off(led);
	}
}

void flashing(LED *led){
	if(led->illuminate){
		if(get_cur_time_in_mlsec() - led->flash.last_change_time > led->flash.illumination_time){
			led->illuminate = !(led->illuminate);
			led->flash.last_change_time = get_cur_time_in_mlsec();
		}
	}else{
		if(get_cur_time_in_mlsec() - led->flash.last_change_time > led->flash.extinction_time){
			led->illuminate = !(led->illuminate);
			led->flash.last_change_time = get_cur_time_in_mlsec();
		}
	}
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
	led1.illuminate = true;
	led2.illuminate = true;
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
		if(led1.flash.on){
			flashing(&led1);
		}
		if(led2.flash.on){
			flashing(&led2);
		}
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

void light_off(LED *led){
	*(led->mem_base) = 0;
	*(led->mem_base + 1) = 0;
	*(led->mem_base + 2) = 0;
}

void static_lighting(LED *led){
	if(led->illuminate){
		*(led->mem_base) = led->rgb.b;
		*(led->mem_base + 1) = led->rgb.g;
		*(led->mem_base + 2) = led->rgb.r;
	}else{
		light_off(led);
	}
}

