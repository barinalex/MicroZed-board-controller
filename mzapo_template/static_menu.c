#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "static_menu.h"

void turn_on_off_static(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			led1.color_flash.on = false;
			led1.cont.on = false;
			led1.st.on = !led1.st.on;
			break;
		case 1:
			led2.color_flash.on = false;
			led2.cont.on = false;
			led2.st.on = !led2.st.on;
			break;
		case 2:
			led1.color_flash.on = false;
			led2.color_flash.on = false;
			led1.cont.on = false;
			led2.cont.on = false;
			led1.st.on = !led1.st.on;
			led2.st.on = led1.st.on;
			break;
	}
}

void change_static_led1(menu_ *menu){
	led1.change = true;
	led2.change = false;
	choose_colors(&(menu->cur_knobs), 0, 0, &(led1.st), &(led2.st));
}

void change_static_led2(menu_ *menu){
	led1.change = false;
	led2.change = true;
	choose_colors(&(menu->cur_knobs), 0, 0, &(led1.st), &(led2.st));
}

void change_static_both(menu_ *menu){
	led1.change = true;
	led2.change = true;
	HSV hsv = led2.st.hsv;
	RGB rgb = led2.st.rgb;
	led2.st.hsv = led1.st.hsv;
	led2.st.rgb = led1.st.rgb;
	if (!choose_colors(&(menu->cur_knobs), 0, 0, &(led1.st), &(led2.st))){
		led2.st.hsv = hsv;
		led2.st.rgb = rgb;
	}
}

void copy_led1_to_led2(menu_ *menu){
	led2.st.hsv = led1.st.hsv;
	led2.st.rgb = led1.st.rgb;
	led2.st.on = led1.st.on;
}

void copy_led2_to_led1(menu_ *menu){
	led1.st.hsv = led2.st.hsv;
	led1.st.rgb = led2.st.rgb;
	led1.st.on = led2.st.on;
}

void create_static_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Led 1";
	menu->button1 = "Led 2";
	menu->button2 = "Both";
	
	menu->name = "Static";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	set_no_links(menu);
}

void create_static_led1_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Color";
	menu->button1 = "Copy to";
	menu->button2 = "On/Off";
	
	menu->name = "Led 1";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_static_led1;
	menu->func1 = &copy_led1_to_led2;
	menu->func2 = &turn_on_off_static;
	set_no_links(menu);
}

void create_static_led2_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Color";
	menu->button1 = "Copy to";
	menu->button2 = "On/Off";
	
	menu->name = "Led 2";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_static_led2;
	menu->func1 = &copy_led2_to_led1;
	menu->func2 = &turn_on_off_static;
	set_no_links(menu);
}

void create_static_both_menu(menu_ *menu){
	menu->buttons_number = 2;
	menu->pos = 0;
	menu->button0 = "Color";
	menu->button1 = "On/Off";
	
	menu->name = "Both";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &change_static_both;
	menu->func1 = &turn_on_off_static;
	set_no_links(menu);
}
