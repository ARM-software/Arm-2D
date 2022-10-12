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

#include "./progress_wheel.h"

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

#define c_tileQuaterArc     c_tileQuaterArcGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#define c_tileQuaterArc     c_tileQuaterArcRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#define c_tileQuaterArc     c_tileQuaterArcCCCA8888

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileQuaterArc;
extern const arm_2d_tile_t c_tileQuaterArcMask;
/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/


/*============================ IMPLEMENTATION ================================*/

void progress_wheel_init(void)
{
    
}

ARM_NONNULL(1)
void progress_wheel_show(   const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            int16_t iProgress,
                            const int16_t iDiameter,
                            uint8_t chOpacity,
                            bool bIsNewFrame)
{

    static arm_2d_op_trans_msk_opa_t s_tOP[4];
    static float s_fScale = 1.0f;
    static float s_fAngle = 0.0f;
    
    arm_2d_region_t tRegion = {0};
    if (NULL == ptRegion) {
        tRegion.tSize = ptTarget->tRegion.tSize;
        ptRegion = &tRegion;
    }
    
    arm_2d_region_t tRotationRegion = *ptRegion;
    arm_2d_location_t tTargetCentre = {
        .iX = ptRegion->tLocation.iX + (ptRegion->tSize.iWidth >> 1),
        .iY = ptRegion->tLocation.iY + (ptRegion->tSize.iHeight >> 1),
    };
    
    arm_2d_location_t tCentre = {
        .iX = c_tileQuaterArc.tRegion.tSize.iWidth - 1,
        .iY = c_tileQuaterArc.tRegion.tSize.iHeight - 1,
    };

    if (bIsNewFrame) {
        s_fScale = (float)(     (float)iDiameter 
                            /   ((float)c_tileQuaterArc.tRegion.tSize.iWidth *2.0f));

        s_fAngle = ARM_2D_ANGLE((float)iProgress * 36.0f / 100.0f);
    }

    tRotationRegion.tSize.iWidth = ((ptRegion->tSize.iWidth + 1) >> 1);
    tRotationRegion.tSize.iHeight = ((ptRegion->tSize.iHeight + 1) >> 1);
    
    if(s_fAngle < ARM_2D_ANGLE(90)){
        arm_2d_region_t tQuater = tRotationRegion;
        tQuater.tLocation.iX += ((ptRegion->tSize.iWidth + 1) >> 1);
        tQuater.tLocation.iY += ((ptRegion->tSize.iWidth + 1) >> 1);
        
        arm_2dp_tile_transform_with_src_mask_and_opacity(
            &s_tOP[1],
            &c_tileQuaterArc,
            &c_tileQuaterArcMask,
            ptTarget,
            &tQuater,
            tCentre,
            ARM_2D_ANGLE(180),
            s_fScale,
            chOpacity,
            &tTargetCentre);
        
        arm_2d_op_wait_async((arm_2d_op_core_t *)&s_tOP[1]);
    }

    if(s_fAngle < ARM_2D_ANGLE(180)){
        arm_2d_region_t tQuater = tRotationRegion;
        tQuater.tLocation.iY += ((ptRegion->tSize.iWidth + 1) >> 1);
    
        arm_2dp_tile_transform_with_src_mask_and_opacity(
            &s_tOP[2],
            &c_tileQuaterArc,
            &c_tileQuaterArcMask,
            ptTarget,
            &tQuater,
            tCentre,
            ARM_2D_ANGLE(270),
            s_fScale,
            chOpacity,
            &tTargetCentre);
            
        arm_2d_op_wait_async((arm_2d_op_core_t *)&s_tOP[2]);
    } 

    if(s_fAngle < ARM_2D_ANGLE(270)){
    
        arm_2dp_tile_transform_with_src_mask_and_opacity(
            &s_tOP[3],
            &c_tileQuaterArc,
            &c_tileQuaterArcMask,
            ptTarget,
            &tRotationRegion,
            tCentre,
            ARM_2D_ANGLE(0),
            s_fScale,
            chOpacity,
            &tTargetCentre);

        arm_2d_op_wait_async((arm_2d_op_core_t *)&s_tOP[3]);
    }

    if (s_fAngle < ARM_2D_ANGLE(90)) {
        tRotationRegion.tLocation.iX += ((ptRegion->tSize.iWidth + 1) >> 1);
    } else if (s_fAngle < ARM_2D_ANGLE(180)) {
        tRotationRegion.tLocation.iY += ((ptRegion->tSize.iHeight + 1) >> 1);
        tRotationRegion.tLocation.iX += ((ptRegion->tSize.iWidth + 1) >> 1);        
    } else if (s_fAngle < ARM_2D_ANGLE(270)) {
        tRotationRegion.tLocation.iY += ((ptRegion->tSize.iHeight + 1) >> 1);
    }
    
    arm_2dp_tile_transform_with_src_mask_and_opacity(
                &s_tOP[0], 
                &c_tileQuaterArc,
                &c_tileQuaterArcMask,
                ptTarget,
                &tRotationRegion,
                tCentre,
                s_fAngle + ARM_2D_ANGLE(90),
                s_fScale,
                chOpacity,
                &tTargetCentre);
        
    arm_2d_op_wait_async((arm_2d_op_core_t *)&s_tOP[0]);

}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
