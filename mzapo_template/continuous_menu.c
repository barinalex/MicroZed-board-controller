#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "continuous_menu.h"

void turn_on_off_continuous(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.flash.on = false;
			led1.cont.on = !led1.cont.on;
			if(led1.cont.on){
				led1.cont.hsv_cur = led1.cont.hsv;
			}
			break;
		case 1:
			led2.flash.on = false;
			led2.cont.on = !led2.cont.on;
			if(led2.cont.on){
				led2.cont.hsv_cur = led2.cont.hsv;
			}
			break;
		case 2:
			led1.flash.on = false;
			led2.flash.on = false;
			led1.cont.on = !led1.cont.on;
			led2.cont.on = led1.cont.on;
			
			if(led1.cont.on){
				led1.cont.hsv_cur = led1.cont.hsv;
				led2.cont.hsv_cur = led2.cont.hsv;
			}
			break;
	}
}

void change_continuous_color(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			break;
		case 2:
			led1.change = true;
			led2.change = true;
			led2.cont.hsv = led1.cont.hsv;
			led2.cont.rgb = led1.cont.rgb;
			led2.cont.hsv2 = led1.cont.hsv2;
			led2.cont.rgb2 = led1.cont.rgb2;
			break;
	}
	choose_colors(menu->pos, &(led1.cont), &(led2.cont));
}


void change_continuous_time(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			break;
		case 2:	
			led1.change = true;
			led2.change = true;
			led2.cont.change_time = led1.cont.change_time;
			break;
	}
	choose_time(&(led1.cont.change_time), &(led2.cont.change_time), 130);
}

void create_continuous_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	set_no_links(menu);
}

void create_continuous_color_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Changing time";
	menu->button3 = "on/off continuous";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_continuous_color;
	menu->func1 = &change_continuous_color;
	menu->func2 = &change_continuous_time;
	menu->func3 = &turn_on_off_continuous;
	set_no_links(menu);
}

