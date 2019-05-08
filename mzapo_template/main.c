/*******************************************************************
  Simple program to check LCD functionality on MicroZed
  based MZ_APO board designed by Petr Porazil at PiKRON

  mzapo_lcdtest.c       - main and only file

  (C) Copyright 2004 - 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      work:     http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

int main(int argc, char *argv[])
{
  uint16_t color;
  uint32_t *knobs = map_phys_address(SPILED_REG_BASE_PHYS + SPILED_REG_KNOBS_8BIT_o, 4, false);
  unsigned char *parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  parlcd_hx8357_init(parlcd_mem_base);
  parlcd_write_cmd(parlcd_mem_base, 0x2c);

  while(true){
    color = *knobs;
    printf("%x\n", *knobs);
    //parlcd_write_data2x(parlcd_mem_base, color);
	parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int i = 0; i < 320 ; i++) {
      for (int j = 0; j < 480 ; j++) {
        parlcd_write_data2x(parlcd_mem_base, j < 420 ? color : 0);
      }
    }

  }

  return 0;
}
