#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

typedef struct{
	int top;
	int bottom;
	int left;
	int right;
}rect_;

rect_ set_rect(int menu_pos);
int change_menu_pos(int buttons_number, uint8_t button_value, uint8_t prev_button_value, int menu_pos);

#endif
