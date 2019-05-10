#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

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

