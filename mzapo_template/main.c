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
	unsigned char *parlcd_mem_base;
	uint32_t *knobs;
	
int get_knob_value();
	
void to_lcd(uint32_t rgb_knobs_value, int row, int column, int width, int height){
	uint16_t rk, gk, bk;
	uint16_t color = 0;
	printf("%x\n", *knobs);
	parlcd_write_cmd(parlcd_mem_base, 0x2c);
	
	bk =  rgb_knobs_value      & 0xFF; // blue knob position
	gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
	rk = (rgb_knobs_value>>16) & 0xFF; // red knob position

	color = ((rk >> 3) << 11) | ((gk >> 2) << 5) | (bk >> 3);
			
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			if(r >= row && r < height + row && c >= column && c < width + column){
				*(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_DATA_o) = color;
			}else{
			    *(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_DATA_o) = 0;
			}
		}
	}
}

int get_knob_value(){
	return *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
}

void to_led1(unsigned char *led_mem_base, uint32_t rgb_knobs_value){
	uint16_t rk, gk, bk, rb, gb, bb;
	bk =  rgb_knobs_value      & 0xFF; // blue knob position
	gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
	rk = (rgb_knobs_value>>16) & 0xFF; // red knob position
	bb = (rgb_knobs_value>>24) & 1;    // blue button
	gb = (rgb_knobs_value>>25) & 1;    // green button
	rb = (rgb_knobs_value>>26) & 1;    // red buttom
	
	*led_mem_base = bk;
	*(led_mem_base+1) = gk;
	*(led_mem_base+2) = rk;

	if(bb == 1){
		*led_mem_base = bb;
	}
	if(gb == 1){
		*(led_mem_base+1) = gb;
	}
	if(rb == 1){
		*(led_mem_base+2) = rb;
	}
}

int main(int argc, char *argv[])
{
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) exit(1);

	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL)  exit(1);
	parlcd_hx8357_init(parlcd_mem_base);
		
	knobs = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	
	unsigned char *led1_mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	unsigned char *led2_mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	
	uint32_t rgb_knobs_value;
	
	while(true){
		rgb_knobs_value = get_knob_value();
		int yRow = strToFrame("Menu", 50, 50, 0xFFFF, 0x0);
		frameToLCD(parlcd_mem_base);	
		to_lcd(rgb_knobs_value, 100, 100, 100, 100);
		to_led1(led1_mem_base, rgb_knobs_value);
		to_led1(led2_mem_base, rgb_knobs_value);
	}
	return 0;
}

