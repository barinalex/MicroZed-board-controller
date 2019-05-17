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
			led1.flash.on = !led1.flash.on;
			led1.flash.last_change_time = get_cur_time_in_mlsec();
			led1.illuminate = true;
			break;
		case 1:
			led2.flash.on = !led2.flash.on;
			led2.flash.last_change_time = get_cur_time_in_mlsec();
			led2.illuminate = true;
			break;
		case 2:
			led1.flash.on = !led1.flash.on;
			led2.flash.on = led1.flash.on;
			led2.flash.to_2 = led1.flash.to_2;
			led1.flash.last_change_time = get_cur_time_in_mlsec();
			led2.flash.last_change_time = get_cur_time_in_mlsec();
			led1.illuminate = true;
			led2.illuminate = true;
			break;
	}
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
			led2.flash.illumination_time = led1.flash.illumination_time;
			led2.flash.extinction_time = led1.flash.extinction_time;
			break;
	}
	switch(menu->pos){
		case 0:
			choose_time(&(led1.flash.illumination_time), &(led2.flash.illumination_time), 30);
			break;
		case 1:
			choose_time(&(led1.flash.extinction_time), &(led2.flash.extinction_time), 80);
			break;
	}
}

void choose_shift_flash(menu_ *menu){
	switch(menu->pos){
		case 3:
			led1.change = true;
			led2.change = false;
			choose_time(&(led1.flash.shift), &(led2.flash.shift), 180);
			break;
		case 4:
			led1.change = false;
			led2.change = true;
			choose_time(&(led1.flash.shift), &(led2.flash.shift), 230);
			break;
	}
}

void create_flash_menu(menu_ *menu){
	menu->buttons_number = 5;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->button3 = "Shift 1 to 2";
	menu->button4 = "Shift 2 to 1";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	menu->func3 = &choose_shift_flash;
	menu->func4 = &choose_shift_flash;
	set_no_links(menu);
}

void create_flash_led_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Illumination";
	menu->button1 = "Extinction";
	menu->button2 = "on/off";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_flash_time;
	menu->func1 = &change_flash_time;
	menu->func2 = &turn_on_off_flash;
	set_no_links(menu);
}

