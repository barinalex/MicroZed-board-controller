#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcdframe.h"
#include "font_types.h"

#define BLACK 0x0

void frameToLCD()
{
	// To begin drawing, move LCD pixel pointer to point 0,0 - top left corner 
	*(volatile uint16_t*)(parlcd_mem_base + PARLCD_REG_CMD_o) = 0x2c; // command
	
	volatile uint16_t* ptr = (volatile uint16_t*) frame;
	volatile uint16_t* dst = (volatile uint16_t*) (parlcd_mem_base + PARLCD_REG_DATA_o);

	// jeden prvek smycky bude trvat 200ns
	int size = FRAME_H*(FRAME_W);
	for (int i = 0; i < size; i++) {
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

int bigCharToFrame(char c, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor) 
{
	// mezera je prvni znak ve fontu
	int cIdx = c - ' ';
	int w = font_winFreeSystem14x16.width[cIdx] * 2 + 4; // aby za tim fontem byla mezera na 4 pozice
	// vim ze velikost znaku je 16
	for (int y = 0; y < 32; y+=2) {
		uint16_t mask = font_winFreeSystem14x16.bits[16*cIdx+y/2];
		// w je sirka znaku 
		for (int x = 0; x < w; x+=2) {
			frame[yRow+y][xColumn + x] = (mask & 0x8000) ? forecolor : backcolor;
			frame[yRow+y][xColumn + x + 1] = (mask & 0x8000) ? forecolor : backcolor;
			frame[yRow+y + 1][xColumn + x] = (mask & 0x8000) ? forecolor : backcolor;
			frame[yRow+y + 1][xColumn + x + 1] = (mask & 0x8000) ? forecolor : backcolor;
			mask <<= 1; // mask = mask << 1; rotuju o 1
		}
	}
	return w; // sirka vytisteneho znaku	
}

int strToFrame(char *str, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor, bool big) 
{
	char c;
	int w = 0;
	while((c=*str++) != 0) {
		if(big){
			w += bigCharToFrame(c, yRow, xColumn+w, forecolor, backcolor);
		}
		else{
			w += charToFrame(c, yRow, xColumn+w, forecolor, backcolor);
		}
	}
	return w; // pixelova sirka napsaneho textu, abychom vedeli, kde muzeme psat dalsi znak
}

int int_to_frame(int number, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor, bool big){
		char str[12];
		str[11] = '\0';
		sprintf(str, "%d", number);
		for(int i = 0; i < 12; ++i){
			if(str[i] == '\0'){
				for(int j = i; j < 11; ++j){
					str[j] = ' ';
				}
				break;
			}
		}
		strToFrame(str, yRow, xColumn, forecolor, backcolor, big);
}

void clear_screen(){
	for (int r = 0; r < 320 ; r++) {
		for (int c = 0; c < 480 ; c++) {
			frame[r][c] = BLACK;
		}
	}
	frameToLCD();
}


