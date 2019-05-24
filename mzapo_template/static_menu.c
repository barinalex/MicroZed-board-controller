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
	choose_colors(0, 0, &(led1.st), &(led2.st));
}

void change_static_led2(menu_ *menu){
	led1.change = false;
	led2.change = true;
	choose_colors(0, 1, &(led1.st), &(led2.st));
}

void change_static_both(menu_ *menu){
	led1.change = true;
	led2.change = true;
	led2.st.hsv = led1.st.hsv;
	led2.st.rgb = led1.st.rgb;
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

