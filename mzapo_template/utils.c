#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"
#include "reg_manager.h"
#include "connection.h"

int get_knobs_value(){
	if(nw_state.connected && nw_state.receiving){
		return received_knobs_value;
	}
	return *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void choose_colors(int menu_pos, mode_ *mode1, mode_ *mode2){
	rect_ rect_led;
	HSV *led1hsv, *led2hsv;
	RGB *led1rgb, *led2rgb;
	
	set_ptr_to_hsv_rgb(&led1hsv, &led2hsv, &led1rgb, &led2rgb, mode1, mode2, &rect_led, menu_pos);
	LED saved_led1 = led1;
	LED saved_led2 = led2;
	
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	while(true){
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			led1 = saved_led1;
			led2 = saved_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			change_rgb_hsv(led1hsv, led1rgb, knobs, prev_knobs);
			rectangle_to_lcd(*led1rgb, rect_led);
		}
		if(led2.change){
			change_rgb_hsv(led2hsv, led2rgb, knobs, prev_knobs);
			rectangle_to_lcd(*led2rgb, rect_led);
		}
		prev_knobs = knobs;
		frameToLCD();
	}
}

void choose_time(unsigned long *led1time, unsigned long *led2time, int lcd_pos, int border){
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	LED saved_led1 = led1;
	LED saved_led2 = led2;
	
	while(true){
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			led1 = saved_led1;
			led2 = saved_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			*led1time = change_long(*led1time, knobs.b_knob, &(prev_knobs.b_knob), 100, border);
			int_to_frame(*led1time, lcd_pos, 240, WHITE, BLACK, big_text);
			strToFrame(" ms", lcd_pos, 240 + 100, WHITE, BLACK, big_text);
		}
		if(led2.change){
			*led2time = change_long(*led2time, knobs.b_knob, &(prev_knobs.b_knob), 100, border);
			int_to_frame(*led2time, lcd_pos, 240, WHITE, BLACK, big_text);
			strToFrame(" ms", lcd_pos, 240 + 100, WHITE, BLACK, big_text);
		}
		frameToLCD();
		prev_knobs = knobs;
	}
}

void set_ptr_to_hsv_rgb(HSV** hsv1, HSV** hsv2, RGB** rgb1, RGB** rgb2, mode_ *mode1, mode_ *mode2, rect_ *rect_led, int color){
	switch(color){
		case 1:
			*rect_led = set_rect(1);
			*hsv1 = &(mode1->hsv2);
			*rgb1 = &(mode1->rgb2);
			*hsv2 = &(mode2->hsv2);
			*rgb2 = &(mode2->rgb2);
			break;
		default:
			*rect_led = set_rect(0);
			*hsv1 = &(mode1->hsv);
			*rgb1 = &(mode1->rgb);
			*hsv2 = &(mode2->hsv);
			*rgb2 = &(mode2->rgb);
			break;
	}
}
void change_rgb_hsv(HSV* hsv, RGB* rgb, knobs_ knobs, knobs_ prev_knobs){
	rgb->r = change(rgb->r, knobs.r_knob, &(prev_knobs.r_knob), 255); 
	rgb->g = change(rgb->g, knobs.g_knob, &(prev_knobs.g_knob), 255); 
	rgb->b = change(rgb->b, knobs.b_knob, &(prev_knobs.b_knob), 255);
	printf("rgb %d %d %d\n", rgb->r, rgb->g, rgb->b);
	*hsv = rgb_to_hsv(*rgb);
	printf("hsv %d %d %d\n", hsv->h, hsv->s, hsv->v);
}

void change_hsv_rgb(HSV* hsv, RGB* rgb, knobs_ knobs, knobs_ prev_knobs){
	hsv->h = change(hsv->h, knobs.r_knob, &(prev_knobs.r_knob), 360); 
	hsv->s = change(hsv->s, knobs.g_knob, &(prev_knobs.g_knob), 100); 
	hsv->v = change(hsv->v, knobs.b_knob, &(prev_knobs.b_knob), 100);
	*rgb = hsv_to_rgb(*hsv);
	printf("hsv %d %d %d\n", hsv->h, hsv->s, hsv->v);
	printf("rgb %d %d %d\n", rgb->r, rgb->g, rgb->b);	
}

void get_knobs_data(knobs_ *knobs){
	knobs->rgb_value = get_knobs_value();
	knobs->b_knob =  knobs->rgb_value      & 0xFF; // blue knob position
	knobs->g_knob = (knobs->rgb_value>>8)  & 0xFF; // green knob position
	knobs->r_knob = (knobs->rgb_value>>16) & 0xFF; // red knob position
	knobs->b_button = (knobs->rgb_value>>24) & 1; // blue button
	knobs->g_button = (knobs->rgb_value>>25) & 1; // green button
	knobs->r_button = (knobs->rgb_value>>26) & 1; // red buttom
}

unsigned long get_cur_time(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
	return time_in_micros;
}

unsigned long get_cur_time_in_mlsec(){
	return get_cur_time() / 1000;
}
	
