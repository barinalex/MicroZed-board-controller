#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>

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

unsigned long get_cur_time();

rect_ set_rect(int menu_pos);

int change_menu_pos(int buttons_number, uint8_t cur_value, uint8_t prev_value, int menu_pos);

RGB HsvToRgb(HSV hsv);

bool is_increased(uint8_t cur_value, uint8_t prev_value);

bool is_decreased(uint8_t cur_value, uint8_t prev_value);

#endif
