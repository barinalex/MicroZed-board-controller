#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "connection_menu.h"
#include "connection.h"

void add_ip_buttons(menu_ *menu){
	if(nw_state.ready[0]){
		menu->buttons_number = 1;
		menu->button0 = nw_state.ip_addr[0];
	}
	if(nw_state.ready[1]){
		menu->buttons_number = 2;
		menu->button1 = nw_state.ip_addr[1];
	}
	if(nw_state.ready[2]){
		menu->buttons_number = 3;
		menu->button2 = nw_state.ip_addr[2];
	}
	if(nw_state.ready[3]){
		menu->buttons_number = 4;
		menu->button3 = nw_state.ip_addr[3];
	}
	if(nw_state.ready[4]){
		menu->buttons_number = 5;
		menu->button4 = nw_state.ip_addr[4];
	}
}

void find_all(menu_ *menu){
	printf("find_all\n");
	nw_state.sending = false;
	nw_state.connected = false;
	nw_state.receiving = false;
	nw_state.find_others = true;
	nw_state.sending = true;
	nw_state.receiver_ip = NULL;
	usleep(200000);
	add_ip_buttons(menu);
}

void set_ip(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 0:
			nw_state.receiver_ip = menu->prev->button0;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[0]);
			break;
		case 1:
			nw_state.receiver_ip = menu->prev->button1;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[1]);
			break;
		case 2:
			nw_state.receiver_ip = menu->prev->button2;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[2]);
			break;
		case 3:
			nw_state.receiver_ip = menu->prev->button3;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[3]);
			break;
		case 4:
			nw_state.receiver_ip = menu->prev->button4;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[4]);
			break;
	}
}

void connect(menu_ *menu){
	printf("connect\n");
	set_ip(menu);
	nw_state.sending = true;
	nw_state.receiving = false;
	nw_state.connected = false;
	nw_state.find_others = false;
}

void copy(menu_ *menu){
	printf("copy\n");
	if(!nw_state.connected){
		nw_state.receiving = false;
		nw_state.find_others = false;
		set_ip(menu);
		nw_state.copy = true;
		nw_state.sending = true;
	}
}

void disconnect(menu_ *menu){
	nw_state.sending = false;
	nw_state.receiving = true;
	nw_state.connected = false;
	nw_state.find_others = false;
	nw_state.receiver_ip = NULL;
}

void create_connection_menu(menu_ *menu){
	menu->buttons_number = 0;
	menu->pos = 0;
	menu->button0 = ""; //napsat disconnect tady
	menu->button1 = ""; //tady find
	menu->button2 = ""; //dal adresy desek
	menu->button3 = "";
	menu->button4 = "";
	menu->name = "Network";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &go_next_menu;
	menu->func1 = &go_next_menu;
	menu->func2 = &go_next_menu;
	menu->func3 = &go_next_menu;
	menu->func4 = &go_next_menu;
	set_no_links(menu);
}

void create_ip_menu(menu_ *menu){
	menu->buttons_number = 2;
	menu->pos = 0;
	menu->button0 = "Connect";
	menu->button1 = "Copy leds";
	//menu->button2 = "Disconnect";
	menu->name = "Connection";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &connect;
	menu->func1 = &copy;
	//menu->func2 = &disconnect;
	set_no_links(menu);
}

