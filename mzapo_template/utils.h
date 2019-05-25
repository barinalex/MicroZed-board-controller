#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcdframe.h"

#define BLACK 0x0
#define RED 0xF800
#define SEA 0x6715
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
	
	bool h_to_2;
	bool s_to_2;
	bool v_to_2;
	
	int h_decrement;
	int s_decrement;
	int v_decrement;
	
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
	
	unsigned long h_last_change_time; //milisec
	unsigned long s_last_change_time; //milisec
	unsigned long v_last_change_time; //milisec
	
	unsigned long shift; //milisec
}mode_;


bool big_text;

int get_knobs_value();

int get_difference(uint8_t cur_value, uint8_t *prev_value);

void choose_colors(int color_num, int rect_pos, mode_ *mode1, mode_ *mode2);

void choose_time(unsigned long *led1time, unsigned long *led2time, int lcd_pos, int border);

void set_ptr_to_hsv_rgb(HSV** hsv1, HSV** hsv2, RGB** rgb1, RGB** rgb2, mode_ *mode1, mode_ *mode2, int color_num);

void set_rects_pos(rect_ *rect_led1, rect_ *rect_led2, int rect_pos);

void change_rgb_hsv(HSV* hsv, RGB* rgb, knobs_ knobs, knobs_ *prev_knobs);

void change_hsv_rgb(HSV* hsv, RGB* rgb, knobs_ knobs, knobs_ *prev_knobs);

void get_knobs_data(knobs_ *knobs);

void rectangle_to_lcd(RGB rgb, rect_ rect);

void rect_to_lcd(uint16_t color, rect_ rect);

unsigned long get_cur_time();

unsigned long get_cur_time_in_mlsec();

rect_ set_rect(int menu_pos);

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t *prev_value, int menu_pos);

RGB hsv_to_rgb(HSV hsv);

HSV rgb_to_hsv(RGB rgb);

bool is_increased(uint8_t cur_value, uint8_t prev_value);

bool is_decreased(uint8_t cur_value, uint8_t prev_value);

uint16_t change(int data, uint8_t cur_value, uint8_t *prev_value, int max_data);

unsigned long change_long(long long data, uint8_t cur_value, uint8_t *prev_value, int step, int border);

#endif
