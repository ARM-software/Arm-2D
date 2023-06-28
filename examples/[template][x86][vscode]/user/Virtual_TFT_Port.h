#ifndef _VIRTUAL_TFT_PORT_H_
#define _VIRTUAL_TFT_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

//
// 虚拟屏幕设定参数,即需要一个什么样的屏幕
//
#define VT_WIDTH           480
#define VT_HEIGHT          272
#define VT_COLOR_DEPTH     16
#define VT_VIRTUAL_MACHINE 0                   /*Different rendering should be used if running in a Virtual machine*/

#if VT_COLOR_DEPTH == 1 || VT_COLOR_DEPTH == 8 || VT_COLOR_DEPTH == 16 || VT_COLOR_DEPTH == 24
#if VT_COLOR_DEPTH == 1 || VT_COLOR_DEPTH == 8
typedef uint8_t color_typedef;
#elif VT_COLOR_DEPTH == 16
typedef uint16_t color_typedef;
#elif VT_COLOR_DEPTH == 24
typedef uint32_t color_typedef;
#endif
#else
#error "Invalid VT_COLOR_DEPTH in Virtual_TFT_Port.h"
#endif




void VT_Init(void);
void VT_Fill_Single_Color(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef color);
void VT_Fill_Multiple_Colors(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef * color_p);
void VT_Set_Point(int32_t x, int32_t y, color_typedef color);
color_typedef VT_Get_Point(int32_t x, int32_t y);
void VT_Clear(color_typedef color);
bool VT_Mouse_Get_Point(int16_t *x,int16_t *y);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_VIRTUAL_TFT_PORT_H_
