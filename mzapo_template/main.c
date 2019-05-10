/*******************************************************************
  Simple program to check LCD functionality on MicroZed

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#define BLACK 0x0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define DELAY 200000

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
#include "utils.h"

typedef struct{
	int buttons_number;
	int menu_pos;
	char *button0;
	char *button1;
	char *button2;
	char *button3;
	char *button4;
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
bool big_text;
	
int get_knobs_value();
void get_rgb_values(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value);
void rectangle_to_lcd(knobs_ knobs, rect_ rect);
void static_menu();
int initialize_adresses();
void menu_template(menu_ menu);
void get_knobs_data(knobs_ *knobs);
	
void rectangle_to_lcd(knobs_ knobs, rect_ rect){
	uint16_t color = 0;
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

int get_knobs_value(){
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
	if(menu.buttons_number > 0) strToFrame(menu.button0, 30, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 1) strToFrame(menu.button1, 80, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 2) strToFrame(menu.button2, 130, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 3) strToFrame(menu.button3, 180, 50, WHITE, BLACK, big_text);
	if(menu.buttons_number > 4) strToFrame(menu.button4, 230, 50, WHITE, BLACK, big_text);
	switch(menu.menu_pos){
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
	menu.button1 = "Led 2";
	menu.button2 = "Both";
	menu.button3 = "Copy";
	menu.comment = "Exit: red. Choose: blue";
	
	while(true){
		printf("%x\n", *knobs_base);
		get_knobs_data(&knobs);
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
			if(menu.menu_pos != 3) rectangle_to_lcd(knobs, set_rect(menu.menu_pos));
			static_menu_led_control(menu.menu_pos, knobs.rgb_value, &last_changed_led);
			if(menu.menu_pos == 3) choosing_color = false;
		}
		prev_blue_knob_value = knobs.b_knob;
		menu_template(menu);
	}
}

void get_knobs_data(knobs_ *knobs){
	knobs->rgb_value = get_knobs_value();
	knobs->b_knob =  knobs->rgb_value      & 0xFF; // blue knob position
	knobs->g_knob = (knobs->rgb_value>>8)  & 0xFF; // green knob position
	knobs->r_knob = (knobs->rgb_value>>16) & 0xFF; // red knob position
	knobs->b_button = (knobs->rgb_value>>24) & 1; // blue button
	knobs->g_button = (knobs->rgb_value>>25) & 1; // green button
	knobs->r_button = (knobs->rgb_value>>26) & 1; // red buttom
}

void main_desk_menu(){
	clear_screen();
	knobs_ knobs;
	knobs_ prev_knobs;
	menu_ menu;
	menu.buttons_number = 3;
	menu.menu_pos = 0;
	menu.button0 = "Static";
	menu.button1 = "Continuous";
	menu.button2 = "Flashing";
	menu.comment = "Exit: red. Choose: blue";
	
	get_knobs_data(&knobs);
	prev_knobs = knobs;

		
	while(true){
		printf("%x\n", *knobs_base);
		get_knobs_data(&knobs);
		if(knobs.r_button) {
			usleep(DELAY);
			clear_screen();
			break;
		}
		menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_knobs.b_knob, menu.menu_pos);
		prev_knobs = knobs;
		menu_template(menu);
		if(menu.menu_pos == 0 && knobs.b_button){
			usleep(DELAY);
			static_menu();
			menu.menu_pos = 0;
		}
	}
}

int main(int argc, char *argv[])
{
	if(initialize_adresses() != 0) exit(1);
	knobs_ knobs;
	knobs_ prev_knobs;
	menu_ menu;
		menu.buttons_number = 3;
	menu.menu_pos = 0;
	menu.button0 = "This";
	menu.button1 = "Choose another";
	menu.button2 = "Change Text Size";
	menu.comment = "Exit: red. Choose: blue";
	
	get_knobs_data(&knobs);
	prev_knobs = knobs;
	
	big_text = false;
		
	while(true){
		printf("%x\n", *knobs_base);
		get_knobs_data(&knobs);
		menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_knobs.b_knob, menu.menu_pos);
		prev_knobs = knobs;
		menu_template(menu);
		if(menu.menu_pos == 0 && knobs.b_button){
			usleep(DELAY);
			main_desk_menu();
			menu.menu_pos = 0;
		}
		if(menu.menu_pos == 2 && knobs.b_button){
			big_text = !big_text;
			usleep(DELAY);
			clear_screen();
		}
	}
	
	main_desk_menu();
	return 0;
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

