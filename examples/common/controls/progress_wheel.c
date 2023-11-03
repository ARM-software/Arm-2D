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
#define __PROGRESS_WHEEL_IMPLEMENT__

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



#elif __GLCD_CFG_COLOUR_DEPTH__ == 16



#elif __GLCD_CFG_COLOUR_DEPTH__ == 32



#else
#   error Unsupported colour depth!
#endif

#undef this
#define this  (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/


extern const arm_2d_tile_t c_tileQuaterArcMask;
extern const arm_2d_tile_t c_tileWhiteDotMask;
/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/


/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void __progress_wheel_init( progress_wheel_t *ptThis, 
                            const progress_wheel_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(progress_wheel_t));
    this.tCFG = *ptCFG;
    
    if (NULL == this.tCFG.ptileArcMask) {
        this.tCFG.ptileArcMask = &c_tileQuaterArcMask;
    }
    if (NULL == this.tCFG.ptileDotMask) {
        this.tCFG.ptileDotMask = &c_tileWhiteDotMask;
        this.tCFG.tDotColour = GLCD_COLOR_WHITE;
    }

    if (0 == this.tCFG.iWheelDiameter) {
        this.tCFG.iWheelDiameter = this.tCFG.ptileArcMask->tRegion.tSize.iWidth;
    }

    arm_foreach(arm_2d_op_fill_cl_msk_opa_trans_t, this.tOP, ptItem) {
        ARM_2D_OP_INIT(*ptItem);
    }

    progress_wheel_set_diameter(ptThis, ptCFG->iWheelDiameter);

    arm_2d_region_list_item_t **ppDirtyRegionList = this.tCFG.ppList;
    if (NULL != ppDirtyRegionList) {
        /* initialize transform helper */
        arm_2d_helper_transform_init(&this.tTransHelper,
                                    (arm_2d_op_t *)&this.tOP[5],
                                    0.01f,
                                    0.1f,
                                    ppDirtyRegionList);

        if (NULL != ppDirtyRegionList) {
            while(NULL != (*ppDirtyRegionList)) {
                ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
            }

            /* add dirty region items to the list */
            (*ppDirtyRegionList) = &this.tDirtyRegion;
            this.tDirtyRegion.ptNext = NULL;

            this.tDirtyRegion.bIgnore = true;
        }
    }
}


ARM_NONNULL(1)
void progress_wheel_set_diameter(progress_wheel_t *ptThis, 
                                int16_t iDiameter)
{
    assert(NULL != ptThis);

    this.fScale = (float)(  (float)iDiameter 
                         /  ((float)this.tCFG.ptileArcMask->tRegion.tSize.iWidth *2.0f));
    this.tCFG.iWheelDiameter = iDiameter;
}

ARM_NONNULL(1)
void progress_wheel_depose(progress_wheel_t *ptThis)
{
    arm_foreach(arm_2d_op_fill_cl_msk_opa_trans_t, this.tOP, ptItem) {
        ARM_2D_OP_DEPOSE(*ptItem);
    }

    arm_2d_helper_transform_depose(&this.tTransHelper);

    arm_2d_region_list_item_t **ppDirtyRegionList = this.tCFG.ppList;

    if (NULL == ppDirtyRegionList) {
        return ;
    }

    while(NULL != (*ppDirtyRegionList)) {

        /* remove the dirty region from the user dirty region list */
        if ((*ppDirtyRegionList) == &this.tDirtyRegion) {
            (*ppDirtyRegionList) = this.tDirtyRegion.ptNext;
            this.tDirtyRegion.ptNext = NULL;
            break;
        }

        ppDirtyRegionList = &((*ppDirtyRegionList)->ptNext);
    }
}

