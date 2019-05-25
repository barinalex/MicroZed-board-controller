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

#include "connection.h"
#include "led_control.h"
#include "utils.h"
#include "reg_manager.h"

char buffer[BUFSIZE];
struct sockaddr_in sender_addr, receiver_addr;
unsigned long last_connection_time;

void init_sender_addr(){
    sender_addr.sin_family    = AF_INET; // IPv4
    sender_addr.sin_addr.s_addr = INADDR_ANY;
    sender_addr.sin_port = htons(SENDER_PORT);
}

int init_socket(){
   	if ((nw_state.sockfd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr, "Socket error!\n");
        return 1;
    }
    int yes=1;
	if (setsockopt(nw_state.sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
	 	fprintf(stderr, "Setsockopt error!\n");
        return 1;
	}
    if (bind(nw_state.sockfd, (const struct sockaddr*)&sender_addr, sizeof(sender_addr)) < 0){
        fprintf(stderr, "Binding error!\n");
        return 1;
    }
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = TIMEOUT;
    if (setsockopt(nw_state.sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "Timeout error!\n");
        return 1;
    }
    return 0;
}

int init_broad_socket(){
   	if ((nw_state.broad_socket = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr, "Socket error!\n");
        return 1;
    }
	int yes=1;
	if (setsockopt(nw_state.broad_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
	 	fprintf(stderr, "Setsockopt error!\n");
        return 1;
	}
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = TIMEOUT;
    if (setsockopt(nw_state.broad_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "Timeout error!\n");
        return 1;
    }
    int so_broadcast = 1;
    if (setsockopt(nw_state.broad_socket, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast) < 0){
        fprintf(stderr, "Timeout error!\n");
        return 1;
    }
    return 0;
}

void set_receiver_addr(unsigned long IP){
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = IP;
    receiver_addr.sin_port = htons(RECEIVER_PORT);
}

void send_connection_message(char message){
	int msg_len;
	unsigned int receiver_addr_len;
	printf("Try to connect\n");
	
	sendto(nw_state.sockfd, (const char *) &message, sizeof(char), 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
	//if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) > 0){
		printf("ack received\n");
		nw_state.connected = true;
	//}
	
	/*
	unsigned long start_sending = get_cur_time_in_mlsec();
	do{
		sendto(nw_state.sockfd, (const char *) &message, sizeof(char), 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
		if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len))){
			printf("sender received_form_ip: %s\n", inet_ntoa(receiver_addr.sin_addr));
			if(buffer[0] == '1'){
				nw_state.connected = true;
			}
		}
	}while((get_cur_time_in_mlsec() - start_sending) < 1000);
	*/
}

void receive_ip(int index){
	int msg_len;
	unsigned int receiver_addr_len;
	if((msg_len = recvfrom(nw_state.broad_socket, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) > 0){
	printf("receive ip: %c\n", buffer[0]);
	printf("ip: %s\n", inet_ntoa(receiver_addr.sin_addr));
	if(buffer[0] == '1'){
			strcpy(nw_state.ip_addr[index], inet_ntoa(receiver_addr.sin_addr));
			printf("ip: %s\n", nw_state.ip_addr[index]);
			nw_state.ready[index] = true;
		}
	}
}

void send_init_message(char message){
	printf("%c\n",message);
	unsigned int index = 0;
	unsigned int receiver_addr_len;
	int msg_len;
	sendto(nw_state.broad_socket, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
	
	if((msg_len = recvfrom(nw_state.broad_socket, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) > 0){
		printf("receive: %c\n", buffer[0]);
		printf("ip: %s\n", inet_ntoa(receiver_addr.sin_addr));
		if(buffer[0] == '1'){
			strcpy(nw_state.ip_addr[index], inet_ntoa(receiver_addr.sin_addr));
			printf("ip: %s\n", nw_state.ip_addr[index]);
			nw_state.ready[index++] = true;
		}
	}
	receive_ip(index++);
	receive_ip(index++);
	receive_ip(index++);
	receive_ip(index++);
	nw_state.find_others = false;
}

void receive_init_message(){
	unsigned int sender_addr_len;
	int msg_len;
	char message = '1';
	srand(time(NULL));
	if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &sender_addr, &sender_addr_len)) > 0){
		printf("receive: %c\n", buffer[0]);
		switch(buffer[0]){
			case '1':
				//usleep((rand() % 180) * 1000);
				printf("send ack to ip: %s\n", inet_ntoa(sender_addr.sin_addr));
				sendto(nw_state.sockfd, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &sender_addr, sizeof(sender_addr));
				break;
			case '2':
				nw_state.connected = true;
				printf("send ack to ip: %s\n", inet_ntoa(sender_addr.sin_addr));
				sendto(nw_state.sockfd, (const char *) &message, sizeof(message), 0, (const struct sockaddr *) &sender_addr, sizeof(sender_addr));
				break;
			case '3':
				nw_state.connected = true;
				nw_state.copy = true;
				break;
		}
	}
}

void send_package(char *package, int size){
	unsigned int receiver_addr_len;
	int msg_len;
	uint32_t crc = 0, received_crc = 0;
	//memcpy(package, (char*)&crc, sizeof(crc));
	unsigned long start_sending = get_cur_time_in_mlsec();
	do{
		sendto(nw_state.sockfd, (const char *) package, sizeof(uint32_t) + size, 0, (const struct sockaddr *) &receiver_addr, sizeof(receiver_addr));
		if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &receiver_addr, &receiver_addr_len)) >= sizeof(uint32_t)){
			memcpy((char*)&received_crc, buffer, sizeof(uint32_t));
			//printf("received_form_ip: %s\n", inet_ntoa(receiver_addr.sin_addr));
			//printf("crc: %d / received crc: %d\n", crc, received_crc);
		}
	}while((get_cur_time_in_mlsec() - start_sending) < 1000 && crc != received_crc);
}

