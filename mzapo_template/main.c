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
#include <pthread.h>

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

typedef struct{
	unsigned long start_time; // = get_cur_time()
	unsigned long cur_time;
}flash_time_;

typedef struct{
	bool change;
	unsigned char *mem_base;
	HSV hsv;
	RGB rgb;
	HSV hsv2;
	RGB rgb2;
	unsigned long cur_time;
}LED;

LED led1;
LED led2;
unsigned char *mem_base;
uint32_t *knobs_base;
uint16_t frame[FRAME_H][FRAME_W];
bool big_text;
	
int get_knobs_value();
void get_rgb_values(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void get_buttons_value(uint32_t rgb_knobs_value, uint8_t *r, uint8_t *g, uint8_t *b);
void to_led(unsigned char *led_mem_base, uint32_t rgb_knobs_value);
void rectangle_to_lcd(RGB rgb, rect_ rect);
void static_menu();
int initialize_adresses();
void menu_template(menu_ menu);
void get_knobs_data(knobs_ *knobs);
	
void rectangle_to_lcd(RGB rgb, rect_ rect){
	uint16_t color = 0;
	color = ((rgb.r >> 3) << 11) | ((rgb.g >> 2) << 5) | (rgb.b >> 3);
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

void to_led_(LED led1, LED led2){
	*led1.mem_base = led1.rgb.b;
	*(led1.mem_base + 1) = led1.rgb.g;
	*(led1.mem_base + 2) = led1.rgb.r;
	
	*led2.mem_base = led2.rgb.b;
	*(led2.mem_base + 1) = led2.rgb.g;
	*(led2.mem_base + 2) = led2.rgb.r;
}

void led_thread(void *vargp){
	while(true){
		*led1.mem_base = led1.rgb.b;
		*(led1.mem_base + 1) = led1.rgb.g;
		*(led1.mem_base + 2) = led1.rgb.r;
		
		*led2.mem_base = led2.rgb.b;
		*(led2.mem_base + 1) = led2.rgb.g;
		*(led2.mem_base + 2) = led2.rgb.r;
	}
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

uint16_t change(uint16_t data, uint8_t new_value, uint8_t old_value, uint16_t max_data){
		if(new_value > old_value + 1){
			data++;
			data %= (max_data + 1);
		}
		else if(new_value < old_value - 1){
			data--;
		}
		data = (data > max_data) ? max_data: data;
		data = (data < 0) ? 0: data;
		printf("data: %d\n", data);
		return data;
}

void color_menu(int menu_pos){
	rect_ rect_led1 = set_rect(0);
	rect_ rect_led2 = set_rect(1);
	LED new_led1 = led1;
	LED new_led2 = led2;
	
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&knobs);
	prev_knobs = knobs;
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

void static_pos_chosed(int menu_pos){
	switch(menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			color_menu(menu_pos);
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			color_menu(menu_pos);
			break;
		case 2:
			led1.change = true;
			led2.change = true;
			led2.hsv = led1.hsv;
			led2.rgb = led1.rgb;
			color_menu(menu_pos);
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

void static_menu(){
	clear_screen();
	bool choosing_color = false;	
	knobs_ knobs;
	get_knobs_data(&knobs);
	uint8_t prev_blue_knob_value = knobs.b_knob;
	
	menu_ menu;
	menu.buttons_number = 5;
	menu.menu_pos = 0;
	menu.button0 = "Led 1";
	menu.button1 = "Led 2";
	menu.button2 = "Both";
	menu.button3 = "Led 1 to Led 2";
	menu.button4 = "Led 2 to Led 1";
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
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_blue_knob_value, menu.menu_pos);
		}else{
			static_pos_chosed(menu.menu_pos);
			choosing_color = !choosing_color;
		}
		prev_blue_knob_value = knobs.b_knob;
		menu_template(menu);
	}
}

int change_uint(int data, uint8_t new_value, uint8_t old_value){
		if(new_value > old_value + 1){
			data++;
		}
		else if(new_value < old_value - 1){
			data--;
		}
		data = (data < 0) ? 0: data;
		return data;
}

void choose_time(){
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	int time;
	char str[12];
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
			break;
		}
		time = change_uint(time, knobs.b_knob, prev_knobs.b_knob);
		sprintf(str, "%d", time);
		strToFrame(str, 130, 240, WHITE, BLACK, big_text);
		frameToLCD();
		prev_knobs = knobs;
	}
}

