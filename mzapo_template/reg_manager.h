#ifndef _REG_MANAGER_H_
#define _REG_MANAGER_H_

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "led_control.h"


unsigned char *mem_base;
uint32_t *knobs_mem_base;

int initialize_adresses();
int get_knobs_value();

#endif