void send_knobs(){
	char package[16];
	int knobs = get_knobs_value();
	uint32_t crc = 0;
	memcpy(package, (char*)&crc, sizeof(crc));
	memcpy(package + sizeof(uint32_t), (char*)&knobs, sizeof(knobs));
	send_package(package, sizeof(knobs));
}

void send_leds(){
	char package[1024];
	uint32_t crc = 1;
	memcpy(package, (char*)&crc, sizeof(crc));
	memcpy(package + sizeof(uint32_t), (char*)&led1, sizeof(led1));
	memcpy(package + sizeof(uint32_t) + sizeof(led1), (char*)&led2, sizeof(led2));
	send_package(package, sizeof(led1) + sizeof(led2));
	nw_state.connected = false;
	nw_state.copy = false;
	nw_state.sending = false;
	nw_state.receiving = true;
}

unsigned int receive_package(){
	unsigned int sender_addr_len;
	int msg_len;
	uint32_t crc = 0, received_crc = 0;
	if((msg_len = recvfrom(nw_state.sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &sender_addr, &sender_addr_len)) >= 4){
		memcpy((char*)&received_crc, buffer, sizeof(uint32_t));
		if(received_crc < 2){
			sendto(nw_state.sockfd, (const char *) &crc , sizeof(crc), 0, (const struct sockaddr *) &sender_addr, sizeof(sender_addr));
			return msg_len;
		}
	}
	return 0;
}

void receive(){
	int msg_len = receive_package();
	uint32_t received_crc;
	if(msg_len >= 8){
		memcpy((char*)&received_crc, buffer, sizeof(uint32_t));
		switch(received_crc){
			case 0:
				memcpy((char*)&received_knobs_value, buffer + sizeof(uint32_t), sizeof(received_knobs_value));
				last_connection_time = get_cur_time_in_mlsec();
				break;
			case 1:
				memcpy((char*)&led1, buffer + sizeof(uint32_t), sizeof(led1));
				memcpy((char*)&led2, buffer + sizeof(uint32_t) + sizeof(led1), sizeof(led2));
				led1.mem_base = mem_base + SPILED_REG_LED_RGB1_o;
				led2.mem_base = mem_base + SPILED_REG_LED_RGB2_o;
				set_last_time();
				nw_state.copy = false;
				nw_state.connected = false;
				break;
		}
	}
	if(get_cur_time_in_mlsec() - last_connection_time > 200){
		nw_state.connected = false;
	}
}

void initialize_state(){
	nw_state.sending = false;
	nw_state.copy = false;
	nw_state.receiving = true;
	nw_state.connected = false;
	nw_state.find_others = false;
	nw_state.receiver_ip = NULL;
	nw_state.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(nw_state.ready, false, 20);
	init_sender_addr();
	init_socket();
	init_broad_socket();
	received_knobs_value = get_knobs_value();
}

void sending(){
	
}

void* network_communication(void *vargp){
	initialize_state();
	last_connection_time = 0;
	while(true){
		if(nw_state.sending){
			if(nw_state.connected){
				if(!nw_state.copy){
					send_knobs();
				}
				else{
					send_leds();
				}
			}
			else if(nw_state.find_others){
				set_receiver_addr(INADDR_BROADCAST);
				send_init_message('1');		//broadcast			
			}else if(nw_state.receiver_ip != NULL){
				printf("send to ip: %s\n", nw_state.rec_ip);
				set_receiver_addr(inet_addr(nw_state.rec_ip));
				if(nw_state.copy){
					send_connection_message('3'); //set copy connection with one of the desks
				}else{
					send_connection_message('2'); //set control connection with one of the desks
				}
			}
		}else if(nw_state.receiving){
			if(nw_state.connected){
				receive();
			}
			else{
				receive_init_message();
			}
		}
		if(get_cur_time_in_mlsec() - last_connection_time > DISCONNECT){
			//nw_state.connected = false;
		}
	}
    close(nw_state.sockfd);
}
