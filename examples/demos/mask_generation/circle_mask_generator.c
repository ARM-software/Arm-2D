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
#define __CIRCLE_MASK_GENERATOR_IMPLEMENT__
#include "circle_mask_generator.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)


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
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
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
void circle_mask_generator_init( circle_mask_generator_t *ptThis,
                          circle_mask_generator_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    memset(ptThis, 0, sizeof(circle_mask_generator_t));

    //if (NULL != ptCFG) {
        this.tCFG = *ptCFG;
    //}


}

ARM_NONNULL(1)
void circle_mask_generator_depose( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void circle_mask_generator_on_load( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void circle_mask_generator_on_frame_start( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void circle_mask_generator_on_frame_complete( circle_mask_generator_t *ptThis)
{
    assert(NULL != ptThis);
    
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif