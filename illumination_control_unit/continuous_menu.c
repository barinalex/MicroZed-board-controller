#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "continuous_menu.h"

#include "led_control.h"

void set_anti_phase(menu_ *menu){
	cont_on(&led1, true);
	HSV hsv = led2.cont.hsv;
	RGB rgb = led2.cont.rgb;
	led2.cont.hsv = led2.cont.hsv2;
	led2.cont.rgb = led2.cont.rgb2;
	led2.cont.hsv2 = hsv;
	led2.cont.rgb2 = rgb;
	cont_on(&led2, true);
}

void turn_on_off_continuous(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.color_flash.on = false;
			led1.cont.on = !led1.cont.on;
			if(led1.cont.on){
				cont_on(&led1, true);
			}
			break;
		case 1:
			led2.color_flash.on = false;
			led2.cont.on = !led2.cont.on;
			if(led2.cont.on){
				cont_on(&led2, true);
			}
			break;
		case 2:
			led1.color_flash.on = false;
			led2.color_flash.on = false;
			led1.cont.on = !led1.cont.on;
			led2.cont.on = led1.cont.on;
			if(led1.cont.on){
				cont_on(&led1, true);
				cont_on(&led2, true);
				led1.cont.to_2 = (led1.cont.hsv.h < led1.cont.hsv2.h);
				led2.cont.to_2 = (led2.cont.hsv.h < led2.cont.hsv2.h);
			}
			break;
	}
}

void change_continuous_color(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.cont), &(led2.cont));
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.cont), &(led2.cont));
			break;
		case 2:
			led1.change = true;
			led2.change = true;
			HSV hsv = led2.cont.hsv;
			RGB rgb = led2.cont.rgb;
			HSV hsv2 = led2.cont.hsv2;
			RGB rgb2 = led2.cont.rgb2;
			led2.cont.hsv = led1.cont.hsv;
			led2.cont.rgb = led1.cont.rgb;
			led2.cont.hsv2 = led1.cont.hsv2;
			led2.cont.rgb2 = led1.cont.rgb2;
			if (!choose_colors(&(menu->cur_knobs), menu->pos, menu->pos, &(led1.cont), &(led2.cont))){
				led2.cont.hsv = hsv;
				led2.cont.rgb = rgb;
				led2.cont.hsv2 = hsv2;
				led2.cont.rgb2 = rgb2;
			}
			break;
	}
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
	choose_time(&(menu->cur_knobs), &(led1.cont.change_time), &(led2.cont.change_time), 140, 0);
}

void create_continuous_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->name = "Continuous";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
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
	menu->button2 = "Period";
	menu->button3 = "On/Off";
	menu->name = "Led";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_continuous_color;
	menu->func1 = &change_continuous_color;
	menu->func2 = &change_continuous_time;
	menu->func3 = &turn_on_off_continuous;
	set_no_links(menu);
}

void create_continuous_both_color_menu(menu_ *menu){
	menu->buttons_number = 5;
	menu->pos = 0;
	menu->button0 = "Color 1";
	menu->button1 = "Color 2";
	menu->button2 = "Period";
	menu->button3 = "Anti-phase";
	menu->button4 = "On/Off";
	menu->name = "Led";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_continuous_color;
	menu->func1 = &change_continuous_color;
	menu->func2 = &change_continuous_time;
	menu->func3 = &set_anti_phase;
	menu->func4 = &turn_on_off_continuous;
	set_no_links(menu);
}

