#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "static_menu.h"
#include "color_flash_menu.h"
#include "flash_menu.h"
#include "continuous_menu.h"
#include "connection_menu.h"
#include "connection.h"

int special_text_color = BLUE;
rect_ top_comment;
rect_ bottom_comment;

void create_main_menu(menu_ *menu);
void create_desk_menu(menu_ *menu);
void change_text_size(menu_ *menu);

void set_rects(rect_ *top, rect_ *bottom){
	top->top = 0;
	top->bottom = 45;
	top->left = 0;
	top->right = 480;
	
	bottom->top = 275;
	bottom->bottom = 320;
	bottom->left = 0;
	bottom->right = 480;
}

void menu(menu_ menu){
	clear_screen();
	get_knobs_data(&(menu.prev_knobs));
	bool jumped = false;
	uint8_t prev_b_knob = menu.prev_knobs.b_knob;
	set_rects(&top_comment, &bottom_comment);
	//rect_to_lcd(GREEN, top_comment);
	//rect_to_lcd(GREEN, bottom_comment);
	while(true){
		//printf("%x\n", *knobs_mem_base);
		get_knobs_data(&(menu.cur_knobs));
		actualize_buttons_state(&(menu.cur_knobs));
		menu.pos = change_menu_pos(menu.buttons_number, menu.cur_knobs.b_knob, &(prev_b_knob), menu.pos);
		menu.prev_knobs = menu.cur_knobs;
		draw_menu(menu);
		if((menu.cur_knobs.r_button && !menu.cur_knobs.r_pressed) && menu.prev != NULL){
			menu.cur_knobs.r_pressed = true;
			prev_b_knob = menu.prev_knobs.b_knob;
			go_prev_menu(&menu);
			continue;
		}
		if(menu.cur_knobs.b_button && !menu.cur_knobs.b_pressed){
			menu.cur_knobs.b_pressed = true;
			special_text_color = GREEN;
			draw_menu(menu);
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
			special_text_color = BLUE;
			get_knobs_data(&(menu.prev_knobs));
			prev_b_knob = menu.prev_knobs.b_knob;
		}
		if(!jumped && ((nw_state.receiving && nw_state.connected && !nw_state.copy) || (nw_state.sending && nw_state.connected && !nw_state.copy))){
			jumped = true;
			go_desk_menu(&menu);
			get_knobs_data(&(menu.prev_knobs));
			prev_b_knob = menu.prev_knobs.b_knob;
		}
		else if(jumped && !nw_state.connected){
			jumped = false;
		}
	}
}

void draw_menu(menu_ menu){
	strToFrame(menu.name, 5, 180, BLACK, WHITE, big_text);
	if(menu.buttons_number > 0) strToFrame(menu.button0, 60, 30, WHITE, BLACK, big_text);
	if(menu.buttons_number > 1) strToFrame(menu.button1, 100, 30, WHITE, BLACK, big_text);
	if(menu.buttons_number > 2) strToFrame(menu.button2, 140, 30, WHITE, BLACK, big_text);
	if(menu.buttons_number > 3) strToFrame(menu.button3, 180, 30, WHITE, BLACK, big_text);
	if(menu.buttons_number > 4) strToFrame(menu.button4, 220, 30, WHITE, BLACK, big_text);
	switch(menu.pos){
		case 0:
			strToFrame(menu.button0, 60, 30, special_text_color, BLACK, big_text);
			break;
		case 1:
			strToFrame(menu.button1, 100, 30, special_text_color, BLACK, big_text);
			break;
		case 2:
			strToFrame(menu.button2, 140, 30, special_text_color, BLACK, big_text);
			break;
		case 3:
			strToFrame(menu.button3, 180, 30, special_text_color, BLACK, big_text);
			break;
		case 4:
			strToFrame(menu.button4, 220, 30, special_text_color, BLACK, big_text);
			break;
	}
	strToFrame(menu.comment, 285, 30, RED, WHITE, big_text);
	strToFrame(menu.comment2, 285, 350, BLUE, WHITE, big_text);
	frameToLCD();
}


void go_desk_menu(menu_ *menu){
	clear_screen();
	if(menu->desk_menu != NULL){
		*menu = *(menu->desk_menu);
	}
	menu->pos = 0;
}

void go_next_menu(menu_ *menu){
	knobs_ cur_knobs = menu->cur_knobs;
	if(menu->buttons_number != 0){
		switch(menu->pos){
			case 0:
				if(menu->next0 != NULL){
					clear_screen();
					*menu = *(menu->next0);
					menu->cur_knobs = cur_knobs;
					menu->prev_menu_pos = 0;
				}
				break;
			case 1:
				if(menu->next1 != NULL){
					clear_screen();
					*menu = *(menu->next1);
					menu->cur_knobs = cur_knobs;
					menu->prev_menu_pos = 1;
				}
				break;
			case 2:
				if(menu->next2 != NULL){
					clear_screen();
					*menu = *(menu->next2);
					menu->cur_knobs = cur_knobs;
					menu->prev_menu_pos = 2;
				}
				break;
			case 3:
				if(menu->next3 != NULL){
					clear_screen();
					*menu = *(menu->next3);
					menu->cur_knobs = cur_knobs;
					menu->prev_menu_pos = 3;
				}
				break;
			case 4:
				if(menu->next4 != NULL){
					clear_screen();
					*menu = *(menu->next4);
					menu->cur_knobs = cur_knobs;
					menu->prev_menu_pos = 4;
				}
				break;				
		}
	}
	draw_menu(*menu);
}

