#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "flash_menu.h"

void turn_on_off_flash(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.cont.on = false;
			led1.flash.on = !led1.flash.on;
			break;
		case 1:
			led1.cont.on = false;
			led2.flash.on = !led2.flash.on;
			break;
		case 2:
			led1.cont.on = false;
			led2.cont.on = false;
			led1.flash.on = !led1.flash.on;
			led2.flash.on = led1.flash.on;
			led2.flash.to_2 = led1.flash.to_2;
			break;
	}
}

void change_flash_color(menu_ *menu){
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
			led2.flash.hsv = led1.flash.hsv;
			led2.flash.rgb = led1.flash.rgb;
			led2.flash.hsv2 = led1.flash.hsv2;
			led2.flash.rgb2 = led1.flash.rgb2;
			break;
	}
	choose_colors(menu->pos, &(led1.flash), &(led2.flash));
}


void change_flash_time(menu_ *menu){
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
			led2.flash.change_time = led1.flash.change_time;
			break;
	}
	choose_time(&(led1.flash.change_time), &(led2.flash.change_time), 130);
}

void choose_shift_flash(menu_ *menu){	
	led1.change = false;
	led2.change = true;
	
	choose_time(&(led1.flash.shift), &(led2.flash.shift), 180);
}

void create_flash_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->button3 = "Shift";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	menu->func3 = &choose_shift_flash;
	set_no_links(menu);
}

void create_flash_led_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Shift";
	menu->button3 = "on/off flashing";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_flash_color;
	menu->func1 = &change_flash_color;
	menu->func2 = &change_flash_time;
	menu->func3 = &turn_on_off_flash;
	set_no_links(menu);
}

