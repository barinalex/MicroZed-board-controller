#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "static_menu.h"

void change_static_led1(menu_ *menu){
	led1.change = true;
	led2.change = false;
	choose_color(menu->pos);
}

void change_static_led2(menu_ *menu){
	led1.change = false;
	led2.change = true;
	choose_color(menu->pos);
}

void change_static_both(menu_ *menu){
	led1.change = true;
	led2.change = true;
	led2.hsv = led1.hsv;
	led2.rgb = led1.rgb;
	choose_color(menu->pos);
}

void copy_led1_to_led2(menu_ *menu){
	led2.hsv = led1.hsv;
	led2.rgb = led1.rgb;
}

void copy_led2_to_led1(menu_ *menu){
	led1.hsv = led2.hsv;
	led1.rgb = led2.rgb;
}

void create_static_menu(menu_ *menu){
	menu->buttons_number = 5;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->button3 = "Led 1 to Led 2";
	menu->button4 = "Led 2 to Led 1";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_static_led1;
	menu->func1 = &change_static_led2;
	menu->func2 = &change_static_both;
	menu->func3 = &copy_led1_to_led2;
	menu->func4 = &copy_led2_to_led1;
	set_no_links(menu);
}

void choose_color(int menu_pos){
	rect_ rect_led1 = set_rect(0);
	rect_ rect_led2 = set_rect(1);
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
			if(led1.change){
				led1.color_flash.on = false;
				led1.cont.on = false;
			}
			if(led2.change){
				led2.color_flash.on = false;
				led2.cont.on = false;	
			}
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			led1.hsv.h = change(led1.hsv.h, knobs.r_knob, prev_knobs.r_knob, 360); 
			led1.hsv.s = change(led1.hsv.s, knobs.g_knob, prev_knobs.g_knob, 100); 
			led1.hsv.v = change(led1.hsv.v, knobs.b_knob, prev_knobs.b_knob, 100);
			led1.rgb = HsvToRgb(led1.hsv);
			printf("%d %d %d\n", led1.hsv.h, led1.hsv.s, led1.hsv.v);
			printf("%d %d %d\n", led1.rgb.r, led1.rgb.g, led1.rgb.b);
			if(menu_pos == 0 || menu_pos == 2) rectangle_to_lcd(led1.rgb, rect_led1);
		}
		if(led2.change){
			led2.hsv.h = change(led2.hsv.h, knobs.r_knob, prev_knobs.r_knob, 360); 
			led2.hsv.s = change(led2.hsv.s, knobs.g_knob, prev_knobs.g_knob, 100); 
			led2.hsv.v = change(led2.hsv.v, knobs.b_knob, prev_knobs.b_knob, 100);
			led2.rgb = HsvToRgb(led2.hsv);
			printf("%d %d %d\n", led2.hsv.h, led2.hsv.s, led2.hsv.v);
			printf("%d %d %d\n", led2.rgb.r, led2.rgb.g, led2.rgb.b);
			if(menu_pos == 1 || menu_pos == 2) rectangle_to_lcd(led2.rgb, rect_led2);
		}
		prev_knobs = knobs;
		frameToLCD();
	}
	led1.change = false;
	led2.change = false;
}

