#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>

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

#endif
