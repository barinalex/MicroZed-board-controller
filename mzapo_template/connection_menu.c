#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "menu.h"
#include "connection_menu.h"
#include "connection.h"

void add_ip_buttons(menu_ *menu){
	if(nw_state.ready[0]){
		menu->buttons_number = 2;
		menu->button1 = nw_state.ip_addr[0];
	}
	if(nw_state.ready[1]){
		menu->buttons_number = 3;
		menu->button2 = nw_state.ip_addr[1];
	}
	if(nw_state.ready[2]){
		menu->buttons_number = 4;
		menu->button3 = nw_state.ip_addr[2];
	}
	if(nw_state.ready[3]){
		menu->buttons_number = 5;
		menu->button4 = nw_state.ip_addr[3];
	}
}

void find_all(menu_ *menu){
	nw_state.sending = true;
	nw_state.receiving = false;
	nw_state.connected = false;
	nw_state.find_others = true;
	usleep(200000);
	add_ip_buttons(menu);
}

void connect(menu_ *menu){
	nw_state.sending = true;
	nw_state.receiving = false;
	nw_state.connected = false;
	nw_state.find_others = false;
	switch(menu->prev_menu_pos){
		case 1:
			nw_state.receiver_ip = menu->prev->button1;
			break;
		case 2:
			nw_state.receiver_ip = menu->prev->button2;
			break;
		case 3:
			nw_state.receiver_ip = menu->prev->button3;
			break;
		case 4:
			nw_state.receiver_ip = menu->prev->button4;
			break;
	}
	unsigned long start_connection = get_cur_time_in_mlsec();
	while(get_cur_time_in_mlsec() - start_connection < 200){
		if(nw_state.connected){
			go_desk_menu(menu);
			break;
		}
	}
}

void disconnect(menu_ *menu){
	nw_state.sending = false;
	nw_state.receiving = false;
	nw_state.connected = false;
	nw_state.find_others = false;
}

void create_connection_menu(menu_ *menu){
	menu->buttons_number = 1;
	menu->pos = 0;
	menu->button0 = "Find all";
	menu->button1 = "";
	menu->button2 = "";
	menu->button3 = "";
	menu->button4 = "";
	menu->name = "Network";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &find_all;
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
	menu->button1 = "Disconnect";
	menu->name = "Connection";
	menu->comment = "exit";
	menu->comment2 = "choose";
	
	menu->func0 = &connect;
	menu->func1 = &disconnect;
	set_no_links(menu);
}

