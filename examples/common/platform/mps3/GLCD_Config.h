/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*-----------------------------------------------------------------------------
 * File     GLCD_Config.h
 * Brief    Graphic LCD interface configuration file for V2M-MPS3 evaluation board
 * Version  1.0.0
 *----------------------------------------------------------------------------*/

#ifndef __GLCD_CONFIG_H
#define __GLCD_CONFIG_H

/*---------------------- Graphic LCD orientation configuration ---------------*/
#ifndef GLCD_MIRROR_X
#define GLCD_MIRROR_X   1               /* Mirror X axis = 1:yes, 0:no */
#endif
#ifndef GLCD_MIRROR_Y
#define GLCD_MIRROR_Y   0               /* Mirror Y axis = 1:yes, 0:no */
#endif
#ifndef GLCD_SWAP_XY
#define GLCD_SWAP_XY    1               /* Swap X&Y axis = 1:yes, 0:no */
#endif

/*---------------------- Graphic LCD physical definitions --------------------*/
#define GLCD_SIZE_X     240             /* Screen size X (in pixels) */
#define GLCD_SIZE_Y     320             /* Screen size Y (in pixels) */
#define GLCD_BPP        16              /* Bits per pixel            */

#if    (GLCD_SWAP_XY)
#define GLCD_WIDTH      GLCD_SIZE_Y     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_X     /* Screen Height (in pixels) */
#else
#define GLCD_WIDTH      GLCD_SIZE_X     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_Y     /* Screen Height (in pixels) */
#endif

/*---------------------- Graphic LCD color definitions -----------------------*/
/* Color coding (16-bit):
     15..11 = R4..0 (Red)
     10..5  = G5..0 (Green)
      4..0  = B4..0 (Blue)
*/

#if defined(__USE_RGB32__)
#   define __RGB(__R, __G, __B)    ((((uint32_t)(__R) & 0xFF) << 16)   |        \
                                    (((uint32_t)(__G) & 0xFF) << 8)   |         \
                                    (((uint32_t)(__B) & 0xFF) << 0)  |          \
                                    (uint32_t)0xFF << 24)
#else   
#   define __RGB(__R, __G, __B)    ((((uint16_t)(__R) & 0xFF) >> 3 << 11)   |   \
                                    (((uint16_t)(__G) & 0xFF) >> 2 << 5)   |    \
                                    (((uint16_t)(__B) & 0xFF) >> 3 << 0)  ) 
#endif

/* GLCD RGB color definitions                            */
#define GLCD_COLOR_BLACK        __RGB(   0,   0,   0  )
#define GLCD_COLOR_NAVY         __RGB(   0,   0, 128  )
#define GLCD_COLOR_DARK_GREEN   __RGB(   0, 128,   0  )
#define GLCD_COLOR_DARK_CYAN    __RGB(   0, 128, 128  )
#define GLCD_COLOR_MAROON       __RGB( 128,   0,   0  )
#define GLCD_COLOR_PURPLE       __RGB( 128,   0, 128  )
#define GLCD_COLOR_OLIVE        __RGB( 128, 128,   0  )
#define GLCD_COLOR_LIGHT_GREY   __RGB( 192, 192, 192  )
#define GLCD_COLOR_DARK_GREY    __RGB( 128, 128, 128  )
#define GLCD_COLOR_BLUE         __RGB(   0,   0, 255  )
#define GLCD_COLOR_GREEN        __RGB(   0, 255,   0  )
#define GLCD_COLOR_CYAN         __RGB(   0, 255, 255  )
#define GLCD_COLOR_RED          __RGB( 255,   0,   0  )
#define GLCD_COLOR_MAGENTA      __RGB( 255,   0, 255  )
#define GLCD_COLOR_YELLOW       __RGB( 255, 255, 0    )
#define GLCD_COLOR_WHITE        __RGB( 255, 255, 255  )

#endif /* __GLCD_CONFIG_H */
