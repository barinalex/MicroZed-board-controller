#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "color_flash_menu.h"

void turn_on_off(menu_ *menu){
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

void change_flash(menu_ *menu){
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
	change_colors(menu->pos, &(led1.flash), &(led2.flash));
}

void choose_time(menu_ *menu){
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
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
	
	LED saved_led1 = led1;
	LED saved_led2 = led2;
	while(true){
		printf("%x\n", *knobs_mem_base);
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			led1 = saved_led1;
			led2 = saved_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			led1.flash.change_time = change_int(led1.flash.change_time, knobs.b_knob, prev_knobs.b_knob, 100);
			int_to_frame(led1.flash.change_time, 130, 240, WHITE, BLACK, big_text);
		}
		if(led2.change){
			led2.flash.change_time = change_int(led2.flash.change_time, knobs.b_knob, prev_knobs.b_knob, 100);
			int_to_frame(led2.flash.change_time, 130, 240, WHITE, BLACK, big_text);
		}
		frameToLCD();
		prev_knobs = knobs;
	}
}

void choose_shift(menu_ *menu){
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	LED saved_led2 = led2;
	while(true){
		printf("%x\n", *knobs_mem_base);
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			led2 = saved_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		led2.flash.shift = change_int(led2.flash.shift, knobs.b_knob, prev_knobs.b_knob, 100);
		int_to_frame(led2.flash.shift, 180, 240, WHITE, BLACK, big_text);
		frameToLCD();
		prev_knobs = knobs;
	}
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
	menu->func3 = &choose_shift;
	set_no_links(menu);
}

void create_flash_color_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Shift";
	menu->button3 = "on/off flashing";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_flash;
	menu->func1 = &change_flash;
	menu->func2 = &choose_time;
	menu->func3 = &turn_on_off;
	set_no_links(menu);
}

