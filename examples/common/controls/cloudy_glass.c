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
#define __CLOUDY_GLASS_IMPLEMENT__

#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "cloudy_glass.h"
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
extern const arm_2d_tile_t c_tileRadialGradientMask;

/*============================ PROTOTYPES ====================================*/
static
void __draw_bubble_handler( void *pObj,
                            dynamic_nebula_t *ptDN,
                            const arm_2d_tile_t *ptTile,
                            arm_2d_location_t tLocation,
                            uint8_t chOpacity,
                            int16_t iDistance);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void cloudy_glass_init( cloudy_glass_t *ptThis,
                          cloudy_glass_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(cloudy_glass_t));
    this.tCFG = *ptCFG;

    arm_2d_size_t tGlassSize = this.tCFG.tSize;
    assert(tGlassSize.iHeight > 0);
    assert(tGlassSize.iWidth > 0);

    assert(this.tCFG.hwParticleCount > 0);
    assert(this.tCFG.ptParticles != NULL);

    do {
        int16_t iRadius = MIN(tGlassSize.iHeight, tGlassSize.iWidth) / 2;
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 0.6f,
            .iRadius = iRadius,
            .iVisibleRingWidth = iRadius,
            .hwParticleCount = this.tCFG.hwParticleCount,
            .ptParticles = this.tCFG.ptParticles,

            .evtOnDrawParticles = {
                .fnHandler = &__draw_bubble_handler,
                .pTarget = ptThis,
            },
        };
        dynamic_nebula_init(&this.tNebula, &tCFG);
    } while(0);

    ARM_2D_OP_INIT(this.tBlurOP);

}


static
void __draw_bubble_handler( void *pObj,
                            dynamic_nebula_t *ptDN,
                            const arm_2d_tile_t *ptTile,
                            arm_2d_location_t tLocation,
                            uint8_t chOpacity,
                            int16_t iDistance)
{
    cloudy_glass_t *ptThis = (cloudy_glass_t *)pObj;

    ARM_2D_UNUSED(ptDN);

    arm_2d_region_t tBubbleRegion = c_tileRadialGradientMask.tRegion;

    tBubbleRegion.tLocation.iX = tLocation.iX - c_tileRadialGradientMask.tRegion.tSize.iWidth / 2;
    tBubbleRegion.tLocation.iY = tLocation.iY - c_tileRadialGradientMask.tRegion.tSize.iHeight / 2;

    uint8_t chOriginalOpacity = 256 - chOpacity;

    if (iDistance > 16) {
        chOpacity = chOriginalOpacity;
    } else {
        chOpacity = MIN((iDistance << 4), chOriginalOpacity);
    }

    arm_2d_fill_colour_with_mask_and_opacity(ptTile, 
                                    &tBubbleRegion, 
                                    &c_tileRadialGradientMask, 
                                    (__arm_2d_color_t){this.tCFG.tColour},
                                    chOpacity);
}

ARM_NONNULL(1)
void cloudy_glass_depose( cloudy_glass_t *ptThis)
{
    assert(NULL != ptThis);
    
    dynamic_nebula_depose(&this.tNebula);

    ARM_2D_OP_DEPOSE(this.tBlurOP);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_remove_items(&this.tCFG.ptScene->tDirtyRegionHelper,
                                            &this.tDirtyRegionItem,
                                            1);
    }
}

ARM_NONNULL(1)
void cloudy_glass_on_load( cloudy_glass_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_add_items(&this.tCFG.ptScene->tDirtyRegionHelper,
                                             &this.tDirtyRegionItem,
                                             1);
    }
}

ARM_NONNULL(1)
void cloudy_glass_on_frame_start( cloudy_glass_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void cloudy_glass_set_colour( cloudy_glass_t *ptThis, 
                                COLOUR_INT_TYPE tColour)
{
    assert(NULL != ptThis);
    this.tCFG.tColour = tColour;
}

ARM_NONNULL(1)
void cloudy_glass_on_frame_complete( cloudy_glass_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2dp_filter_iir_blur_depose(&this.tBlurOP);
    
}

ARM_NONNULL(1)
void cloudy_glass_show(   cloudy_glass_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }
    
    assert(NULL!= ptThis);
    assert(NULL!= ptTile);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTile);

    arm_2d_container(ptTile, __glass, ptRegion) {
        /* put your drawing code inside here
         *    - &__control is the target tile (please do not use ptTile anymore)
         *    - __control_canvas is the canvas
         */

        /* example code: flash a 50x50 red box in the centre */
        arm_2d_align_centre(__glass_canvas, this.tCFG.tSize) {

            /* show nebula */
            dynamic_nebula_show(&this.tNebula, 
                                &__glass, 
                                &__centre_region, 
                                GLCD_COLOR_WHITE, 
                                255,
                                bIsNewFrame);

            arm_2dp_filter_iir_blur(&this.tBlurOP,
                                    &__glass,
                                    &__centre_region,
                                    255 - 16);

            if (NULL != this.tCFG.ptScene) {
                /* update dirty region */
                arm_2d_helper_dirty_region_update_item( &this.tCFG.ptScene->tDirtyRegionHelper,
                                                        &this.tDirtyRegionItem,
                                                        &__glass,
                                                        &__glass_canvas,
                                                        &__centre_region);
            }

            /* make sure the operation is complete */
            ARM_2D_OP_WAIT_ASYNC(&this.tBlurOP);
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
