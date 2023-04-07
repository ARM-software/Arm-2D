/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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
#include "./arm_extra_controls.h"

#include "arm_2d_helper.h"

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
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void arm_extra_controls_init(void)
{
    arm_2d_helper_init();

    busy_wheel_init();
    spinning_wheel_init();
    progress_bar_drill_init();
    progress_bar_flowing_init();
    progress_bar_simple_init();
}


void arm_2d_draw_box(const arm_2d_tile_t *ptTarget,
                     const arm_2d_region_t *ptRegion,
                     int16_t iBorderWidth, 
                     COLOUR_INT tColour,
                     uint8_t chOpacity)
{
    assert( NULL != ptTarget );
    if (NULL == ptRegion) {
        ptRegion = &(ptTarget->tRegion);
    }
    
    arm_2d_region_t tDrawRegion = *ptRegion;
    
    tDrawRegion.tSize.iHeight = iBorderWidth;
    
    /* draw the top horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion.tLocation.iY += ptRegion->tSize.iHeight - iBorderWidth;
    
    /* draw the bottom horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion = *ptRegion;
    
    /* draw left vertical line */
    tDrawRegion.tSize.iWidth = iBorderWidth;
    tDrawRegion.tLocation.iY += iBorderWidth;
    tDrawRegion.tSize.iHeight -= iBorderWidth * 2;

    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
    
    /* draw right vertical line */
    tDrawRegion.tLocation.iX += ptRegion->tSize.iWidth - iBorderWidth;
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    chOpacity);
    
    arm_2d_op_wait_async(NULL);
}



#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
