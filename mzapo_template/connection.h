#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	bool sending;
	bool receiving;
	bool connected;
	bool find_others;
	int sockfd;
	
	char *ip_addr[20];
	char *receiver_ip;
	bool ready[20];
}network_state_;

network_state_ nw_state;
int received_knobs_value;

void* network_communication(void *vargp);

#endif
