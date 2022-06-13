/*-----------------------------------------------------------------------------
 * Copyright (c) 2015 Arm Limited (or its affiliates). All
 * rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1.Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   2.Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   3.Neither the name of Arm nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * Name:    GLCD_Config.h
 * Purpose: Graphic LCD interface configuration file for STM32F746G Discovery Kit
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

#ifndef __GLCD_CONFIG_H
#define __GLCD_CONFIG_H

/*---------------------- Graphic LCD orientation configuration ---------------*/
#ifndef GLCD_LANDSCAPE
#define GLCD_LANDSCAPE  1
#endif

/*---------------------- Graphic LCD physical definitions --------------------*/
#define GLCD_SIZE_X     480             /* Screen size X (in pixels) */
#define GLCD_SIZE_Y     272             /* Screen size Y (in pixels) */
#define GLCD_BPP        16              /* Bits per pixel            */

#if    (GLCD_LANDSCAPE != 0)
#define GLCD_WIDTH      GLCD_SIZE_X     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_Y     /* Screen Height (in pixels) */
#else
#define GLCD_WIDTH      GLCD_SIZE_Y     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_X     /* Screen Height (in pixels) */
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
