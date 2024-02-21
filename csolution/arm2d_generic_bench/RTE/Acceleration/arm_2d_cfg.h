/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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

#ifndef __ARM_2D_USER_CFG_H__
#define __ARM_2D_USER_CFG_H__

/*============================ INCLUDES ======================================*/

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Arm-2D General Configurations
// =======================
//
// <q>Enable Asynchronous Programmers' model support
// <i> Note that if you don't have any hardware accelerator, disable this feature can reduce code size and gain a small performance uplift.
// <i> This feature is enabled by default.
#ifndef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                                     0
#endif

// <q>Enable anti-alias support for all tranform operations.
// <i> Note that enabling this feature suffers a non-negligible performance drop.
// <i> This feature is disabled by default.
#ifndef __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
#   define __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__                      1
#endif

// <q>Enable support for accessing individual colour channels
// <i> Note that enabling this feature will add the support for a special colour type: ARM_2D_CHANNEL_8in32
// <i> This feature is disabled by default to save code size
#ifndef __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__             1
#endif
// </h>

// <h>Log and Debug
// =======================
// <q>Enable Log
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_ENABLE_LOG__
#   define __ARM_2D_CFG_ENABLE_LOG__                                0
#endif

// <q>The terminal support colour
// <i> The terminal is compatible with VT100 and support colour display. This feature is disabled by default.
#ifndef __ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__
#   define __ARM_2D_CFG_LOG_OUTPUT_SUPPORT_COLOUR__                 0
#endif

// <o>The maximum length of log string <64-65535>
// <i> The number of bytes requested from heap during log output
// <i> Default: 256
#ifndef __ARM_2D_LOG_MAX_STRING_LEN__
#   define __ARM_2D_LOG_MAX_STRING_LEN__        256
#endif

/* The filter of log channels. Please comment the channels that you want to mask.
 */
#ifndef __ARM_2D_LOG_CHANNEL_MASK_FILTER__
#   define __ARM_2D_LOG_CHANNEL_MASK_FILTER__                                   \
            (   ARM_2D_LOG_CHN_TYPE_USER                                        \
            |   ARM_2D_LOG_CHN_TYPE_INFO                                        \
            |   ARM_2D_LOG_CHN_TYPE_WARNING                                     \
            |   ARM_2D_LOG_CHN_TYPE_ERROR                                       \
            |   ARM_2D_LOG_CHN_PIPELINE                                         \
            |   ARM_2D_LOG_CHN_OPCODE                                           \
            |   ARM_2D_LOG_CHN_HELPER                                           \
            |   ARM_2D_LOG_CHN_HELPER_PFB                                       \
            |   ARM_2D_LOG_CHN_SCENE_PLAYER                                     \
            |   ARM_2D_LOG_CHN_DIRTY_REGION_OPTIMISATION                        \
            |   ARM_2D_LOG_CHN_STATISTICS                                       \
            |   ARM_2D_LOG_CHN_APP)
#endif
// </h>

// <h>Patches for improving performance
// =======================
// 
// <c1> Do NOT treat alpha value 255 as completely opaque in mask related operations
// <i> When define this macro, alpha value 0xFF will not be treated as opaque in mask related operations and you can barely see the background. Defining this macro improves performance.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__  
// </c>

// <c1> Ignore calibrartion for small angles in transform operations
// <i> This option is used to speed up M-cores without DSP support. It skips saturation in the QADD/QDADD/QDSUB involved in the rotation. The chances of overflow remain low as elements involved are using non-accumulating Q15.16 format and integer parts are in the range of the screen size providing enough margin.
//#define __ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__ 
// </c>

// <c1> Ignore satuation protection in fixed-point operations
// <i> This option is used to remove calibration in angle computations to gain a better performance, small error might be noticible for angles like 90, 180, 270 etc.
//#define __ARM_2D_CFG_UNSAFE_NO_SATURATION_IN_FIXED_POINT__ 
// </c>


