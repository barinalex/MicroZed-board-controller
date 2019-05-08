#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcdframe.h"
#include "font_types.h"


void frameToLCD()
{
	// To begin drawing, move LCD pixel pointer to point 0,0 - top left corner 
	*(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o) = 0x2c; // command
	
	// budu cist po 2 prvcich
	volatile uint32_t* ptr = (volatile uint32_t*) frame;
	volatile uint32_t* dst = (volatile uint32_t*) (parlcd_mem_base + PARLCD_REG_DATA_o);

	// jeden prvek smycky bude trvat 200ns
	for (int i = 0; i < FRAME_H*(FRAME_W/2); i++) {
		*dst = *ptr++;
	}
}

// fonts: opt/apo/lcd/fonts/
int charToFrame(char c, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor) 
{
	// mezera je prvni znak ve fontu
	int cIdx = c - ' ';
	int w = font_winFreeSystem14x16.width[cIdx] + 4; // aby za tim fontem byla mezera na 4 pozice
	// vim ze velikost znaku je 16
	for (int y = 0; y < 16; y++) {
		uint16_t mask = font_winFreeSystem14x16.bits[16*cIdx+y];
		// w je sirka znaku 
		for (int x = 0; x < w; x++) {
			frame[yRow+y][xColumn + x] = (mask & 0x8000) ? forecolor : backcolor;
			mask <<= 1; // mask = mask << 1; rotuju o 1
		}
	}
	return w; // sirka vytisteneho znaku	
}

int strToFrame(char *str, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor) 
{
	char c;
	int w = 0;
	while((c=*str++) != 0) {
		w += charToFrame(c, yRow, xColumn+w, forecolor, backcolor);
	}
	return w; // pixelova sirka napsaneho textu, abychom vedeli, kde muzeme psat dalsi znak
}

