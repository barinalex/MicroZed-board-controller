#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "static_menu.h"
#include "flash_menu.h"

void create_main_menu(menu_ *menu);
void create_desk_menu(menu_ *menu);
void change_text_size(menu_ *menu);

void menu(menu_ menu){
	get_knobs_data(&(menu.prev_knobs));
	while(true){
		//printf("%x\n", *knobs_base);
		get_knobs_data(&(menu.cur_knobs));
		menu.pos = change_menu_pos(menu.buttons_number, menu.cur_knobs.b_knob, menu.prev_knobs.b_knob, menu.pos);
		menu.prev_knobs = menu.cur_knobs;
		draw_menu(menu);
		if(menu.cur_knobs.r_button && menu.prev != NULL){
			go_prev_menu(&menu);
			continue;
		}
		if(menu.cur_knobs.b_button){
			switch(menu.pos){
				case 0:
					menu.func0(&menu);
					break;
				case 1:
					menu.func1(&menu);
					break;
				case 2:
					menu.func2(&menu);
					break;
				case 3:
					menu.func3(&menu);
					break;
				case 4:
					menu.func4(&menu);
					break;
			}
		}
	}
}

void draw_menu(menu_ menu){
	if(menu.buttons_number > 0) strToFrame(menu.button0, 30, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 1) strToFrame(menu.button1, 80, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 2) strToFrame(menu.button2, 130, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 3) strToFrame(menu.button3, 180, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 4) strToFrame(menu.button4, 230, 50, WHITE, BLACK, big_text);
	switch(menu.pos){
		case 0:
			strToFrame(menu.button0, 30, 50, BLUE, BLACK, big_text);
			break;
		case 1:
			strToFrame(menu.button1, 80, 50, BLUE, BLACK, big_text);
			break;
		case 2:
			strToFrame(menu.button2, 130, 50, BLUE, BLACK, big_text);
			break;
		case 3:
			strToFrame(menu.button3, 180, 50, BLUE, BLACK, big_text);
			break;
		case 4:
			strToFrame(menu.button4, 230, 50, BLUE, BLACK, big_text);
			break;
	}
	strToFrame(menu.comment, 280, 50, BLUE, BLACK, big_text);
	frameToLCD();
}

void choose_option(menu_ *menu){
	switch(menu->pos){
		case 0:
			led1.change = true;
			led2.change = false;
			choose_color(menu->pos);
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			choose_color(menu->pos);
			break;
		case 2:
			led1.change = true;
			led2.change = true;
			led2.hsv = led1.hsv;
			led2.rgb = led1.rgb;
			choose_color(menu->pos);
			break;
		case 3:
			led2.hsv = led1.hsv;
			led2.rgb = led1.rgb;
			to_led_(led1, led2);
			break;
		case 4:
			led1.hsv = led2.hsv;
			led1.rgb = led2.rgb;
			to_led_(led1, led2);
			break;
	}
}

void choose_color(int menu_pos){
	rect_ rect_led1 = set_rect(0);
	rect_ rect_led2 = set_rect(1);
	LED new_led1 = led1;
	LED new_led2 = led2;
	
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	while(true){
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			usleep(DELAY);
			to_led_(led1, led2);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			led1 = new_led1;
			led2 = new_led2;
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(led1.change){
			new_led1.hsv.h = change(new_led1.hsv.h, knobs.r_knob, prev_knobs.r_knob, 360); 
			new_led1.hsv.s = change(new_led1.hsv.s, knobs.g_knob, prev_knobs.g_knob, 100); 
			new_led1.hsv.v = change(new_led1.hsv.v, knobs.b_knob, prev_knobs.b_knob, 100);
			new_led1.rgb = HsvToRgb(new_led1.hsv);
			printf("%d %d %d\n", new_led1.hsv.h, new_led1.hsv.s, new_led1.hsv.v);
			printf("%d %d %d\n", new_led1.rgb.r, new_led1.rgb.g, new_led1.rgb.b);
			if(menu_pos == 0 || menu_pos == 2) rectangle_to_lcd(new_led1.rgb, rect_led1);
		}
		if(led2.change){
			new_led2.hsv.h = change(new_led2.hsv.h, knobs.r_knob, prev_knobs.r_knob, 360); 
			new_led2.hsv.s = change(new_led2.hsv.s, knobs.g_knob, prev_knobs.g_knob, 100); 
			new_led2.hsv.v = change(new_led2.hsv.v, knobs.b_knob, prev_knobs.b_knob, 100);
			new_led2.rgb = HsvToRgb(new_led2.hsv);
			printf("%d %d %d\n", new_led2.hsv.h, new_led2.hsv.s, new_led2.hsv.v);
			printf("%d %d %d\n", new_led2.rgb.r, new_led2.rgb.g, new_led2.rgb.b);
			if(menu_pos == 1 || menu_pos == 2) rectangle_to_lcd(new_led2.rgb, rect_led2);
		}
		prev_knobs = knobs;
		to_led_(new_led1, new_led2);
		frameToLCD();
	}
	led1.change = false;
	led2.change = false;
}

void go_next_menu(menu_ *menu){
	switch(menu->pos){
		case 0:
			if(menu->next0 != NULL){
				usleep(DELAY);
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next0);
			}
			break;
		case 1:
			if(menu->next1 != NULL){
				usleep(DELAY);
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next1);
			}
			break;
		case 2:
			if(menu->next2 != NULL){
				usleep(DELAY);
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next2);
			}
			break;
		case 3:
			if(menu->next3 != NULL){
				usleep(DELAY);
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next3);
			}
			break;
		case 4:
			if(menu->next4 != NULL){
				usleep(DELAY);
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next4);
			}
			break;				
	}
	draw_menu(*menu);
}

void go_prev_menu(menu_ *menu){
	if(menu->prev != NULL){
		usleep(DELAY);
		menu->pos = 0;
		*(menu) = *(menu->prev);
		get_knobs_data(&(menu->prev_knobs));
		clear_screen();
		draw_menu(*menu);
	}
}

void change_text_size(menu_ *menu){
	big_text = !big_text;
}

void *create_menu(void *vargp){
	menu_ main_menu;
	create_main_menu(&main_menu);
	
	menu_ desk_menu;
	create_desk_menu(&desk_menu);
	
	main_menu.next0 = &desk_menu;
	desk_menu.prev = &main_menu;
		
	menu_ static_menu;
	create_static_menu(&static_menu);
	
	desk_menu.next0 = &static_menu;
	static_menu.prev = &desk_menu;
	
	menu_ flash_menu;
	create_flash_menu(&flash_menu);
	
	desk_menu.next2 = &flash_menu;
	flash_menu.prev = &desk_menu;
	
	menu(main_menu);
	return NULL;
}

void set_no_links(menu_ *menu){
	menu->prev = NULL;
	menu->next0 = NULL;
	menu->next1 = NULL;
	menu->next2 = NULL;
	menu->next3 = NULL;
	menu->next4 = NULL;
}

void create_main_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "This";
	menu->button1 = "Choose another";
	menu->button2 = "Change Text Size";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &go_next_menu;
	menu->func2 = &change_text_size;
	set_no_links(menu);
}

void create_desk_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Static";
	menu->button1 = "Continuous";
	menu->button2 = "Flashing";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	set_no_links(menu);
}
