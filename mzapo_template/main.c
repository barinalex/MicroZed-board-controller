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
	char *button0;
	char *button1;
	char *button2;
	char *button3;
	char *button4;
	char *button5;
	char *comment;
} menu_;

menu_ menu;
unsigned char *mem_base;
uint32_t *knobs;
unsigned char *led1_mem_base;
unsigned char *led2_mem_base;
uint32_t led1_rgb_value;
uint32_t led2_rgb_value;
uint16_t frame[FRAME_H][FRAME_W];
	
int get_knob_value();
void get_rgb_values(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value);
void rectangle_to_lcd(uint32_t rgb_knobs_value, rect_ rect);
void clear_screen();
void static_menu();
int initialize_adresses();
void menu_template(int menu_pos, int buttons_number);
void rect_template(int menu_pos, uint32_t rgb_knobs_value);
	
void rectangle_to_lcd(uint32_t rgb_knobs_value, rect_ rect){
	uint8_t rk, gk, bk;
	uint16_t color = 0;
	printf("%x\n", *knobs);
	get_rgb_values(rgb_knobs_value, &rk, &gk, &bk);
	color = ((rk >> 3) << 11) | ((gk >> 2) << 5) | (bk >> 3);
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

void draw_main_menu(int menu_pos){
	strToFrame("Static", 50, 50, WHITE, BLACK);
	strToFrame("Continuous", 150, 50, WHITE, BLACK);
	strToFrame("Flashing", 250, 50, WHITE, BLACK);
	switch(menu_pos){
		case 0:
			strToFrame("Static", 50, 50, BLUE, BLACK);
			break;
		case 1:
			strToFrame("Continuous", 150, 50, BLUE, BLACK);
			break;
		case 2:
			strToFrame("Flashing", 250, 50, BLUE, BLACK);
			break;
	}
	frameToLCD();
}

void draw_static_menu(int menu_pos){
	strToFrame("Led 1", 50, 50, WHITE, BLACK);
	strToFrame("Led 2", 120, 50, WHITE, BLACK);
	strToFrame("Both", 190, 50, WHITE, BLACK);
	strToFrame("Copy", 260, 50, WHITE, BLACK);
	switch(menu_pos){
		case 0:
			strToFrame("Led 1", 50, 50, BLUE, BLACK);
			break;
		case 1:
			strToFrame("Led 2", 120, 50, BLUE, BLACK);
			break;
		case 2:
			strToFrame("Both", 190, 50, BLUE, BLACK);
			break;
		case 3:
			strToFrame("Copy", 260, 50, BLUE, BLACK);
			break;
	}
	frameToLCD();
}

void menu_template(int menu_pos, int buttons_number){
	if(buttons_number > 0) strToFrame(menu.button0, 10, 50, WHITE, BLACK);
	if(buttons_number > 1) strToFrame(menu.button1, 60, 50, WHITE, BLACK);
	if(buttons_number > 2) strToFrame(menu.button2, 110, 50, WHITE, BLACK);
	if(buttons_number > 3) strToFrame(menu.button3, 160, 50, WHITE, BLACK);
	if(buttons_number > 4) strToFrame(menu.button4, 210, 50, WHITE, BLACK);
	if(buttons_number > 5) strToFrame(menu.button5, 260, 50, WHITE, BLACK);
	switch(menu_pos){
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

void rect_template(int menu_pos, uint32_t rgb_knobs_value){
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
	rectangle_to_lcd(rgb_knobs_value, rect);
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
	uint32_t rgb_knobs_value;
	uint16_t blue_knob_value;
	uint16_t prev_blue_knob_value = get_knob_value()  & 0xFF;;
	uint8_t blue_button, green_button, red_button;
	int menu_pos = 0, buttons_number = 4;
	bool choosing_color = false;
	int last_changed_led = 0;
	
	menu.button0 = "Led 1";
	menu.button1 = "Led";
	menu.button2 = "Both";
	menu.button3 = "Copy";
	
	while(true){
		rgb_knobs_value = get_knob_value();
		blue_knob_value =  rgb_knobs_value & 0xFF;
		get_buttons_value(rgb_knobs_value, &red_button, &green_button, &blue_button);
		blue_button = (rgb_knobs_value>>24) & 1;
		red_button = (rgb_knobs_value>>26) & 1;
		if(red_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		if(blue_button) {
			usleep(DELAY);
			clear_screen();
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu_pos = change_menu_pos(buttons_number, blue_knob_value, prev_blue_knob_value, menu_pos);
		}else{
			rect_template(menu_pos, rgb_knobs_value);
			static_menu_led_control(menu_pos,rgb_knobs_value, &last_changed_led);
			choosing_color = (menu_pos == 3) ? false: choosing_color;
		}
		prev_blue_knob_value = blue_knob_value;
		menu_template(menu_pos, buttons_number);
	}
}

void set_main_menu_buttons(){
	menu.button0 = "Static";
	menu.button1 = "Continuous";
	menu.button2 = "Flashing";
}

int main(int argc, char *argv[])
{
	if(initialize_adresses() != 0) exit(1);
	uint32_t rgb_knobs_value;
	uint16_t blue_knob_value;
	uint16_t prev_blue_knob_value = get_knob_value() & 0xFF;
	uint8_t blue_button;
	int menu_pos = 0, buttons_number = 4;
	set_main_menu_buttons();

		
	while(true){
		rgb_knobs_value = get_knob_value();
		blue_knob_value =  rgb_knobs_value & 0xFF;
		blue_button = (rgb_knobs_value>>24) & 1;
		menu_pos = change_menu_pos(buttons_number, blue_knob_value, prev_blue_knob_value, menu_pos);
		prev_blue_knob_value = blue_knob_value;
		menu_template(menu_pos, buttons_number);		
		//draw_main_menu(menu_pos);
		if(menu_pos == 0 && blue_button){
			usleep(DELAY);
			static_menu();
			menu_pos = 0;
			set_main_menu_buttons();
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
	knobs = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	led1_mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	led2_mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	return 0;
}

