/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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

#include "./arm_2d_example_controls.h"
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

    if (0 == this.tCFG.hwParticleTypeSize) {
        this.tCFG.hwParticleTypeSize = sizeof(dynamic_nebula_particle_t);
    }

    this.fOpacityStep = 255.0f / (float)(this.tCFG.iVisibleRingWidth - this.tCFG.u8FadeOutEdgeWidth - this.tCFG.iFullyVisibleRingWidth);
    this.fFadeOutOpacityStep = 255.0f / (float)this.tCFG.u8FadeOutEdgeWidth;

    do {
        float fAngle = ARM_2D_ANGLE((float)(rand() % 3600) / 10.0f);
        
        ptParticle->fSin = arm_sin_f32(fAngle);
        ptParticle->fCos = arm_cos_f32(fAngle);
        ptParticle->fOffset = (float)(rand() % this.tCFG.iVisibleRingWidth);
        ptParticle = (dynamic_nebula_particle_t *)(((uintptr_t)ptParticle) + this.tCFG.hwParticleTypeSize);
    } while(--n);

}

ARM_NONNULL(1)
int16_t dynamic_nebula_get_radius(dynamic_nebula_t *ptThis)
{
    assert(NULL != ptThis);

    return this.tCFG.iRadius;
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

        arm_2d_region_t tValidRegionInRoot;
        arm_2d_location_t tOffset;
        arm_2d_tile_get_root( &__control, &tValidRegionInRoot, &tOffset);
        tValidRegionInRoot.tLocation = tOffset;

        arm_2d_align_centre(__control_canvas, this.tCFG.iRadius * 2, this.tCFG.iRadius * 2) {

            arm_2d_location_t tCentre = {
                    .iX = (__centre_region.tSize.iWidth >> 1) + __centre_region.tLocation.iX,
                    .iY = (__centre_region.tSize.iHeight >> 1) + __centre_region.tLocation.iY,
                };

            int n = this.tCFG.hwParticleCount;
            dynamic_nebula_particle_t *ptParticle = this.tCFG.ptParticles;
            float fInvisibleRadius = (float)(this.tCFG.iRadius - this.tCFG.iVisibleRingWidth);
            bool bIsBeenDrawing = arm_2d_helper_pfb_is_region_being_drawing(&__control, &__centre_region, NULL);

            do {
                this.ptCurrent = ptParticle;

                if (bIsNewFrame) {
                    if (this.tCFG.bMovingOutward) {
                        ptParticle->fOffset += this.tCFG.fSpeed;

                        if (ptParticle->fOffset >= (float)this.tCFG.iVisibleRingWidth) {
                            ptParticle->fOffset = 0.0f;

                            float fAngle = ARM_2D_ANGLE((float)(rand() % 3600) / 10.0f);
            
                            ptParticle->fSin = arm_sin_f32(fAngle);
                            ptParticle->fCos = arm_cos_f32(fAngle);
                        }
                    } else {
                        ptParticle->fOffset -= this.tCFG.fSpeed;

                        if (ptParticle->fOffset <= 0.0f) {
                            ptParticle->fOffset = (float)this.tCFG.iVisibleRingWidth;

                            float fAngle = ARM_2D_ANGLE((float)(rand() % 3600) / 10.0f);
            
                            ptParticle->fSin = arm_sin_f32(fAngle);
                            ptParticle->fCos = arm_cos_f32(fAngle);
                        }
                    }

                } else if (!bIsBeenDrawing) {
                    ptParticle = (dynamic_nebula_particle_t *)(((uintptr_t)ptParticle) + this.tCFG.hwParticleTypeSize);
                    continue;
                } 

                float fRadius = ptParticle->fOffset + fInvisibleRadius;
                float fX = ptParticle->fCos * fRadius;
                float fY = ptParticle->fSin * fRadius;

                arm_2d_location_t tParicleLocation = {
                    .iX = tCentre.iX + (int16_t)fX,
                    .iY = tCentre.iY + (int16_t)fY,
                };

                uint16_t hwOpacity = 0;
                float fFadeOutOffset =  ptParticle->fOffset - (this.tCFG.iVisibleRingWidth - this.tCFG.u8FadeOutEdgeWidth);
                
                if (fFadeOutOffset < 0) {
                    if (ptParticle->fOffset > (this.tCFG.iVisibleRingWidth - this.tCFG.u8FadeOutEdgeWidth - this.tCFG.iFullyVisibleRingWidth)) {
                        hwOpacity = 255;
                    } else {
                        hwOpacity = (uint16_t)((float)(ptParticle->fOffset * this.fOpacityStep) * chOpacity) >> 8;
                    } 
                } else {
                    hwOpacity = 255 - ((uint16_t)((float)(fFadeOutOffset * this.fFadeOutOpacityStep) * chOpacity) >> 8);
                }
                hwOpacity = MIN(hwOpacity, 255);

                if (NULL == this.tCFG.evtOnDrawParticles.fnHandler) {
                    if (arm_2d_is_point_inside_region(  &tValidRegionInRoot, 
                                                    &tParicleLocation)) {
                        arm_2d_draw_point(  &__control, 
                                                tParicleLocation,
                                                tColour,
                                                (uint8_t)hwOpacity);
                        
                        ARM_2D_OP_WAIT_ASYNC();
                    }
                    
                } else {
                    
                    ARM_2D_INVOKE_RT_VOID(this.tCFG.evtOnDrawParticles.fnHandler,
                                             ARM_2D_PARAM(
                                                this.tCFG.evtOnDrawParticles.pTarget,
                                                ptThis,
                                                &__control,
                                                tParicleLocation,
                                                (uint8_t)hwOpacity,
                                                (int16_t)fRadius
                                             ));
                    ARM_2D_OP_WAIT_ASYNC();
                }

                ptParticle = (dynamic_nebula_particle_t *)(((uintptr_t)ptParticle) + this.tCFG.hwParticleTypeSize);
            } while(--n);

            /* make sure the operation is complete */
            ARM_2D_OP_WAIT_ASYNC();
        }
        
    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
