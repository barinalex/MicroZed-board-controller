#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcdframe.h"

#define BLACK 0x0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define DELAY 200000

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


typedef struct{
	bool on;
	bool to_2;
	HSV hsv;
	RGB rgb;
	HSV hsv2;
	RGB rgb2;
	HSV hsv_cur;
	RGB rgb_cur;

	unsigned long illumination_time; //milisec
	unsigned long extinction_time; //milisec
	unsigned long change_time; //milisec
	unsigned long last_change_time; //milisec
	unsigned long shift; //milisec
}mode_;


bool big_text;

void choose_colors(int menu_pos, mode_ *mode1, mode_ *mode2);

void choose_time(unsigned long *led1time, unsigned long *led2time, int lcd_pos);

void set_ptr_to_hsv_rgb(HSV** hsv1, HSV** hsv2, RGB** rgb1, RGB** rgb2, mode_ *mode1, mode_ *mode2, rect_ *rect_led, int color);

void change_hsv_rgb(HSV* hsv, RGB* rgb, knobs_ knobs, knobs_ prev_knobs);

void get_knobs_data(knobs_ *knobs);

void rectangle_to_lcd(RGB rgb, rect_ rect);

unsigned long get_cur_time();

unsigned long get_cur_time_in_mlsec();

rect_ set_rect(int menu_pos);

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t prev_value, int menu_pos);

RGB HsvToRgb(HSV hsv);

bool is_increased(uint8_t cur_value, uint8_t prev_value);

bool is_decreased(uint8_t cur_value, uint8_t prev_value);

uint16_t change(int data, uint8_t cur_value, uint8_t prev_value, uint16_t max_data);

int change_int(int data, uint8_t cur_value, uint8_t prev_value, int step);

#endif
