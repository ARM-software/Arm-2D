/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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
#define __PROGRESS_BAR_ROUND_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "progress_bar_round.h"
#include <assert.h>
#include <string.h>

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
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void progress_bar_round_init( progress_bar_round_t *ptThis,
                          progress_bar_round_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(progress_bar_round_t));
    this.tCFG = *ptCFG;


}

ARM_NONNULL(1)
void progress_bar_round_depose( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void progress_bar_round_on_load( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void progress_bar_round_on_frame_start( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void progress_bar_round_on_frame_complete( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1, 2)
void progress_bar_round_show(   progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __progress_bar, ptRegion) {

        arm_2d_dock_vertical(__progress_bar_canvas, this.tCFG.ptCircleMask->tRegion.tSize.iHeight) {

            draw_round_corner_box(  &__progress_bar, 
                                    &__vertical_region, 
                                    tBackgroundColour, 
                                    chOpacity);

        }

    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
