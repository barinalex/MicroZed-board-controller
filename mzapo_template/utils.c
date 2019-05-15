#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"
#include "reg_manager.h"

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

uint16_t change(int data, uint8_t cur_value, uint8_t prev_value, uint16_t max_data){
		if(is_increased(cur_value, prev_value)){
			data++;
			data %= (max_data + 1);
		}
		else if(is_decreased(cur_value, prev_value)){
			data--;
		}
		data = (data > max_data) ? max_data: data;
		data = (data < 0) ? max_data: data;
		printf("data: %d\n", data);
		return data;
}

int change_int(int data, uint8_t cur_value, uint8_t prev_value){
		if(is_increased(cur_value, prev_value)){
			data++;
		}
		else if(is_decreased(cur_value, prev_value)){
			data--;
		}
		data = (data < 0) ? 0: data;
		return data;
}

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t prev_value, int menu_pos){
		if(is_increased(cur_value, prev_value)){
			menu_pos++;
			menu_pos = (menu_pos > buttons_number - 1) ? buttons_number - 1: menu_pos;
		}
		else if(is_decreased(cur_value, prev_value)){
			menu_pos--;
			menu_pos = (menu_pos < 0) ? 0: menu_pos;
		}
		return menu_pos;
}

bool is_increased(uint8_t cur_value, uint8_t prev_value){
	return ((cur_value > prev_value + 1) &&
			!(cur_value > 250 && prev_value < 5)) ||
			(cur_value < 5 && prev_value > 250);
}

bool is_decreased(uint8_t cur_value, uint8_t prev_value){
	return cur_value < prev_value - 1 || 
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

RGB HsvToRgb(HSV hsv) {
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
