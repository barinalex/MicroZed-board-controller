#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "flash_menu.h"

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
			led2.hsv = led1.hsv;
			led2.rgb = led1.rgb;
			led2.hsv2 = led1.hsv2;
			led2.rgb2 = led1.rgb2;
			break;
	}
	change_flash_colors(menu->pos);
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
	menu->func3 = &choose_time;
	set_no_links(menu);
}

void create_flash_color_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Shift";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_flash;
	menu->func1 = &change_flash;
	menu->func2 = &choose_time;
	set_no_links(menu);
}

void change_flash_colors(int menu_pos){
	rect_ rect_led;
	HSV* led1hsv, *led2hsv;
	RGB* led1rgb, *led2rgb;
	switch(menu_pos){
		case 0:
			rect_led = set_rect(0);
			led1hsv = &(led1.hsv);
			led1rgb = &(led1.rgb);
			led2hsv = &(led2.hsv);
			led2rgb = &(led2.rgb);
			break;
		case 1:
			rect_led = set_rect(1);
			led1hsv = &(led1.hsv2);
			led1rgb = &(led1.rgb2);
			led2hsv = &(led2.hsv2);
			led2rgb = &(led2.rgb2);
			break;
	}
	LED saved_led1 = led1;
	LED saved_led2 = led2;
	
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	while(true){
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			led1 = saved_led1;
			led2 = saved_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			led1.flashing = false;
			led2.flashing = false;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			led1hsv->h = change(led1hsv->h, knobs.r_knob, prev_knobs.r_knob, 360); 
			led1hsv->s = change(led1hsv->s, knobs.g_knob, prev_knobs.g_knob, 100); 
			led1hsv->v = change(led1hsv->v, knobs.b_knob, prev_knobs.b_knob, 100);
			*led1rgb = HsvToRgb(*led1hsv);
			printf("%d %d %d\n", led1hsv->h, led1hsv->s, led1hsv->v);
			printf("%d %d %d\n", led1rgb->r, led1rgb->g, led1rgb->b);
			rectangle_to_lcd(*led1rgb, rect_led);
		}
		if(led2.change){
			led2hsv->h = change(led2hsv->h, knobs.r_knob, prev_knobs.r_knob, 360); 
			led2hsv->s = change(led2hsv->s, knobs.g_knob, prev_knobs.g_knob, 100); 
			led2hsv->v = change(led2hsv->v, knobs.b_knob, prev_knobs.b_knob, 100);
			*led2rgb = HsvToRgb(*led2hsv);
			printf("%d %d %d\n", led2hsv->h, led2hsv->s, led2hsv->v);
			printf("%d %d %d\n", led2rgb->r, led2rgb->g, led2rgb->b);
			rectangle_to_lcd(*led2rgb, rect_led);
		}
		prev_knobs = knobs;
		frameToLCD();
	}
	led1.change = false;
	led2.change = false;
}

