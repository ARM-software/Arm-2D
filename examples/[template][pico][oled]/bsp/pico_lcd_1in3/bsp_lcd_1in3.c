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
#include "./LCD/LCD_1In3.h"
#include "./GLCD_Config.h"

#include "./LCD/LCD_1in3.c"
#include "./Config/DEV_Config.c"

/*============================ MACROS ========================================*/
#define TOP         (0x1FFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ABS
#   define ABS(__N)    ((__N) < 0 ? -(__N) : (__N))
#endif
#ifndef _BV
#   define _BV(__N)    ((uint32_t)1<<(__N))
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

__attribute__((used))
void __pico_bsp_use_lcd_1in3__(void)
{}

/*! \brief set the 16-level led gradation
 *! \param hwLevel gradation
 *! \return none
 */
static
void set_led_gradation(uint16_t hwLevel)
{
    static uint16_t s_hwCounter = 0;
    
    if (hwLevel >= s_hwCounter) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    } else {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }
    
    s_hwCounter++;
    s_hwCounter &= TOP;
}

void breath_led(void)
{
    static uint16_t s_hwCounter = 0;
    static int16_t s_nGray = (TOP >> 1);
    
    s_hwCounter++;
    if (!(s_hwCounter & (_BV(10)-1))) {
        s_nGray++; 
        if (s_nGray == TOP) {
            s_nGray = 0;
        }
    }
    
    set_led_gradation(ABS(s_nGray - (TOP >> 1)));
}


void bsp_init(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    DEV_Delay_ms(100);

    if(DEV_Module_Init()!=0){
        //assert(0);
    }
   
    DEV_SET_PWM(50);
    /* LCD Init */
    
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(GLCD_COLOR_BLUE);
    
    for (int n = 0; n < KEY_NUM; n++) {
        dev_key_init(n);
    }
}
