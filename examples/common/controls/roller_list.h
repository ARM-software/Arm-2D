#ifndef __ROLLER_LIST_H__
#define __ROLLER_LIST_H__

#include "arm_extra_controls.h"

extern int8_t roller_switch;
#define ROLLER_LIST_INC	if(roller_switch==0)roller_switch = 1;
#define ROLLER_LIST_DEC	if(roller_switch==0)roller_switch = -1;


void roller_list_draw(  const arm_2d_tile_t *ptTarget, 
                        const arm_2d_region_t *ptRegion,
                        bool bIsNewFrame);












#endif
