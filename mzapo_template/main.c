/*******************************************************************
  Simple program to check LCD functionality on MicroZed

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#define BLACK 0x0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define DELAY 100000

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcdframe.h"

typedef struct{
	int top;
	int bottom;
	int left;
	int right;
} rect_;

typedef struct{
	int buttons_number;
	int menu_pos;
	char *button0;
	char *button1;
	char *button2;
	char *button3;
	char *button4;
	char *button5;
	char *comment;
} menu_;

typedef struct{
	uint32_t rgb_value;
	uint8_t r_knob;
	uint8_t g_knob;
	uint8_t b_knob;
	uint8_t r_button;
	uint8_t g_button;
	uint8_t b_button;	
} knobs_;

unsigned char *mem_base;
uint32_t *knobs_base;
unsigned char *led1_mem_base;
unsigned char *led2_mem_base;
uint32_t led1_rgb_value;
uint32_t led2_rgb_value;
uint16_t frame[FRAME_H][FRAME_W];
	
int get_knob_value();
void get_rgb_values(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value);
void rectangle_to_lcd(knobs_ knobs, rect_ rect);
void clear_screen();
void static_menu();
int initialize_adresses();
void menu_template(menu_ menu);
rect_ set_rect(int menu_pos);
void get_knobs_data(knobs_ *knobs);
	
void rectangle_to_lcd(knobs_ knobs, rect_ rect){
	uint16_t color = 0;
	printf("%x\n", *knobs_base);
	color = ((knobs.r_knob >> 3) << 11) | ((knobs.g_knob >> 2) << 5) | (knobs.b_knob >> 3);
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			if(r >= rect.top && r < rect.bottom && c >= rect.left && c < rect.right){
				frame[r][c] = color;
			}
		}
	}
	frameToLCD(parlcd_mem_base);
}

int get_knob_value(){
	return *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value){
	uint8_t rk, gk, bk; 
	get_rgb_values(rgb_knobs_value, &rk, &gk, &bk);
	*led_mem_base = bk;
	*(led_mem_base+1) = gk;
	*(led_mem_base+2) = rk;
}

void menu_template(menu_ menu){
	if(menu.buttons_number > 0) strToFrame(menu.button0, 10, 50, WHITE, BLACK);
	if(menu.buttons_number > 1) strToFrame(menu.button1, 60, 50, WHITE, BLACK);
	if(menu.buttons_number > 2) strToFrame(menu.button2, 110, 50, WHITE, BLACK);
	if(menu.buttons_number > 3) strToFrame(menu.button3, 160, 50, WHITE, BLACK);
	if(menu.buttons_number > 4) strToFrame(menu.button4, 210, 50, WHITE, BLACK);
	if(menu.buttons_number > 5) strToFrame(menu.button5, 260, 50, WHITE, BLACK);
	switch(menu.menu_pos){
		case 0:
			strToFrame(menu.button0, 10, 50, BLUE, BLACK);
			break;
		case 1:
			strToFrame(menu.button1, 60, 50, BLUE, BLACK);
			break;
		case 2:
			strToFrame(menu.button2, 110, 50, BLUE, BLACK);
			break;
		case 3:
			strToFrame(menu.button3, 160, 50, BLUE, BLACK);
			break;
		case 4:
			strToFrame(menu.button4, 210, 50, BLUE, BLACK);
			break;
		case 5:
			strToFrame(menu.button5, 260, 50, BLUE, BLACK);
			break;
	}
	frameToLCD();
}

rect_ set_rect(int menu_pos){
	rect_ rect;
	rect.left = 40;
	rect.right = 400;
	switch(menu_pos){
		case 0:
			rect.top = 35;
			rect.bottom = 50;
			break;
		case 1:
			rect.top = 85;
			rect.bottom = 100;
			break;
		case 2:
			rect.top = 135;
			rect.bottom = 150;
			break;
		case 3:
			rect.top = 185;
			rect.bottom = 200;
			break;
		case 4:
			rect.top = 235;
			rect.bottom = 250;
			break;
		case 5:
			rect.top = 285;
			rect.bottom = 300;
			break;
	}
	return rect;
}

void get_rgb_values(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b){
	*b =  rgb_knobs_value      & 0xFF; // blue knob position
	*g = (rgb_knobs_value>>8)  & 0xFF; // green knob position
	*r = (rgb_knobs_value>>16) & 0xFF; // red knob position
}

void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b){
	*b = (rgb_knobs_value>>24) & 1; // blue button
	*g = (rgb_knobs_value>>25) & 1; // green button
	*r = (rgb_knobs_value>>26) & 1; // red buttom
}

int change_menu_pos(int buttons_number, uint8_t button_value, uint8_t prev_button_value, int menu_pos){
		if(button_value > prev_button_value + 1){
			menu_pos++;
			menu_pos = (menu_pos > buttons_number - 1) ? buttons_number - 1: menu_pos;
		}
		else if(button_value < prev_button_value - 1){
			menu_pos--;
			menu_pos = (menu_pos < 0) ? 0: menu_pos;
		}
		return menu_pos;
}

void static_menu_led_control(int menu_pos, int rgb_knobs_value, int *last_changed_led){
	switch(menu_pos){
		case 0:
			to_led(led1_mem_base, rgb_knobs_value);
			led1_rgb_value = rgb_knobs_value;
			*last_changed_led = 0;
			break;
		case 1:
			to_led(led2_mem_base, rgb_knobs_value);
			led2_rgb_value = rgb_knobs_value; 
			*last_changed_led = 1;
			break;
		case 2:
			to_led(led1_mem_base, rgb_knobs_value);
			to_led(led2_mem_base, rgb_knobs_value);
			break;
		case 3:
			if(*last_changed_led == 0){
				to_led(led2_mem_base, led1_rgb_value);
			}
			else{
				to_led(led1_mem_base, led2_rgb_value);
			}
			break;
	}
}

void static_menu(){
	clear_screen();
	bool choosing_color = false;
	int last_changed_led = 0;
	
	knobs_ knobs;
	get_knobs_data(&knobs);
	uint8_t prev_blue_knob_value = knobs.b_knob;
	
	menu_ menu;
	menu.buttons_number = 4;
	menu.menu_pos = 0;
	menu.button0 = "Led 1";
	menu.button1 = "Led";
	menu.button2 = "Both";
	menu.button3 = "Copy";
	
	while(true){
		if(knobs.r_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(knobs.b_button) {
			usleep(DELAY);
			clear_screen();
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_blue_knob_value, menu.menu_pos);
		}else{	
			rectangle_to_lcd(knobs, set_rect(menu.menu_pos));
			static_menu_led_control(menu.menu_pos, knobs.rgb_value, &last_changed_led);
			if(menu.menu_pos == 3) choosing_color = false;
		}
		prev_blue_knob_value = knobs.b_knob;
		menu_template(menu);
	}
}

void get_knobs_data(knobs_ *knobs){
	knobs->rgb_value = get_knob_value();
	knobs->b_knob =  knobs->rgb_value      & 0xFF; // blue knob position
	knobs->g_knob = (knobs->rgb_value>>8)  & 0xFF; // green knob position
	knobs->r_knob = (knobs->rgb_value>>16) & 0xFF; // red knob position
	knobs->b_button = (knobs->rgb_value>>24) & 1; // blue button
	knobs->g_button = (knobs->rgb_value>>25) & 1; // green button
	knobs->r_button = (knobs->rgb_value>>26) & 1; // red buttom
}

int main(int argc, char *argv[])
{
	if(initialize_adresses() != 0) exit(1);
	knobs_ knobs;
	menu_ menu;
	menu.buttons_number = 3;
	menu.menu_pos = 0;
	menu.button0 = "Static";
	menu.button1 = "Continuous";
	menu.button2 = "Flashing";
	
	get_knobs_data(&knobs);
	uint8_t prev_b_knob = knobs.b_knob;

		
	while(true){
		get_knobs_data(&knobs);
		menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_b_knob, menu.menu_pos);
		prev_b_knob = knobs.b_knob;
		menu_template(menu);		
		//draw_main_menu(menu_pos);
		if(menu.menu_pos == 0 && knobs.b_button){
			usleep(DELAY);
			static_menu();
			menu.menu_pos = 0;
		}
	}
	return 0;
}

void clear_screen(){
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			frame[r][c] = BLACK;
		}
	}
	frameToLCD();
}

int initialize_adresses(){
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) return 1;
	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL) return 1;
	parlcd_hx8357_init(parlcd_mem_base);
	knobs_base = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	led1_mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	led2_mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	return 0;
}

