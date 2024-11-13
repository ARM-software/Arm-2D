#ifndef _VIRTUAL_TFT_PORT_H_
#define _VIRTUAL_TFT_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "arm_2d_disp_adapter_0.h"

#if     __DISP0_CFG_ROTATE_SCREEN__ == 1\
    ||  __DISP0_CFG_ROTATE_SCREEN__ == 3
#   define VT_WIDTH           __DISP0_CFG_SCEEN_HEIGHT__
#   define VT_HEIGHT          __DISP0_CFG_SCEEN_WIDTH__
#else
#   define VT_WIDTH           __DISP0_CFG_SCEEN_WIDTH__
#   define VT_HEIGHT          __DISP0_CFG_SCEEN_HEIGHT__
#endif

#if __DISP0_CFG_COLOR_SOLUTION__ == 0
#   define VT_COLOR_DEPTH     __DISP0_CFG_COLOUR_DEPTH__
#elif __DISP0_CFG_COLOR_SOLUTION__ == 1         /* monochrome */
#   define VT_COLOR_DEPTH     1
#endif


#define VT_VIRTUAL_MACHINE 0                   /*Different rendering should be used if running in a Virtual machine*/

#if VT_COLOR_DEPTH == 1 || VT_COLOR_DEPTH == 8 || VT_COLOR_DEPTH == 16 || VT_COLOR_DEPTH == 24 || VT_COLOR_DEPTH == 32
#if VT_COLOR_DEPTH == 1 || VT_COLOR_DEPTH == 8
typedef uint8_t color_typedef;
#elif VT_COLOR_DEPTH == 16
typedef uint16_t color_typedef;
#elif VT_COLOR_DEPTH == 24 || VT_COLOR_DEPTH == 32
typedef uint32_t color_typedef;
#endif
#else
#error "Invalid VT_COLOR_DEPTH in Virtual_TFT_Port.h"
#endif


extern void VT_init(void);
extern bool VT_is_request_quit(void);
extern void VT_deinit(void);
extern void VT_sdl_flush(int32_t nMS);
extern void VT_sdl_refresh_task(void);
extern void VT_enter_global_mutex(void);
extern void VT_leave_global_mutex(void);

/*******************************************************************************
 * @name     :VT_Mouse_Get_Point
 * @brief    :get mouse click position
 * @param    :x       pointer,save click position x
 *            y       pointer,save click position y
 * @return   :true    press
 *            false   relase
 * @version  :V0.1
 * @author   :
 * @date     :2018.11.20
 * @details  :
*******************************************************************************/
extern
bool VT_mouse_get_location(arm_2d_location_t *ptLocation);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_VIRTUAL_TFT_PORT_H_
