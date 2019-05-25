#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "color_flash_menu.h"

void turn_on_off_color_flash(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.cont.on = false;
			led1.color_flash.on = !led1.color_flash.on;
			led1.color_flash.rgb_cur = led1.color_flash.rgb;
			break;
		case 1:
			led2.cont.on = false;
			led2.color_flash.on = !led2.color_flash.on;
			led2.color_flash.rgb_cur = led2.color_flash.rgb;
			break;
		case 2:
			led1.cont.on = false;
			led2.cont.on = false;
			led1.color_flash.on = !led1.color_flash.on;
			led2.color_flash.on = led1.color_flash.on;
			led2.color_flash.to_2 = led1.color_flash.to_2;
			
			led1.color_flash.rgb_cur = led1.color_flash.rgb;
			led2.color_flash.rgb_cur = led2.color_flash.rgb;
			break;
	}
}

void change_color_flash_color(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.color_flash), &(led2.color_flash));
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.color_flash), &(led2.color_flash));
			break;
		case 2:
			led1.change = true;
			led2.change = true;			
			HSV hsv = led2.color_flash.hsv;
			RGB rgb = led2.color_flash.rgb;
			HSV hsv2 = led2.color_flash.hsv2;
			RGB rgb2 = led2.color_flash.rgb2;
			led2.color_flash.hsv = led1.color_flash.hsv;
			led2.color_flash.rgb = led1.color_flash.rgb;
			led2.color_flash.hsv2 = led1.color_flash.hsv2;
			led2.color_flash.rgb2 = led1.color_flash.rgb2;
			if (!choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.color_flash), &(led2.color_flash))){
				led2.color_flash.hsv = hsv;
				led2.color_flash.rgb = rgb;
				led2.color_flash.hsv2 = hsv2;
				led2.color_flash.rgb2 = rgb2;
			}
			break;
	}
}


void change_color_flash_time(menu_ *menu){
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
			led2.color_flash.change_time = led1.color_flash.change_time;
			break;
	}
	choose_time(&(menu->cur_knobs), &(led1.color_flash.change_time), &(led2.color_flash.change_time), 140, 0);
}

void choose_shift_color_flash(menu_ *menu){	
	led1.change = false;
	led2.change = true;
	
	choose_time(&(menu->cur_knobs), &(led1.color_flash.shift), &(led2.color_flash.shift), 180, led1.color_flash.change_time + 1);
}

void create_color_flash_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->name = "Color flash";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	set_no_links(menu);
}

void create_color_flash_led_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Period";
	menu->button3 = "on/off";
	menu->name = "Led";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_color_flash_color;
	menu->func1 = &change_color_flash_color;
	menu->func2 = &change_color_flash_time;
	menu->func3 = &turn_on_off_color_flash;
	set_no_links(menu);
}

