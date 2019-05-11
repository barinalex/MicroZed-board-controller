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
	to_led_(led1, led2);
}

void copy_led2_to_led1(menu_ *menu){
	led1.hsv = led2.hsv;
	led1.rgb = led2.rgb;
	to_led_(led1, led2);
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

