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
	//choose_color(menu->pos);
	choose_colors(0, 0, &(led1.st), &(led2.st));
}

void change_static_led2(menu_ *menu){
	led1.change = false;
	led2.change = true;
	//choose_color(menu->pos);
	choose_colors(0, 1, &(led1.st), &(led2.st));
}

void change_static_both(menu_ *menu){
	led1.change = true;
	led2.change = true;
	led2.st.hsv = led1.st.hsv;
	led2.st.rgb = led1.st.rgb;
	//choose_color(menu->pos);
	choose_colors(0, 2, &(led1.st), &(led2.st));
}

void copy_led1_to_led2(menu_ *menu){
	led2.st.hsv = led1.st.hsv;
	led2.st.rgb = led1.st.rgb;
}

void copy_led2_to_led1(menu_ *menu){
	led1.st.hsv = led2.st.hsv;
	led1.st.rgb = led2.st.rgb;
}

void create_static_menu(menu_ *menu){
	menu->buttons_number = 5;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->button3 = "Led 1 to Led 2";
	menu->button4 = "Led 2 to Led 1";
	
	menu->name = "Static";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_static_led1;
	menu->func1 = &change_static_led2;
	menu->func2 = &change_static_both;
	menu->func3 = &copy_led1_to_led2;
	menu->func4 = &copy_led2_to_led1;
	set_no_links(menu);
}
/*
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
			change_rgb_hsv(&(led1.hsv), &(led1.rgb), knobs, prev_knobs);
			rectangle_to_lcd(led1.rgb, rect_led1);
		}
		if(led2.change){
			change_rgb_hsv(&(led2.hsv), &(led2.rgb), knobs, prev_knobs);
			rectangle_to_lcd(led2.rgb, rect_led2);
		}
		prev_knobs = knobs;
		frameToLCD();
	}
}*/

