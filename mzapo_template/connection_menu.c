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
		menu->buttons_number = 3;
	}
	if(nw_state.ready[1]){
		menu->buttons_number = 4;
	}
	if(nw_state.ready[2]){
		menu->buttons_number = 5;
	}
}

void find_all(menu_ *menu){
	printf("find_all\n");
	nw_state.sending = false;
	nw_state.connected = false;
	nw_state.find_others = true;
	nw_state.sending = true;
	nw_state.receiver_ip = NULL;
	usleep(200000);
	add_ip_buttons(menu);
	nw_state.sending = false;
	nw_state.receiving = true;
	nw_state.first_find = true;
}

void set_ip(menu_ *menu){
	switch(menu->prev_menu_pos){
		case 2:
			nw_state.receiver_ip = menu->prev->button2;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[0]);
			break;
		case 3:
			nw_state.receiver_ip = menu->prev->button3;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[1]);
			break;
		case 4:
			nw_state.receiver_ip = menu->prev->button4;
			strcpy(nw_state.rec_ip, nw_state.ip_addr[2]);
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
	menu->buttons_number = 2;
	menu->pos = 0;
	for(int i = 0; i < 5; ++i){
		strcpy(nw_state.ip_addr[i], "");
	}
	menu->button0 = "Disconnect"; 
	menu->button1 = "Find"; 
	menu->button2 = nw_state.ip_addr[0]; 
	menu->button3 = nw_state.ip_addr[1];
	menu->button4 = nw_state.ip_addr[2];
	menu->name = "Network";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &disconnect;
	menu->func1 = &find_all;
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
	menu->name = "Connection";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &connect;
	menu->func1 = &copy;
	set_no_links(menu);
}

