/*******************************************************************
  Simple program to check LCD functionality on MicroZed

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#define BLACK 0x0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define DELAY 200000

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "reg_manager.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcdframe.h"
#include "utils.h"
#include "menu.h"
#include "led_control.h"
#include "connection.h"

typedef struct{
	unsigned long start_time; // = get_cur_time()
	unsigned long cur_time;
}flash_time_;

uint16_t frame[FRAME_H][FRAME_W];

int main(int argc, char *argv[])
{
	if(initialize_adresses() != 0) exit(1);
   	pthread_t thread_id; 
    pthread_create(&thread_id, NULL, led_thread, (void *)&thread_id);
    pthread_create(&thread_id, NULL, network_communication, (void *)&thread_id);
    pthread_create(&thread_id, NULL, create_menu, (void *)&thread_id);
    pthread_exit(NULL);
	return 0;
}

