#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "continuous_menu.h"

void cont_on(LED *led, bool to_2){
		
		led->cont.hsv_cur = (to_2) ? led->cont.hsv: led->cont.hsv2;
		
		led->cont.h_to_2 = to_2;
		led->cont.s_to_2 = to_2;
		led->cont.v_to_2 = to_2;
		
		led->cont.h_decrement = (to_2 && led->cont.hsv.h < led->cont.hsv2.h) ? 1: -1;
		led->cont.s_decrement = (to_2 && led->cont.hsv.s < led->cont.hsv2.s) ? 1: -1;
		led->cont.v_decrement = (to_2 && led->cont.hsv.v < led->cont.hsv2.v) ? 1: -1;
		
		led->cont.h_last_change_time = get_cur_time_in_mlsec();
		led->cont.s_last_change_time = get_cur_time_in_mlsec();
		led->cont.v_last_change_time = get_cur_time_in_mlsec();
}

void set_anti_phase(menu_ *menu){
	cont_on(&led1, true);
	cont_on(&led2, false);
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
	choose_time(&(led1.cont.change_time), &(led2.cont.change_time), 140, 0);
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