void rectangle_to_lcd(RGB rgb, rect_ rect){
	uint16_t color = 0;
	color = ((rgb.r >> 3) << 11) | ((rgb.g >> 2) << 5) | (rgb.b >> 3);
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			if(r >= rect.top && r < rect.bottom && c >= rect.left && c < rect.right){
				frame[r][c] = color;
			}
		}
	}
	frameToLCD(parlcd_mem_base);
}

uint16_t change(int data, uint8_t cur_value, uint8_t *prev_value, int max_data){
		data += get_difference(cur_value, prev_value);
		data %= (max_data + 1);
		data = (data > max_data) ? max_data: data;
		data = (data < 0) ? max_data: data;
		printf("data: %d\n", data);
		return data;
}

unsigned long change_long(long long data, uint8_t cur_value, uint8_t *prev_value, int step, int border){
		data += get_difference(cur_value, prev_value) * step;
		if(border > 0){
			data %= border;
		}
		data = (data < 0) ? 0: data;
		printf("data: %lld\n", data);
		return data;
}

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t *prev_value, int menu_pos){
		if(is_increased(cur_value, *prev_value)){
			menu_pos++;
			menu_pos = (menu_pos > buttons_number - 1) ? buttons_number - 1: menu_pos;
			*prev_value = cur_value;
		}
		else if(is_decreased(cur_value, *prev_value)){
			menu_pos--;
			menu_pos = (menu_pos < 0) ? 0: menu_pos;
			*prev_value = cur_value;
		}
		return menu_pos;
}

int get_difference(uint8_t cur_value, uint8_t *prev_value){
	int difference = 0;
	if(cur_value < 50 && *prev_value > 200){
		difference = cur_value + (255 - *prev_value);
		*prev_value = cur_value;
	}
	else if(cur_value > 200 && *prev_value < 50){
		difference = - *prev_value - (255 - cur_value);
		*prev_value = cur_value;
	}
	else if(abs(cur_value - *prev_value) > 3){
		difference = (int)cur_value - ((int)*prev_value - 3);
		*prev_value = cur_value;
	}
	return difference;
}

bool is_increased(uint8_t cur_value, uint8_t prev_value){
	return ((cur_value > prev_value + 3) &&
			!(cur_value > 250 && prev_value < 5)) ||
			(cur_value < 5 && prev_value > 250);
}

bool is_decreased(uint8_t cur_value, uint8_t prev_value){
	return ((cur_value < prev_value - 3) &&
			!(cur_value < 5 && prev_value > 250))|| 
			(cur_value > 250 && prev_value < 5);
}

rect_ set_rect(int menu_pos){
	rect_ rect;
	rect.left = 240;
	rect.right = 400;
	switch(menu_pos){
		case 0:
			rect.top = 30;
			rect.bottom = 50;
			break;
		case 1:
			rect.top = 80;
			rect.bottom = 100;
			break;
		case 2:
			rect.top = 130;
			rect.bottom = 150;
			break;
		case 3:
			rect.top = 180;
			rect.bottom = 200;
			break;
		case 4:
			rect.top = 230;
			rect.bottom = 250;
			break;
	}
	return rect;
}

RGB hsv_to_rgb(HSV hsv) {
	double r = 0, g = 0, b = 0;
	double h = (double)hsv.h, s = (double)hsv.s/100, v = (double)hsv.v/100;

	if (hsv.s == 0)
	{
		r = v;
		g = v;
		b = v;
	}
	else
	{
		int i;
		double f, p, q, t;
		h = (h == 360) ? 0: h / 60;
		i = (int)trunc(h);
		f = h - i;
		p = v * (1.0 - s);
		q = v * (1.0 - (s * f));
		t = v * (1.0 - (s * (1.0 - f)));
		switch (i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = v;
			b = p;
			break;

		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;

		case 4:
			r = t;
			g = p;
			b = v;
			break;

		default:
			r = v;
			g = p;
			b = q;
			break;
		}

	}
	RGB rgb;
	rgb.r = r * 255;
	rgb.g = g * 255;
	rgb.b = b * 255;
	return rgb;
}

static uint8_t Min(uint8_t a, uint8_t b) {
	return a <= b ? a : b;
}

static uint8_t Max(uint8_t a, uint8_t b) {
	return a >= b ? a : b;
}

HSV rgb_to_hsv(RGB rgb) {
	double delta, min;
	double h = 0, s, v;

	min = Min(Min(rgb.r, rgb.g), rgb.b);
	v = Max(Max(rgb.r, rgb.g), rgb.b);
	delta = v - min;

	s = (v == 0.0) ? 0 : delta / v;

	if (s == 0){
		h = 0.0;
	}
	else
	{
		if (rgb.r == v)
			h = (rgb.g - rgb.b) / delta;
		else if (rgb.g == v)
			h = 2 + (rgb.b - rgb.r) / delta;
		else if (rgb.b == v)
			h = 4 + (rgb.r - rgb.g) / delta;

		h *= 60;

		if (h < 0.0)
			h = h + 360;
	}

	HSV hsv;
	hsv.h = (uint16_t)h;
	hsv.s = (uint8_t)(s * 100);
	hsv.v = (uint8_t)((v / 255) * 100);

	return hsv;
}
