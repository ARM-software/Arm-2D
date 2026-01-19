#ifndef zhRGB565_DECODER_H
#define zhRGB565_DECODER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif
#if defined(RTE_Acceleration_Arm_2D)
#   include "arm_2d_helper.h"
#   undef this
#   define this (*ptThis)
#endif
#if defined(RTE_Acceleration_Arm_2D_Extra_Loader)
#	include "arm_zhrgb565_cfg.h"
#endif


// RGB565颜色分量提取
#define GET_R(color) 		(((color) >> 11) & 0x1F)
#define GET_G(color) 		(((color) >> 5) & 0x3F)
#define GET_B(color) 		((color) & 0x1F)
#define RGB565(r, g, b) 	((((r) & 0x1F) << 11) | (((g) & 0x3F) << 5) | ((b) & 0x1F))
#define	GET_RGB332_TO_RGB565(diffval)		(((uint16_t)(diffval) & 0xE0) << 6) | (((uint16_t)(diffval) & 0x1C) << 3) | ((uint16_t)(diffval) & 0x03)

/* 获取图片 - 宽度 */
#define     GET_RGB565_ENCODER_WIDTH(BUF)				((BUF)[0])
/* 获取图片 - 高度 */
#define     GET_RGB565_ENCODER_HEIGHT(BUF)				((BUF)[1])
/* 获取图片 - 编码标志 */
#define     GET_RGB565_ENCODER_FLAG(BUF)				((BUF)[2])
/* 获取图片 - 升阶表长度 */
#define     GET_RGB565_ENCODER_SJB_LENGHT(BUF)          ((BUF)[3])
/* 获取图片 - 行偏移表起点地址 */
#define     GET_RGB565_ENCODER_LINE_POS(BUF)			((BUF)[4])
/* 获取图片 - 编码数据起点地址 */
#define     GET_RGB565_ENCODER_DATA_ADDR(BUF)			((BUF)[5])
/* 获取图片 - 获取升阶表第N个数据的行号,N=0,1,2...*/
#define     GET_RGB565_ENCODER_SJB_DATA(BUF, N)			((BUF)[6+N])
/* 获取图片 - 第N行编码数据在行偏移表的地址 ,N=0,1,2...*/
#define     GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N)	((BUF)[GET_RGB565_ENCODER_LINE_POS(BUF) + (N)])
/* 获取图片 - 图片第N行编码数据的第M个数据 ,N=0,1,2...*/
#define     GET_RGB565_ENCODER_LINE_DATA(BUF,N,M)		((BUF)[GET_RGB565_ENCODER_DATA_ADDR(BUF) + GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N) + (M)])
#define     GET_RGB565_ENCODER_LINE_DATA2(BUF,N,M)		((BUF)[(N) + (M)])


void zhRGB565_decompress_baseversion(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height,const uint16_t *src, uint16_t *buf);

#if defined(RTE_Acceleration_Arm_2D)
void zhRGB565_decompress_for_arm2d(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, const uint16_t *src, COLOUR_INT *buf, int16_t iTargetStride);
#endif

#endif

