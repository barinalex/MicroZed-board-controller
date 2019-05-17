#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "static_menu.h"
#include "color_flash_menu.h"
#include "flash_menu.h"
#include "continuous_menu.h"

void create_main_menu(menu_ *menu);
void create_desk_menu(menu_ *menu);
void change_text_size(menu_ *menu);

void choose_desk(menu_ *menu){
}

void menu(menu_ menu){
	get_knobs_data(&(menu.prev_knobs));
	while(true){
		//printf("%x\n", *knobs_mem_base);
		get_knobs_data(&(menu.cur_knobs));
		menu.pos = change_menu_pos(menu.buttons_number, menu.cur_knobs.b_knob, menu.prev_knobs.b_knob, menu.pos);
		menu.prev_knobs = menu.cur_knobs;
		draw_menu(menu);
		if(menu.cur_knobs.r_button && menu.prev != NULL){
			go_prev_menu(&menu);
			continue;
		}
		if(menu.cur_knobs.b_button){
			usleep(DELAY);
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
			get_knobs_data(&(menu.prev_knobs));
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


void go_next_menu(menu_ *menu){
	switch(menu->pos){
		case 0:
			if(menu->next0 != NULL){
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next0);
				menu->prev_menu_pos = 0;
			}
			break;
		case 1:
			if(menu->next1 != NULL){
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next1);
				menu->prev_menu_pos = 1;
			}
			break;
		case 2:
			if(menu->next2 != NULL){
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next2);
				menu->prev_menu_pos = 2;
			}
			break;
		case 3:
			if(menu->next3 != NULL){
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next3);
				menu->prev_menu_pos = 3;
			}
			break;
		case 4:
			if(menu->next4 != NULL){
				clear_screen();
				menu->pos = 0;
				*menu = *(menu->next4);
				menu->prev_menu_pos = 4;
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
	clear_screen();
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
	
	menu_ continuous_menu;
	create_continuous_menu(&continuous_menu);
	desk_menu.next1 = &continuous_menu;
	continuous_menu.prev = &desk_menu;
	
	menu_ continuous_color_menu_led1;
	menu_ continuous_color_menu_led2;
	menu_ continuous_color_menu_both;
	create_continuous_color_menu(&continuous_color_menu_led1);
	create_continuous_color_menu(&continuous_color_menu_led2);
	create_continuous_color_menu(&continuous_color_menu_both);
	
	continuous_menu.next0 = &continuous_color_menu_led1;
	continuous_menu.next1 = &continuous_color_menu_led2;
	continuous_menu.next2 = &continuous_color_menu_both;
	continuous_color_menu_led1.prev = &continuous_menu;
	continuous_color_menu_led2.prev = &continuous_menu;
	continuous_color_menu_both.prev = &continuous_menu;
	
	menu_ color_flash_menu;
	create_color_flash_menu(&color_flash_menu);
	desk_menu.next2 = &color_flash_menu;
	color_flash_menu.prev = &desk_menu;
	
	menu_ color_flash_menu_led1;
	menu_ color_flash_menu_led2;
	menu_ color_flash_menu_both;
	create_color_flash_led_menu(&color_flash_menu_led1);
	create_color_flash_led_menu(&color_flash_menu_led2);
	create_color_flash_led_menu(&color_flash_menu_both);
	
	color_flash_menu.next0 = &color_flash_menu_led1;
	color_flash_menu.next1 = &color_flash_menu_led2;
	color_flash_menu.next2 = &color_flash_menu_both;
	color_flash_menu_led1.prev = &color_flash_menu;
	color_flash_menu_led2.prev = &color_flash_menu;
	color_flash_menu_both.prev = &color_flash_menu;
	
	menu_ flash_menu;
	create_flash_menu(&flash_menu);
	desk_menu.next3 = &flash_menu;
	flash_menu.prev = &desk_menu;
	
	menu_ flash_menu_led1;
	menu_ flash_menu_led2;
	menu_ flash_menu_both;
	create_flash_menu(&flash_menu_led1);
	create_flash_menu(&flash_menu_led2);
	create_flash_menu(&flash_menu_both);
	
	flash_menu.next0 = &flash_menu_led1;
	flash_menu.next1 = &flash_menu_led2;
	flash_menu.next2 = &flash_menu_both;
	flash_menu_led1.prev = &flash_menu;
	flash_menu_led2.prev = &flash_menu;
	flash_menu_both.prev = &flash_menu;

	
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
	menu->func1 = &choose_desk;
	menu->func2 = &change_text_size;
	set_no_links(menu);
}

void create_desk_menu(menu_ *menu){
	menu->buttons_number = 4;
	menu->pos = 0;
	menu->button0 = "Static";
	menu->button1 = "Continuous";
	menu->button2 = "Color flash";
	menu->button3 = "Flash";
	menu->comment = "Exit: red. Choose: blue";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	menu->func3 = &go_next_menu;
	set_no_links(menu);
}
