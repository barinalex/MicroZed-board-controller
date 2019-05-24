#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define RECEIVER_PORT 8888
#define SENDER_PORT 8888
#define BUFSIZE 1024
#define TIMEOUT 10000
#define DISCONNECT 30000

#include "checksum.h"
#include "connection.h"
#include "led_control.h"
#include "utils.h"

char buffer[BUFSIZE];
struct sockaddr_in sender_addr, receiver_addr;
unsigned long last_connection_time;

int init_sender_addr(){
    sender_addr.sin_family    = AF_INET; // IPv4
    sender_addr.sin_addr.s_addr = INADDR_ANY;
    sender_addr.sin_port = htons(SENDER_PORT);
    if (bind(nw_state.sockfd, (const struct sockaddr*)&sender_addr, sizeof(sender_addr)) < 0){
        fprintf(stderr, "Binding error!\n");
        return 1;
    }
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = TIMEOUT;
    if (setsockopt(nw_state.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "Timeout error!\n");
    }
    return 0;
}

void set_receiver_addr(unsigned long IP){
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = IP;
    receiver_addr.sin_port = htons(RECEIVER_PORT);
}

void send_connection_message(char message){
	unsigned int msg_len, receiver_addr_len;
	do{
		sendto(nw_state.sockfd, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
	}while((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) < 0);
	nw_state.connected = true;
}

void send_init_message(char message){
	set_receiver_addr(INADDR_ANY);
	unsigned int msg_len, receiver_addr_len, index = 0;
	unsigned long start_connect = get_cur_time_in_mlsec();
	sendto(nw_state.sockfd, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
	while(get_cur_time_in_mlsec() - start_connect < 200){
		if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) > 0){
			nw_state.ip_addr[index] = inet_ntoa(receiver_addr.sin_addr);
			nw_state.ready[index++] = true;
		}
	}
	nw_state.find_others = false;
}

void receive_init_message(){
	unsigned int msg_len, sender_addr_len;
	char* message = "Ready";
	srand(time(NULL));
	if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &sender_addr, &sender_addr_len)) > 0){
		switch(buffer[0]){
			case 1:
				usleep((rand() % 180) * 1000);
				break;
			case 2:
				nw_state.connected = true;
				break;
		}
		sendto(nw_state.sockfd, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &sender_addr, sizeof(sender_addr));
	}
}

void send_package(char *package, int size){
	unsigned int msg_len, receiver_addr_len;
	uint32_t crc, received_crc = 0;
	crc = crc_32((const unsigned char*)(package + sizeof(uint32_t)), size);
	memcpy(package, (char*)&crc, sizeof(crc));
	unsigned long start_sending = get_cur_time_in_mlsec();
	do{
		sendto(nw_state.sockfd, (const char *) package, sizeof(uint32_t) + size, 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
		if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) >= sizeof(uint32_t)){
			memcpy((char*)&received_crc, buffer, sizeof(uint32_t));
		}
	}while((get_cur_time_in_mlsec() - start_sending) < 1000 && crc != received_crc);
}

void send_knobs(){
	char package[16];
	int knobs = get_knobs_value();
	memcpy(package + sizeof(uint32_t), (char*)&knobs, sizeof(knobs));
	send_package(package, sizeof(knobs));
}

unsigned int receive_package(){
	unsigned int msg_len, sender_addr_len;
	uint32_t crc, received_crc = 0;
	if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &sender_addr, &sender_addr_len)) >= 4){
		memcpy((char*)&received_crc, buffer, sizeof(uint32_t));
		crc = crc_32((const unsigned char*)(buffer + sizeof(uint32_t)), msg_len - sizeof(uint32_t));
		if(received_crc == crc){
			sendto(nw_state.sockfd, (const char *) &crc , sizeof(crc), 0, (const struct sockaddr *) &sender_addr, sizeof(sender_addr));
			return msg_len;
		}
	}
	return 0;
}

void receive_knobs(){
	unsigned int msg_len = receive_package();
	if(msg_len >= 8){
		memcpy((char*)&received_knobs_value, buffer + sizeof(uint32_t), sizeof(received_knobs_value));
		last_connection_time = get_cur_time_in_mlsec();
	}
}

void initialize_state(){
	nw_state.sending = false;
	nw_state.receiving = true;
	nw_state.connected = false;
	nw_state.find_others = false;
	nw_state.receiver_ip = NULL;
	nw_state.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(nw_state.ready, false, 20);
	init_sender_addr();
	set_receiver_addr(INADDR_ANY);
	received_knobs_value = get_knobs_value();
}

void* network_communication(void *vargp){
	initialize_state();
	last_connection_time = 0;
	
	while(true){
		if(nw_state.sending){
			if(nw_state.connected){
				send_knobs();
			}
			else if(nw_state.find_others){
				send_init_message(1);		//broadcast			
			}else if(nw_state.receiver_ip != NULL){
				set_receiver_addr(inet_addr(nw_state.receiver_ip));
				send_connection_message(2); //set connection with one of the desks
			}
		}else if(nw_state.receiving){
			if(nw_state.connected){
				receive_knobs();
			}
			else{
				receive_init_message();
			}
		}
		if(get_cur_time_in_mlsec() - last_connection_time > DISCONNECT){
			nw_state.connected = false;
		}
	}
    close(nw_state.sockfd);
}
