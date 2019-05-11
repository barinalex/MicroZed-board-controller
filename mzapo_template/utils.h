#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcdframe.h"

typedef struct{
	uint32_t rgb_value;
	uint8_t r_knob;
	uint8_t g_knob;
	uint8_t b_knob;
	uint8_t r_button;
	uint8_t g_button;
	uint8_t b_button;	
} knobs_;

typedef struct{
	int top;
	int bottom;
	int left;
	int right;
}rect_;

typedef struct{
	uint16_t h;
	uint8_t s;
	uint8_t v;
}HSV;

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RGB;

void get_knobs_data(knobs_ *knobs);

void rectangle_to_lcd(RGB rgb, rect_ rect);

unsigned long get_cur_time();

rect_ set_rect(int menu_pos);

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t prev_value, int menu_pos);

RGB HsvToRgb(HSV hsv);

bool is_increased(uint8_t cur_value, uint8_t prev_value);

bool is_decreased(uint8_t cur_value, uint8_t prev_value);

uint16_t change(int data, uint8_t cur_value, uint8_t prev_value, uint16_t max_data);

int change_int(int data, uint8_t cur_value, uint8_t prev_value);

#endif
