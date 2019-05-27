#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "reg_manager.h"

int initialize_adresses(){
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (mem_base == NULL) return 1;
	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_mem_base == NULL) return 1;
	parlcd_hx8357_init(parlcd_mem_base);
	knobs_mem_base = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
	led1.mem_base = mem_base + SPILED_REG_LED_RGB1_o;
	led2.mem_base = mem_base + SPILED_REG_LED_RGB2_o;
	return 0;
}


