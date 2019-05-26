#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	bool sending;
	bool copy;
	bool receiving;
	bool connected;
	bool find_others;
	int sockfd;
	int broad_socket;
	
	char ip_addr[5][20];
	char rec_ip[20];
	bool ready[5];
}network_state_;

network_state_ nw_state;
int received_knobs_value;

void set_receiver_addr(unsigned long IP);

void send_init_message(char message);

void* network_communication(void *vargp);

#endif
