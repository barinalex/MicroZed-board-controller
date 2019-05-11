#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"

unsigned long get_cur_time(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
	return time_in_micros;
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
