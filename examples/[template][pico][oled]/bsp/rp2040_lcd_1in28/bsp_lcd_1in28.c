/****************************************************************************
*  Copyright 2023 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "pico/stdlib.h"


#include "./Config/DEV_Config.h"
#include "./LCD/LCD_1In28.h"
#include "./GLCD_Config.h"

#include "./LCD/LCD_1in28.c"
#include "./Config/DEV_Config.c"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

__attribute__((used))
void __rp2040_bsp_use_lcd_1in28__(void)
{}


void breath_led(void)
{

}


void bsp_init(void)
{

    DEV_Delay_ms(100);

    if(DEV_Module_Init()!=0){
        //assert(0);
    }

    /* LCD Init */
    
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(GLCD_COLOR_BLUE);
    DEV_SET_PWM(60);
}