void go_prev_menu(menu_ *menu){
	if(menu->prev != NULL){
		knobs_ cur_knobs = menu->cur_knobs;
		int pos = menu->prev_menu_pos;
		menu->pos = 0;
		*(menu) = *(menu->prev);
		menu->pos = pos;
		menu->cur_knobs = cur_knobs;
		get_knobs_data(&(menu->prev_knobs));
		clear_screen();
		draw_menu(*menu);
	}
}

void change_text_size(menu_ *menu){
	clear_screen();
	big_text = !big_text;
}

void set_no_links(menu_ *menu){
	menu->prev = NULL;
	menu->next0 = NULL;
	menu->next1 = NULL;
	menu->next2 = NULL;
	menu->next3 = NULL;
	menu->next4 = NULL;
	menu->desk_menu = NULL;
}

void choose_connection(menu_ *menu){
	if(!(nw_state.connected && nw_state.receiving)){
		for(int i = 0; i < 5; ++i){
			strcpy(nw_state.ip_addr[i], "");
			nw_state.ready[i] = false;
		}
		go_next_menu(menu);
		//menu->buttons_number = 2;
	}
}

void create_main_menu(menu_ *menu){
	menu->buttons_number = 3;
	menu->pos = 0;
	menu->button0 = "Effects";
	menu->button1 = "Network";
	menu->button2 = "Text size";
	menu->name = "Main";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &choose_connection;
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
	menu->name = "Desk";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	menu->func3 = &go_next_menu;
	set_no_links(menu);
}

void *create_menu(void *vargp){
	menu_ main_menu;
	create_main_menu(&main_menu);
	
	menu_ desk_menu;
	create_desk_menu(&desk_menu);
	main_menu.next0 = &desk_menu;
	main_menu.desk_menu = &desk_menu;
	desk_menu.prev = &main_menu;
		
	menu_ static_menu;
	create_static_menu(&static_menu);
	desk_menu.next0 = &static_menu;
	static_menu.prev = &desk_menu;
	static_menu.desk_menu = &desk_menu;
	
	menu_ static_menu_led1;
	menu_ static_menu_led2;
	menu_ static_menu_both;
	create_static_led1_menu(&static_menu_led1);
	create_static_led2_menu(&static_menu_led2);
	create_static_both_menu(&static_menu_both);
	static_menu.next0 = &static_menu_led1;
	static_menu.next1 = &static_menu_led2;
	static_menu.next2 = &static_menu_both;
	static_menu_led1.prev = &static_menu;
	static_menu_led2.prev = &static_menu;
	static_menu_both.prev = &static_menu;
	
	menu_ continuous_menu;
	create_continuous_menu(&continuous_menu);
	desk_menu.next1 = &continuous_menu;
	continuous_menu.prev = &desk_menu;
	continuous_menu.desk_menu = &desk_menu;
	
	menu_ continuous_color_menu_led1;
	menu_ continuous_color_menu_led2;
	menu_ continuous_color_menu_both;
	create_continuous_color_menu(&continuous_color_menu_led1);
	create_continuous_color_menu(&continuous_color_menu_led2);
	create_continuous_both_color_menu(&continuous_color_menu_both);
	
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
	color_flash_menu.desk_menu = &desk_menu;
	
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
	flash_menu.desk_menu = &desk_menu;
	
	menu_ flash_menu_led1;
	menu_ flash_menu_led2;
	menu_ flash_menu_both;
	create_flash_led_menu(&flash_menu_led1);
	create_flash_led_menu(&flash_menu_led2);
	create_flash_led_menu(&flash_menu_both);
	
	flash_menu.next0 = &flash_menu_led1;
	flash_menu.next1 = &flash_menu_led2;
	flash_menu.next2 = &flash_menu_both;
	flash_menu_led1.prev = &flash_menu;
	flash_menu_led2.prev = &flash_menu;
	flash_menu_both.prev = &flash_menu;
	
	
	menu_ connection_menu;
	create_connection_menu(&connection_menu);
	main_menu.next1 = &connection_menu;
	connection_menu.prev = &main_menu;
	connection_menu.desk_menu = &desk_menu;
	
	menu_ ip0_menu;
	create_ip_menu(&ip0_menu);
	connection_menu.next0 = &ip0_menu;
	ip0_menu.prev = &connection_menu;
	ip0_menu.desk_menu = &desk_menu;
	menu_ ip1_menu;
	create_ip_menu(&ip1_menu);
	connection_menu.next1 = &ip1_menu;
	ip1_menu.prev = &connection_menu;
	ip1_menu.desk_menu = &desk_menu;
	menu_ ip2_menu;
	create_ip_menu(&ip2_menu);
	connection_menu.next2 = &ip2_menu;
	ip2_menu.prev = &connection_menu;
	ip2_menu.desk_menu = &desk_menu;
	menu_ ip3_menu;
	create_ip_menu(&ip3_menu);
	connection_menu.next3 = &ip3_menu;
	ip3_menu.prev = &connection_menu;
	ip3_menu.desk_menu = &desk_menu;
	menu_ ip4_menu;
	create_ip_menu(&ip4_menu);
	connection_menu.next4 = &ip4_menu;
	ip4_menu.prev = &connection_menu;
	ip4_menu.desk_menu = &desk_menu;
	
	menu(main_menu);
	return NULL;
}
