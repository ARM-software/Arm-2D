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
#define __DYNAMIC_NEBULA_IMPLEMENT__

#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d_helper.h"
#include "dynamic_nebula.h"
#include <assert.h>
#include <stdlib.h>
#include <arm_math.h>

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

ARM_NONNULL(1)
void dynamic_nebula_init( dynamic_nebula_t *ptThis, dynamic_nebula_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->ptParticles);
    assert(ptCFG->hwParticleCount > 0);
    memset(ptThis, 0, sizeof(dynamic_nebula_t));

    this.tCFG = *ptCFG;
    this.tCFG.iVisibleRingWidth 
        = MIN(this.tCFG.iRadius, this.tCFG.iVisibleRingWidth);

    srand((unsigned int)arm_2d_helper_get_system_timestamp());
    
    int n = this.tCFG.hwParticleCount;
    dynamic_nebula_particle_t *ptParticle = this.tCFG.ptParticles;
    //int16_t iInvisibleRadius = this.tCFG.iRadius - this.tCFG.iVisibleRingWidth;

    this.fOpacityStep = 255.0f / (float)this.tCFG.iVisibleRingWidth;

    do {
        float fAngle = ARM_2D_ANGLE((float)(rand() % 3600) / 10.0f);
        
        ptParticle->fSin = arm_sin_f32(fAngle);
        ptParticle->fCos = arm_cos_f32(fAngle);
        ptParticle->fOffset = (float)(rand() % this.tCFG.iVisibleRingWidth);
        ptParticle++;
    } while(--n);

}

ARM_NONNULL(1)
void dynamic_nebula_depose( dynamic_nebula_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void dynamic_nebula_show(   dynamic_nebula_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chOpacity,
                            bool bIsNewFrame)
{
    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __control, ptRegion) {

        arm_2d_align_centre(__control_canvas, this.tCFG.iRadius * 2, this.tCFG.iRadius * 2) {

            arm_2d_location_t tCentre = {
                    .iX = __centre_region.tSize.iWidth >> 1,
                    .iY = __centre_region.tSize.iHeight >> 1,
                };

            int n = this.tCFG.hwParticleCount;
            dynamic_nebula_particle_t *ptParticle = this.tCFG.ptParticles;
            float fInvisibleRadius = (float)(this.tCFG.iRadius - this.tCFG.iVisibleRingWidth);

            do {
                float fRadius = ptParticle->fOffset + fInvisibleRadius;
                float fX = ptParticle->fCos * fRadius;
                float fY = ptParticle->fSin * fRadius;

                arm_2d_location_t tParicleLocation = {
                        .iX = tCentre.iX + (int16_t)fX,
                        .iY = tCentre.iY + (int16_t)fY,
                    };

                uint16_t hwOpacity = (uint16_t)((float)(ptParticle->fOffset * this.fOpacityStep) * chOpacity) >> 8;

                arm_2d_rgb565_draw_point(&__control, 
                                         tParicleLocation,
                                         tColour,
                                         (uint8_t)hwOpacity);
                
                arm_2d_op_wait_async(NULL);

                if (bIsNewFrame) {
                    ptParticle->fOffset -= this.tCFG.fSpeed;

                    if (ptParticle->fOffset <= 0.0f) {
                        ptParticle->fOffset = (float)this.tCFG.iVisibleRingWidth;

                        float fAngle = ARM_2D_ANGLE((float)(rand() % 3600) / 10.0f);
        
                        ptParticle->fSin = arm_sin_f32(fAngle);
                        ptParticle->fCos = arm_cos_f32(fAngle);
                    }
                }


                ptParticle++;
            } while(--n);

            /* make sure the operation is complete */
            arm_2d_op_wait_async(NULL);
        }
        
    }

    arm_2d_op_wait_async(NULL);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
