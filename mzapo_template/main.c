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

void led_control(){

}

int main(int argc, char *argv[])
{
	unsigned char *mem_base;
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) exit(1);
	
	uint32_t *knobs = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	uint32_t rgb_knobs_value; 
	
	unsigned char *led1_mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	unsigned char *led2_mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	
	unsigned char *parlcd_mem_base;
	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL)  exit(1);
	parlcd_hx8357_init(parlcd_mem_base);
	
	uint16_t rk, gk, bk, rb, gb, bb;
	uint16_t color = 0;
		
	while(true){
		printf("%x\n", *knobs);
		parlcd_write_cmd(parlcd_mem_base, 0x2c);
		rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

		bk =  rgb_knobs_value      & 0xFF; // blue knob position
		gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
		rk = (rgb_knobs_value>>16) & 0xFF; // red knob position
		bb = (rgb_knobs_value>>24) & 1;    // blue button
		gb = (rgb_knobs_value>>25) & 1;    // green button
		rb = (rgb_knobs_value>>26) & 1;    // red buttom
		
		*led1_mem_base = bk;
		*(led1_mem_base+1) = gk;
		*(led1_mem_base+2) = rk;
		*led2_mem_base = bk;
		*(led2_mem_base+1) = gk;
		*(led2_mem_base+2) = rk;
		
		if(bb == 1){
			*led1_mem_base = bb;
			*led2_mem_base = bb;
		}
		if(gb == 1){
			*(led1_mem_base+1) = gb;
			*(led2_mem_base+1) = gb;
		}
		if(rb == 1){
			*(led1_mem_base+2) = rb;
			*(led2_mem_base+2) = rb;
		}
	
		color = ((rk >> 3) << 11) | ((gk >> 2) << 5) | (bk >> 3);
				for (int i = 0; i < 320 ; i++) {
			for (int j = 0; j < 480 ; j++) {
				*(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_DATA_o) = color;
				//parlcd_write_data(parlcd_mem_base, j < 420 ? color : 0);
			}
		}		
	}

	return 0;
}


