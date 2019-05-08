/*******************************************************************
  Simple program to check LCD functionality on MicroZed
  based MZ_APO board designed by Petr Porazil at PiKRON

  mzapo_lcdtest.c       - main and only file

  (C) Copyright 2004 - 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      work:     http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#define BLACK 0x0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define GREEN 0x07E0

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

unsigned char *mem_base;
uint32_t *knobs;
unsigned char *led1_mem_base;
unsigned char *led2_mem_base;
uint32_t led1_rgb_value;
uint32_t led2_rgb_value;
uint16_t frame[FRAME_H][FRAME_W];
	
int get_knob_value();
void get_rgb_values(uint32_t rgb_knobs_value, uint16_t *r, uint16_t *g, uint16_t *b);
void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value);
void clear_screen();
	
void rectangle_to_lcd(uint32_t rgb_knobs_value, int row, int column, int width, int height){
	uint16_t rk, gk, bk;
	uint16_t color = 0;
	printf("%x\n", *knobs);
	parlcd_write_cmd(parlcd_mem_base, 0x2c);
	
	get_rgb_values(rgb_knobs_value, &rk, &gk, &bk);

	color = ((rk >> 3) << 11) | ((gk >> 2) << 5) | (bk >> 3);
			
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			if(r >= row && r < height + row && c >= column && c < width + column){
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
	uint16_t rk, gk, bk; 
	uint8_t rb, gb, bb;
	get_rgb_values(rgb_knobs_value, &rk, &gk, &bk);
	get_buttons_value(rgb_knobs_value, &rb, &gb, &bb);
	
	*led_mem_base = bk;
	*(led_mem_base+1) = gk;
	*(led_mem_base+2) = rk;
	/*
	if(bb == 1){
		*led_mem_base = bb;
	}
	if(gb == 1){
		*(led_mem_base+1) = gb;
	}
	if(rb == 1){
		*(led_mem_base+2) = rb;
	}
	*/
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

void get_rgb_values(uint32_t rgb_knobs_value, uint16_t *r, uint16_t *g, uint16_t *b){
	*b =  rgb_knobs_value      & 0xFF; // blue knob position
	*g = (rgb_knobs_value>>8)  & 0xFF; // green knob position
	*r = (rgb_knobs_value>>16) & 0xFF; // red knob position
}

void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b){
	*b = (rgb_knobs_value>>24) & 1;    // blue button
	*g = (rgb_knobs_value>>25) & 1;    // green button
	*r = (rgb_knobs_value>>26) & 1;    // red buttom
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

void static_menu(){
	clear_screen();
	uint32_t rgb_knobs_value;
	uint16_t blue_knob_value;
	uint16_t prev_blue_knob_value = get_knob_value()  & 0xFF;;
	uint8_t blue_button, green_button, red_button;
	int menu_pos = 0;
	bool choosing_color = false;
	
	int last_changed_led = 0;
	
	while(true){
		rgb_knobs_value = get_knob_value();
		blue_knob_value =  rgb_knobs_value & 0xFF;
		get_buttons_value(rgb_knobs_value, &red_button, &green_button, &blue_button);
		if(red_button) {
			clear_screen();
			break;
		}
		if(blue_button) {
			usleep(100000);
			clear_screen();
			choosing_color = !choosing_color;
		}
		
		if(!choosing_color){
			menu_pos = change_menu_pos(4, blue_knob_value, prev_blue_knob_value, menu_pos);
			printf("menu_pos: %d\n",menu_pos);
		}
		prev_blue_knob_value = blue_knob_value;
		draw_static_menu(menu_pos);
		if(choosing_color){
			switch(menu_pos){
				case 0:
					rectangle_to_lcd(rgb_knobs_value, 80, 40, 400, 20);
					to_led(led1_mem_base, rgb_knobs_value);
					led1_rgb_value = rgb_knobs_value;
					last_changed_led = 0;
					break;
				case 1:
					rectangle_to_lcd(rgb_knobs_value, 150, 40, 400, 20);
					to_led(led2_mem_base, rgb_knobs_value);
					led2_rgb_value = rgb_knobs_value; 
					last_changed_led = 1;
					break;
				case 2:
					rectangle_to_lcd(rgb_knobs_value, 230, 40, 400, 20);
					to_led(led1_mem_base, rgb_knobs_value);
					to_led(led2_mem_base, rgb_knobs_value);
					break;
				case 3:
					if(last_changed_led == 0){
						to_led(led2_mem_base, led1_rgb_value);
					}
					else{
						to_led(led1_mem_base, led2_rgb_value);
					}
					choosing_color = false;
					break;
			}
		}
	}
}

void clear_screen(){
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			frame[r][c] = BLACK;
		}
	}
	frameToLCD();
}

int main(int argc, char *argv[])
{
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) exit(1);

	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL)  exit(1);
	parlcd_hx8357_init(parlcd_mem_base);
		
	knobs = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	
	led1_mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	led2_mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	
	uint32_t rgb_knobs_value;
	uint16_t blue_knob_value;
	uint16_t prev_blue_knob_value;
	uint8_t blue_button;
	prev_blue_knob_value =  get_knob_value() & 0xFF;
	int menu_pos = 0;
		
	while(true){
		rgb_knobs_value = get_knob_value();
		blue_knob_value =  rgb_knobs_value & 0xFF;
		blue_button = (rgb_knobs_value>>24) & 1;
		menu_pos = change_menu_pos(3, blue_knob_value, prev_blue_knob_value, menu_pos);
		prev_blue_knob_value = blue_knob_value;
		printf("menu_pos: %d\n",menu_pos);
		
		draw_main_menu(menu_pos);
		if(menu_pos == 0 && blue_button){
			usleep(100000);
			static_menu();
			menu_pos = 0;
		}
		//rectangle_to_lcd(rgb_knobs_value, 80, 100, 300, 3 0);
		//to_led(led1_mem_base, rgb_knobs_value);
		//to_led(led2_mem_base, rgb_knobs_value);
	}
	return 0;
}

