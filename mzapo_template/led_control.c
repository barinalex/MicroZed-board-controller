#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "led_control.h"

void change_hsv_(uint16_t *cur_color_param, uint16_t color1_param, uint16_t color2_param, int *decrement, bool *to_2);

void light_off(LED *led);

void set_last_time(){
	unsigned long cur_time = get_cur_time_in_mlsec();
	led1.color_flash.last_change_time = cur_time;
	led1.flash.last_change_time = cur_time;
	led1.st.last_change_time = cur_time;
	led1.cont.last_change_time = cur_time;
	led1.cont.h_last_change_time = cur_time;
	led1.cont.s_last_change_time = cur_time;
	led1.cont.v_last_change_time = cur_time;
	
	led2.color_flash.last_change_time = cur_time;
	led2.flash.last_change_time = cur_time;
	led2.st.last_change_time = cur_time;
	led2.cont.last_change_time = cur_time;
	led2.cont.h_last_change_time = cur_time;
	led2.cont.s_last_change_time = cur_time;
	led2.cont.v_last_change_time = cur_time;
}

void change_hsv(uint16_t *cur_color_param, uint16_t color1_param, uint16_t color2_param, int *decrement, bool *to_2){
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
		change_hsv(&(led->cont.hsv_cur.h), led->cont.hsv.h, led->cont.hsv2.h, &(led->cont.h_decrement), &(led->cont.h_to_2));
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
	//printf("%d %d %d\n", led->cont.hsv_cur.h, led->cont.hsv_cur.s, led->cont.hsv_cur.v);
	led->cont.rgb_cur = hsv_to_rgb(led->cont.hsv_cur);	
	
	if(led->illuminate){
		*(led->mem_base) = led->cont.rgb_cur.b;
		*(led->mem_base + 1) = led->cont.rgb_cur.g;
		*(led->mem_base + 2) = led->cont.rgb_cur.r;
	}else{
		light_off(led);
	}
}

void check_time(LED *led, unsigned long time){
	if(get_cur_time_in_mlsec() - led->flash.last_change_time > time + led->flash.shift){
		led->flash.shift = 0;
		led->illuminate = !(led->illuminate);
		led->flash.last_change_time = get_cur_time_in_mlsec();
	}
}

void flashing(LED *led){
	if(led->illuminate){
		check_time(led, led->flash.illumination_time);
	}else{
		check_time(led, led->flash.extinction_time);
	}
}

void color_flashing(LED *led){
	if((get_cur_time_in_mlsec() - led->color_flash.last_change_time > led->color_flash.change_time)){
		if(led->color_flash.to_2){
			led->color_flash.rgb_cur = led->color_flash.rgb2;
		}
		else{
			led->color_flash.rgb_cur = led->color_flash.rgb;
		}
		led->color_flash.to_2 = !led->color_flash.to_2;
		led->color_flash.last_change_time = get_cur_time_in_mlsec();
	}
	if(led->illuminate){
		*(led->mem_base) = led->color_flash.rgb_cur.b;
		*(led->mem_base + 1) = led->color_flash.rgb_cur.g;
		*(led->mem_base + 2) = led->color_flash.rgb_cur.r;
	}else{
		light_off(led);
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
	light_off(&led1);
	light_off(&led2);
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
		else if(led1.st.on){
			static_lighting(&led1);
		}
		else{
			light_off(&led1);
		}
		if(led2.color_flash.on){
			color_flashing(&led2);
		}
		else if(led2.cont.on){
			continuously_changing(&led2);
		}
		else if(led2.st.on){
			static_lighting(&led2);
		}else{
			light_off(&led2);
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
		*(led->mem_base) = led->st.rgb.b;
		*(led->mem_base + 1) = led->st.rgb.g;
		*(led->mem_base + 2) = led->st.rgb.r;
	}else{
		light_off(led);
	}
}