ARM_NONNULL(1,2)
void progress_wheel_show(   progress_wheel_t *ptThis,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            int16_t iProgress,
                            uint8_t chOpacity,
                            bool bIsNewFrame)
{

    enum {
        START,
        WAIT_CHANGE,
        DRAW_LAST_QUADRANT,
        DRAW_WHOLE_WHEEL,
        DRAW_CURVE,
        DRAW_START_POINT,
        DRAW_END_POINT,
        FINISH,
    };

    const arm_2d_tile_t *ptileArcMask = this.tCFG.ptileArcMask;
    COLOUR_INT tWheelColour = this.tCFG.tWheelColour;

    iProgress = MIN(1000, iProgress);

    int8_t chCurrentQuadrant = (int8_t)(iProgress / 250);

    if (    (this.chState == START) 
        &&  bIsNewFrame 
        && (NULL != this.tCFG.ppList)) {
        /* initialize fsm */
        this.chState = WAIT_CHANGE;
        this.chLastQuadrant = 0;
        this.chState = DRAW_WHOLE_WHEEL;
    }

    arm_2d_container(ptTarget, __wheel, ptRegion) {


        if ( WAIT_CHANGE == this.chState) {
            if (this.iLastProgress != iProgress) {
                this.iLastProgress = iProgress;

                

                int8_t chQuadrantChange = chCurrentQuadrant - this.chLastQuadrant;
                chQuadrantChange = ABS(chQuadrantChange);

                if (1000 == iProgress && 3 == this.chLastQuadrant) {
                    this.chState = DRAW_CURVE;
                } else if (chQuadrantChange < 1) {
                    /* content changed */
                    this.chState = DRAW_CURVE;
                } else if (chQuadrantChange == 1) {
                    this.chState = DRAW_LAST_QUADRANT;
                } else {
                    this.chLastQuadrant = chCurrentQuadrant;
                    this.chState = DRAW_WHOLE_WHEEL;
                }
            }
        }

        if (DRAW_WHOLE_WHEEL == this.chState) {
            this.tDirtyRegion.tRegion = __wheel_canvas;
            this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, __wheel_canvas.tLocation);
            this.tDirtyRegion.bIgnore = false;

            this.tDirtyRegion.bUpdated = true;      /* request update */
            this.chState = FINISH;
        }
    
        arm_2d_region_t tRotationRegion = __wheel_canvas;
        arm_2d_location_t tTargetCentre = {
            .iX = __wheel_canvas.tLocation.iX + (__wheel_canvas.tSize.iWidth >> 1),
            .iY = __wheel_canvas.tLocation.iY + (__wheel_canvas.tSize.iHeight >> 1),
        };
        
        arm_2d_location_t tCentre = {
            .iX = ptileArcMask->tRegion.tSize.iWidth - 1,
            .iY = ptileArcMask->tRegion.tSize.iHeight - 1,
        };

        if (bIsNewFrame) {
            this.fAngle = ARM_2D_ANGLE((float)iProgress * 36.0f / 100.0f);

            if (NULL != this.tCFG.ppList) {
                /* update helper with new values*/
                arm_2d_helper_transform_update_value(&this.tTransHelper, this.fAngle, 1.0f);

                /* call helper's on-frame-begin event handler */
                arm_2d_helper_transform_on_frame_begin(&this.tTransHelper);
            }
        }

        tRotationRegion.tSize.iWidth = ((__wheel_canvas.tSize.iWidth + 1) >> 1);
        tRotationRegion.tSize.iHeight = ((__wheel_canvas.tSize.iHeight + 1) >> 1);

        if(this.fAngle > ARM_2D_ANGLE(90.0f)){
            arm_2d_region_t tQuater = tRotationRegion;
            tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);

            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[3],
                                            ptileArcMask,
                                            &__wheel,
                                            &tQuater,
                                            tCentre,
                                            ARM_2D_ANGLE(90.0f),
                                            this.fScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);

            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[3]);

            /* update dirty region */
            if (DRAW_LAST_QUADRANT == this.chState && this.chLastQuadrant == 0) {
                /* wait idle */
                if (false == this.tDirtyRegion.bUpdated) {
                    this.tDirtyRegion.tRegion = tQuater;
                    this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, tQuater.tLocation);
                    this.tDirtyRegion.bIgnore = false;
                    this.tDirtyRegion.bUpdated = true;      /* request update */

                    this.chLastQuadrant = chCurrentQuadrant;
                    this.chState = DRAW_CURVE;
                }
            }
        }

        if(this.fAngle > ARM_2D_ANGLE(180.0f)){
            arm_2d_region_t tQuater = tRotationRegion;
            tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
            tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);


            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[1],
                                            ptileArcMask,
                                            &__wheel,
                                            &tQuater,
                                            tCentre,
                                            ARM_2D_ANGLE(180.0f),
                                            this.fScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);
            
            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[1]);

            /* update dirty region */
            if (DRAW_LAST_QUADRANT == this.chState && this.chLastQuadrant == 1) {
                /* wait idle */
                if (false == this.tDirtyRegion.bUpdated) {
                    this.tDirtyRegion.tRegion = tQuater;
                    this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, tQuater.tLocation);
                    this.tDirtyRegion.bIgnore = false;
                    this.tDirtyRegion.bUpdated = true;      /* request update */

                    this.chLastQuadrant = chCurrentQuadrant;
                    this.chState = DRAW_CURVE;
                }
            }

        }

        if(this.fAngle > ARM_2D_ANGLE(270.0)){
            arm_2d_region_t tQuater = tRotationRegion;
            tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);

            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[2],
                                            ptileArcMask,
                                            &__wheel,
                                            &tQuater,
                                            tCentre,
                                            ARM_2D_ANGLE(270.0f),
                                            this.fScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);
                
            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[2]);

            /* update dirty region */
            if (DRAW_LAST_QUADRANT == this.chState && this.chLastQuadrant == 2) {
                /* wait idle */
                if (false == this.tDirtyRegion.bUpdated) {
                    this.tDirtyRegion.tRegion = tQuater;
                    this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, tQuater.tLocation);
                    this.tDirtyRegion.bIgnore = false;
                    this.tDirtyRegion.bUpdated = true;      /* request update */

                    this.chLastQuadrant = chCurrentQuadrant;
                    this.chState = DRAW_CURVE;
                }
            }

        } 

        do {
            arm_2d_region_t tQuater = tRotationRegion;
            if (this.fAngle < ARM_2D_ANGLE(90)) {
                tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
            } else if (this.fAngle < ARM_2D_ANGLE(180)) {
                tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
            } else if (this.fAngle < ARM_2D_ANGLE(270)) {
                tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
            }

            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[0],
                                            ptileArcMask,
                                            &__wheel,
                                            &tQuater,
                                            tCentre,
                                            this.fAngle,
                                            this.fScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);
                
            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[0]);

            /* update dirty region */
            if (DRAW_CURVE == this.chState) {
                /* wait idle */
                if (false == this.tDirtyRegion.bUpdated) {
                    this.tDirtyRegion.tRegion = tQuater;
                    this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, tQuater.tLocation);
                    this.tDirtyRegion.bIgnore = false;

                    this.tDirtyRegion.bUpdated = true;      /* request update */
                    this.chState = DRAW_START_POINT;
                }
            }
        } while(0);

        /* draw the starting point */
        const arm_2d_tile_t *ptileDotMask = this.tCFG.ptileDotMask;
        if (NULL != ptileDotMask) {
            arm_2d_region_t tQuater = tRotationRegion;
            arm_2d_location_t tDotCentre = {
                .iX = (ptileDotMask->tRegion.tSize.iWidth + 1) >> 1,
                .iY = ptileArcMask->tRegion.tSize.iHeight - 1,
            };

            /* draw the starting point */
            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[4],
                                            ptileDotMask,
                                            &__wheel,
                                            &tQuater,
                                            tDotCentre,
                                            0.0f,
                                            this.fScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);
                
            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[4]);

            if (DRAW_START_POINT == this.chState) {
                if (false == this.tDirtyRegion.bUpdated) {
                    this.tDirtyRegion.tRegion = tQuater;
                    this.tDirtyRegion.tRegion.tLocation = arm_2d_helper_pfb_get_absolute_location(&__wheel, tQuater.tLocation);
                    this.tDirtyRegion.bIgnore = false;

                    this.tDirtyRegion.bUpdated = true;      /* request update */
                    this.chState = FINISH;
                }
            }

            /* draw the end point */
            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[5],
                                            ptileDotMask,
                                            &__wheel,
                                            &__wheel_canvas,
                                            tDotCentre,
                                            this.fAngle,
                                            this.fScale,
                                            tWheelColour,
                                            255,//chOpacity,
                                            &tTargetCentre);

            if (NULL != this.tCFG.ppList) {
                arm_2d_helper_transform_update_dirty_regions(   &this.tTransHelper,
                                                                &__wheel_canvas,
                                                                bIsNewFrame);
            }

            arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[5]);

            if (this.tCFG.tDotColour != this.tCFG.tWheelColour) {

                tDotCentre.iY = (int16_t)((float)tDotCentre.iY * 1.235f);

                /* draw the white dot */
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                &this.tOP[6],
                                                ptileDotMask,
                                                &__wheel,
                                                &__wheel_canvas,
                                                tDotCentre,
                                                this.fAngle,
                                                this.fScale * 0.80f,
                                                this.tCFG.tDotColour,
                                                255,
                                                &tTargetCentre);

                arm_2d_op_wait_async((arm_2d_op_core_t *)&this.tOP[6]);
            }
        }
    }

    if (FINISH == this.chState) {
        if (false == this.tDirtyRegion.bUpdated) {
            this.tDirtyRegion.bIgnore = true;
            this.chState = WAIT_CHANGE;
        }
    }
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
