#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"

int change_menu_pos(int buttons_number, uint8_t button_value, uint8_t prev_button_value, int menu_pos){
		if(((button_value > prev_button_value + 1) && !(button_value > 250 && prev_button_value < 5)) || (button_value < 5 && prev_button_value > 250)){
			menu_pos++;
			menu_pos = (menu_pos > buttons_number - 1) ? buttons_number - 1: menu_pos;
		}
		else if(button_value < prev_button_value - 1 || (button_value > 250 && prev_button_value < 5)){
			menu_pos--;
			menu_pos = (menu_pos < 0) ? 0: menu_pos;
		}
		return menu_pos;
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

		if (h == 360)
			h = 0;
		else
			h = h / 60;

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
