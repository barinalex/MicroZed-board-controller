#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcdframe.h"
#include "utils.h"
#include "reg_manager.h"
#include "led_control.h"


typedef struct menu_{
	int buttons_number;
	int prev_menu_pos;
	int pos;
	
	knobs_ cur_knobs;
	knobs_ prev_knobs;
	
	char *button0;
	char *button1;
	char *button2;
	char *button3;
	char *button4;
	char *comment;
	char *comment2;
	
	void (*func0)();
	void (*func1)();
	void (*func2)();
	void (*func3)();
	void (*func4)();
	
	struct menu_ *desk_menu;
	
	struct menu_ *prev;
	struct menu_ *next0;
	struct menu_ *next1;
	struct menu_ *next2;
	struct menu_ *next3;
	struct menu_ *next4;
} menu_;

bool big_text;

void go_desk_menu(menu_ *menu);
void *create_menu(void *vargp);
void menu(menu_ menu);
void draw_menu(menu_ menu);
void go_next_menu(menu_ *menu);
void go_prev_menu(menu_ *menu);
void set_no_links(menu_ *menu);

#endif
