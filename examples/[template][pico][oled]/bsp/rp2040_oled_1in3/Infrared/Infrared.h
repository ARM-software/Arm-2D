#ifndef __INFRARED_H
#define __INFRARED_H

#include "DEV_Config.h"

 

extern int Infrared_PIN;

void SET_Infrared_PIN(uint8_t PIN);

int   Read_Infrared_Value(void);



#endif
