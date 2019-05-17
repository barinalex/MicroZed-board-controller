#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "continuous_menu.h"

void turn_on_off_continuous(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.flash.on = false;
			led1.cont.on = !led1.cont.on;
			if(led1.cont.on){
				led1.cont.hsv_cur = led1.cont.hsv;
			}
			break;
		case 1:
			led2.flash.on = false;
			led2.cont.on = !led2.cont.on;
			if(led2.cont.on){
				led2.cont.hsv_cur = led2.cont.hsv;
			}
			break;
		case 2:
			led1.flash.on = false;
			led2.flash.on = false;
			led1.cont.on = !led1.cont.on;
			led2.cont.on = led1.cont.on;
			
			if(led1.cont.on){
				led1.cont.hsv_cur = led1.cont.hsv;
				led2.cont.hsv_cur = led2.cont.hsv;
			}
			break;
	}
}

void change_continuous(menu_ *menu){
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
	change_colors(menu->pos, &(led1.cont), &(led2.cont));
}

void choose_time_continuous(menu_ *menu){
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
			led2.cont.change_time = led1.cont.change_time;
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
			led1.cont.change_time = change_int(led1.cont.change_time, knobs.b_knob, prev_knobs.b_knob, 1000);
			int_to_frame(led1.cont.change_time, 130, 240, WHITE, BLACK, big_text);
		}
		if(led2.change){
			led2.cont.change_time = change_int(led2.cont.change_time, knobs.b_knob, prev_knobs.b_knob, 1000);
			int_to_frame(led2.cont.change_time, 130, 240, WHITE, BLACK, big_text);
		}
		frameToLCD();
		prev_knobs = knobs;
	}
}

void create_continuous_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	menu->comment = "Exit: red. Choose: blue";
	
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
	menu->button2 = "Changing time";
	menu->button3 = "on/off continuous";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &change_continuous;
	menu->func1 = &change_continuous;
	menu->func2 = &choose_time_continuous;
	menu->func3 = &turn_on_off_continuous;
	set_no_links(menu);
}