// <q> Optimize the scaler version of transform operations for pointer-like resources
// <i> This feature is enabled by default. There is no guarantee that the performance will increase or decrease. It is all depends your applications. In most of the case, enabling it helps.
// <i> This feature has no meaning when the anti-alias transform is disabled or the helium acceleration is available.
#ifndef __ARM_2D_CFG_OPTIMIZE_FOR_POINTER_LIKE_SHAPES_IN_TRANSFORM__
#   define __ARM_2D_CFG_OPTIMIZE_FOR_POINTER_LIKE_SHAPES_IN_TRANSFORM__     1
#endif

// <q> Optimize the scaler version of transform operations for hollow out masks
// <i> This feature is disabled by default. There is no guarantee that the performance will increase or decrease. It is all depends your applications. If your application uses a lot of hollow out masks, it might help.
// <i> This feature has no meaning when the anti-alias transform is disabled or the helium acceleration is available.
#ifndef __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
#   define __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__         0
#endif

// </h>


// <h>Extra Components
// =======================
//
#ifndef __GLCD_CFG_COLOUR_DEPTH__
// <o __GLCD_CFG_COLOUR_DEPTH__> Select the screen colour depth
//     <8=>     8 Bits
//     <16=>    16Bits
//     <32=>    32Bits
// <i> The colour depth of your LCD
// <i> Default: 16
#   define __GLCD_CFG_COLOUR_DEPTH__                                    16
#endif

// <o> The size of the LCD printf text buffer <16-65535>
// <i> The text buffer size for the lcd printf service. It determins how many character you can use in one printf string.
#ifndef __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__
#   define __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__                          64
#endif

// <h>Benchmark

// <o>Width of the screen <8-32767>
// <i> The width of your screen for running benchmark
// <i> Default: 320
#ifndef __GLCD_CFG_SCEEN_WIDTH__
#   define __GLCD_CFG_SCEEN_WIDTH__                                     320
#endif

// <o>Height of the screen <8-32767>
// <i> The height of your screen for running benchmark
// <i> Default: 240
#ifndef __GLCD_CFG_SCEEN_HEIGHT__
#   define __GLCD_CFG_SCEEN_HEIGHT__                                    240
#endif

// <o>Number of iterations <1-2000>
// <i> run number of iterations in arm-2d benchmark before calculating the result.
#ifndef ITERATION_CNT
#   define ITERATION_CNT                                                1000
#endif

// <q>Use Tiny mode to run benchmark
// <i> Enable this mode to reduce the benchmark memory footprint (removing background picture etc.)
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_BENCHMARK_TINY_MODE__
#   define __ARM_2D_CFG_BENCHMARK_TINY_MODE__                           0
#endif

// <q> Enable Stopwatch mode in the Benchmark:Watch-panel
// <i> Only update the second-hand (i.e. red pointer) every second in the watch-panel demo
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_WATCH_PANEL_STOPWATCH_MODE__
#   define __ARM_2D_CFG_WATCH_PANEL_STOPWATCH_MODE__                    0
#endif

// <q> Enable the nebula effect mode in the Benchmark:Watch-panel
// <i> This feature is disabled by default and it is only available in the Tiny mode.
#ifndef __ARM_2D_CFG_BENCHMARK_WATCH_PANEL_USE_NEBULA__
#   define __ARM_2D_CFG_BENCHMARK_WATCH_PANEL_USE_NEBULA__              0
#endif

// <q> Exit benchmark when finished
// <i> Exit the arm_2d_run_benchmark() after running specified iterations
// <i> This feature is disabled by default.
#ifndef __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__
#   define __ARM_2D_CFG_BENCHMARK_EXIT_WHEN_FINISH__                    0
#endif

//</h>
// </h>

// <<< end of configuration section >>>


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#ifdef   __cplusplus
}
#endif

#endif
