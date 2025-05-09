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
#define __<CONTROL_NAME>_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "user_<control_name>.h"
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
void <control_name>_init( user_<control_name>_t *ptThis,
                          user_<control_name>_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(user_<control_name>_t));
    this.tCFG = *ptCFG;


}

ARM_NONNULL(1)
void <control_name>_depose( user_<control_name>_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void <control_name>_on_load( user_<control_name>_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void <control_name>_on_frame_start( user_<control_name>_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void <control_name>_on_frame_complete( user_<control_name>_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void <control_name>_show( user_<control_name>_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);

    if (bIsNewFrame) {
        int32_t iResult;

        /* generate a cosine wave for opacity */
        arm_2d_helper_time_cos_slider(0, 255, 2000, 0, &iResult, &this.lTimestamp[0]);
        this.chOpacity = (uint8_t)iResult;
    }

    arm_2d_container(ptTile, __control, ptRegion) {
        /* put your drawing code inside here
         *    - &__control is the target tile (please do not use ptTile anymore)
         *    - __control_canvas is the canvas
         */

        /* example code: flash a 50x50 red box in the centre */
        arm_2d_align_centre(__control_canvas, 50, 50) {

            arm_2d_fill_colour_with_opacity(
                &__control,
                &__centre_region,
                (__arm_2d_color_t) {GLCD_COLOR_RED},
                this.chOpacity
            );

            /* make sure the operation is complete */
            ARM_2D_OP_WAIT_ASYNC();
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
