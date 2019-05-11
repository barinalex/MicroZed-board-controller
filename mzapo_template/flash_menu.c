#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "flash_menu.h"

void change_flash_led1(menu_ *menu){
	led1.change = true;
	led2.change = false;
	choose_color(menu->pos);
}

void change_flash_led2(menu_ *menu){
	led1.change = false;
	led2.change = true;
	choose_color(menu->pos);
}

void change_flash_both(menu_ *menu){
	led1.change = true;
	led2.change = true;
	led2.hsv = led1.hsv;
	led2.rgb = led1.rgb;
	choose_color(menu->pos);
}

void choose_time(menu_ *menu){
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	int time = 0;
	char str[12];
	while(true){
		printf("%x\n", *knobs_mem_base);
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		time = change_int(time, knobs.b_knob, prev_knobs.b_knob);
		sprintf(str, "%d", time);
		strToFrame(str, 130, 240, WHITE, BLACK, big_text);
		frameToLCD();
		prev_knobs = knobs;
	}
}

/*
void choose_flash(){
	clear_screen();
	bool choosing_color = false;	
	knobs_ knobs;
	get_knobs_data(&knobs);
	uint8_t prev_blue_knob_value = knobs.b_knob;
	
	menu_ menu;
	menu.buttons_number = 3;
	menu.pos = 0;
	menu.button0 = "Color 1";
	menu.button1 = "Color 2";
	menu.button2 = "Shift";
	menu.comment = "Exit: red. Choose: blue";
	
	while(true){
		printf("%x\n", *knobs_mem_base);
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu.pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_blue_knob_value, menu.pos);
		}else{
			flash_pos_chosed(menu.pos);
			choosing_color = !choosing_color;
		}
		prev_blue_knob_value = knobs.b_knob;
		draw_menu(menu);
	}
}*/

void create_flash_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->button3 = "Shift";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_flash_led1;
	menu->func1 = &change_flash_led2;
	menu->func2 = &change_flash_both;
	menu->func3 = &choose_time;
	set_no_links(menu);
}

