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

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <math.h>

#include "__common.h"

#include "./spinning_wheel.h"
#include "./shape_round_corner_box.h"
#include <time.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Winitializer-overrides"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#endif

/*============================ MACROS ========================================*/
#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_gray8_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_gray8_t

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_rgb565_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_rgb565_t

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_cccn888_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_cccn888_t

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern uint32_t SystemCoreClock;

extern const arm_2d_tile_t c_tileSpinWheelMask;

/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/

ARM_NOINIT static uint8_t s_tileSpinWheelMaskBuffer[61 * 61];
const static arm_2d_tile_t s_tileSpinWheelMask = {
    .pchBuffer = (uint8_t *)s_tileSpinWheelMaskBuffer,
    .tRegion = {
        .tSize = {61, 61},
    },
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
};

/*============================ IMPLEMENTATION ================================*/


void spinning_wheel_init(void)
{

}

void spinning_wheel_show(const arm_2d_tile_t *ptTarget, bool bIsNewFrame)
{
    assert(NULL != ptTarget);
    ARM_2D_UNUSED(ptTarget);
    ARM_2D_UNUSED(bIsNewFrame);
    static float s_fAngle = 0.0f;

    arm_2d_align_centre(ptTarget->tRegion, 100, 100) {
        
        draw_round_corner_box(  ptTarget, 
                                &__centre_region,
                                GLCD_COLOR_BLACK,
                                64,
                                bIsNewFrame);    
        arm_2d_op_wait_async(NULL);
    }

    //! spin mask
    do {
        static arm_2d_op_rotate_t s_tMaskRotateCB = {0};
    
        if (bIsNewFrame) {
            s_fAngle += ARM_2D_ANGLE(6.0f);
            s_fAngle = fmodf(s_fAngle,ARM_2D_ANGLE(360));
        }

        const arm_2d_location_t c_tCentre = {
            .iX = 30,
            .iY = 30,
        };

        memset(s_tileSpinWheelMask.pchBuffer, 0, 61 * 61);
        
        arm_2dp_gray8_tile_rotation(&s_tMaskRotateCB,
                                    &c_tileSpinWheelMask,
                                    &s_tileSpinWheelMask,
                                    NULL,
                                    c_tCentre,
                                    s_fAngle,
                                    0x00);

        arm_2d_op_wait_async(NULL);
    } while(0);

    arm_2d_align_centre(ptTarget->tRegion, s_tileSpinWheelMask.tRegion.tSize) {
        arm_2d_fill_colour_with_mask(
                                ptTarget, 
                                &__centre_region, 
                                &s_tileSpinWheelMask, 
                                (__arm_2d_color_t){GLCD_COLOR_WHITE});
        arm_2d_op_wait_async(NULL);
    }

}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