void flash_pos_chosed(int menu_pos){
	switch(menu_pos){
		case 0:
			led1.change = true;
			led2.change = false;
			color_menu(menu_pos);
			break;
		case 1:
			led1.change = false;
			led2.change = true;
			color_menu(menu_pos);
			break;
		case 2:
			choose_time();
			break;
	}
}

void choose_flash(){
	clear_screen();
	bool choosing_color = false;	
	knobs_ knobs;
	get_knobs_data(&knobs);
	uint8_t prev_blue_knob_value = knobs.b_knob;
	
	menu_ menu;
	menu.buttons_number = 3;
	menu.menu_pos = 0;
	menu.button0 = "Color 1";
	menu.button1 = "Color 2";
	menu.button2 = "Shift";
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
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_blue_knob_value, menu.menu_pos);
		}else{
			flash_pos_chosed(menu.menu_pos);
			choosing_color = !choosing_color;
		}
		prev_blue_knob_value = knobs.b_knob;
		menu_template(menu);
	}
}

void flashing_menu(){
	clear_screen();
	bool choosing_color = false;	
	knobs_ knobs;
	knobs_ prev_knobs;
	get_knobs_data(&prev_knobs);
	
	menu_ menu;
	menu.buttons_number = 4;
	menu.menu_pos = 0;
	menu.button0 = "Led 1";
	menu.button1 = "Led 2";
	menu.button2 = "Both";
	menu.button3 = "Shift";
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
			choosing_color = !choosing_color;
		}
		if(!choosing_color){
			menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_knobs.b_knob, menu.menu_pos);
		}else{
			choose_flash();
			choosing_color = !choosing_color;
		}
		prev_knobs = knobs;
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
	
	get_knobs_data(&prev_knobs);

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
		if(knobs.b_button){
			switch(menu.menu_pos){
				case 0:
					usleep(DELAY);
					static_menu();
					menu.menu_pos = 0;
					get_knobs_data(&prev_knobs);
					break;
				case 2:
					usleep(DELAY);
					flashing_menu();
					menu.menu_pos = 0;
					get_knobs_data(&prev_knobs);
					break;
			}
		}
	}
}

void main_menu(void *vargp){
	knobs_ knobs;
	knobs_ prev_knobs;
	menu_ menu;
	menu.buttons_number = 3;
	menu.menu_pos = 0;
	menu.button0 = "This";
	menu.button1 = "Choose another";
	menu.button2 = "Change Text Size";
	menu.comment = "Exit: red. Choose: blue";
	
	get_knobs_data(&prev_knobs);
	
	big_text = false;
	
	while(true){
		printf("%x\n", *knobs_base);
		get_knobs_data(&knobs);
		menu.menu_pos = change_menu_pos(menu.buttons_number, knobs.b_knob, prev_knobs.b_knob, menu.menu_pos);
		prev_knobs = knobs;
		menu_template(menu);
		if(knobs.b_button){
			switch(menu.menu_pos){
				case 0:
					usleep(DELAY);
					main_desk_menu();
					menu.menu_pos = 0;
					get_knobs_data(&prev_knobs);
					break;
				case 2:
					big_text = !big_text;
					usleep(DELAY);
					clear_screen();
					break;
			}
		}
		//cur_time = get_cur_time();
		//printf("%ld\n", (cur_time - start_time) / 1000000);
	}
}

int main(int argc, char *argv[])
{
	if(initialize_adresses() != 0) exit(1);
   	pthread_t thread_id; 
    pthread_create(&thread_id, NULL, led_thread, (void *)&thread_id);
    pthread_create(&thread_id, NULL, main_menu, (void *)&thread_id);
    pthread_exit(NULL);
	return 0;
}

int initialize_adresses(){
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) return 1;
	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL) return 1;
	parlcd_hx8357_init(parlcd_mem_base);
	knobs_base = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	led1.mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	led2.mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	return 0;
}

