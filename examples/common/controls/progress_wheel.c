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
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
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
enum {
    START,
    WAIT_CHANGE,
    //DRAW_LAST_QUADRANT,
    //DRAW_ERASE_LAST_CURVE,
    DRAW_MIDDLE_STEP,
    DRAW_WHOLE_WHEEL,
    //DRAW_CURVE,
    //DRAW_END_POINT,
    //FINISH,
    NO_DIRTY_REGIONS,
};
/*============================ GLOBAL VARIABLES ==============================*/


extern const arm_2d_tile_t c_tileQuaterArcMask;
extern const arm_2d_tile_t c_tileWhiteDotMask;
/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/


/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,3)
void progress_wheel_init( progress_wheel_t *ptThis,
                          arm_2d_scene_t *ptTargetScene,
                          const progress_wheel_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(progress_wheel_t));
    this.tCFG = *ptCFG;
    
    this.ptTargetScene = ptTargetScene;

    if (NULL == this.tCFG.ptileArcMask) {
        this.tCFG.ptileArcMask = &c_tileQuaterArcMask;
    }

    progress_wheel_set_diameter(ptThis, this.tCFG.iWheelDiameter);

    if (this.tCFG.bIgnoreDot) {
        if (0 == this.tCFG.iRingWidth) {
            this.tCFG.iRingWidth = this.tCFG.iWheelDiameter / 2;
        }

        const arm_2d_tile_t *ptArcMask = this.tCFG.ptileArcMask;
        assert(NULL != ptArcMask);

        this.tileInvisibleDot = (arm_2d_tile_t)
            impl_child_tile(
                *ptArcMask,
                ptArcMask->tRegion.tSize.iWidth - 1,    /* x offset */
                0,                                      /* y offset */
                1,
                this.tCFG.iRingWidth,                   /* there is no typo here!!! */
            );

            this.tCFG.ptileDotMask = &this.tileInvisibleDot;
            this.tCFG.tDotColour = this.tCFG.tWheelColour;
    } else {
        if (NULL == this.tCFG.ptileDotMask) {
            this.tCFG.ptileDotMask = &c_tileWhiteDotMask;
            this.tCFG.tDotColour = GLCD_COLOR_WHITE;
        } 

        this.tCFG.iRingWidth = this.tCFG.ptileDotMask->tRegion.tSize.iHeight;
    }

    arm_foreach(arm_2d_op_fill_cl_msk_opa_trans_t, this.tOP, ptItem) {
        ARM_2D_OP_INIT(*ptItem);
    }

    if (0 == this.tCFG.u15FullLength) {
        this.tCFG.u15FullLength = 1000;
    }

    this.q16LengthRatio = div_n_q16(reinterpret_q16_s16(3600), this.tCFG.u15FullLength);

    if (NULL == this.ptTargetScene) {
        this.tCFG.bUseDirtyRegions = false;
    }

    if (this.tCFG.bUseDirtyRegions) {
        arm_2d_dirty_region_item_ignore_set(&this.tDirtyRegion, true);

        arm_2d_scene_player_dynamic_dirty_region_init(  &this.tDirtyRegion,
                                                        ptTargetScene);
    }
}


ARM_NONNULL(1)
void progress_wheel_set_diameter(progress_wheel_t *ptThis, 
                                int16_t iDiameter)
{
    assert(NULL != ptThis);

    if (0 == iDiameter) {
        this.fScale = 1.0;
        this.tCFG.iWheelDiameter = this.tCFG.ptileArcMask->tRegion.tSize.iWidth * 2;
    } else {
        this.fScale = (float)(  (float)iDiameter 
                            /  ((float)this.tCFG.ptileArcMask->tRegion.tSize.iWidth *2.0f));
        this.tCFG.iWheelDiameter = iDiameter;
    }
}

ARM_NONNULL(1)
void progress_wheel_depose(progress_wheel_t *ptThis)
{
    assert(NULL != ptThis);
    arm_foreach(arm_2d_op_fill_cl_msk_opa_trans_t, this.tOP, ptItem) {
        ARM_2D_OP_DEPOSE(*ptItem);
    }

    if (this.tCFG.bUseDirtyRegions && NULL != this.ptTargetScene) {

        arm_2d_scene_player_dynamic_dirty_region_depose(&this.tDirtyRegion,
                                                this.ptTargetScene);

        arm_2d_helper_dirty_region_transform_depose(&this.tTransHelper);
    }
}

ARM_NONNULL(1)
void progress_wheel_on_load(progress_wheel_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.tCFG.bUseDirtyRegions) {
        /* initialize transform helper */
        arm_2d_helper_dirty_region_transform_init(  
                &this.tTransHelper,
                &this.ptTargetScene->tDirtyRegionHelper,
                (arm_2d_op_t *)&this.tOP[5],
                0.01f,
                0.1f);
    }
}

ARM_NONNULL(1)
void progress_wheel_on_frame_start(progress_wheel_t *ptThis)
{
    assert(NULL != ptThis);

    this.iProgress = this.iNewProgress;
    float fAngle = (float)this.iProgress / 10.0f + 90.0f * (float)this.tCFG.u2StartPosition;
    fAngle = fmodf(fAngle, 360.0f);
    this.fAngle = ARM_2D_ANGLE(fAngle);

    if (this.tCFG.bUseDirtyRegions) {

        arm_2d_dynamic_dirty_region_on_frame_start(&this.tDirtyRegion, 0xFF);

        /* update helper with new values*/
        arm_2d_helper_dirty_region_transform_update_value(&this.tTransHelper, this.fAngle, 1.0f);

        /* call helper's on-frame-begin event handler */
        arm_2d_helper_dirty_region_transform_on_frame_start(&this.tTransHelper);
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

    bool bNoScale = ABS(this.fScale - 1.0f) < 0.01f;
    bool bIgnoreCurve = false;

    const arm_2d_tile_t *ptileArcMask = this.tCFG.ptileArcMask;
    COLOUR_INT tWheelColour = this.tCFG.tWheelColour;

    if (bIsNewFrame) {
        iProgress = MAX(0, iProgress);
        iProgress = MIN(iProgress, this.tCFG.u15FullLength);

        this.iNewProgress = reinterpret_s16_q16(mul_n_q16(this.q16LengthRatio, iProgress));
    }

    int8_t chCurrentQuadrant = (int8_t)(this.iProgress / 900);

    uint8_t chState = NO_DIRTY_REGIONS;
    
    if (this.tCFG.bUseDirtyRegions) {
        chState = arm_2d_dynamic_dirty_region_wait_next(&this.tDirtyRegion);
    }

    arm_2d_region_t tDrawRegion;

    if (bNoScale) {
        arm_2d_canvas(ptTarget, __target_canvas) {
            if (NULL == ptRegion) {
                ptRegion = &__target_canvas;
            }

            arm_2d_size_t tWheelSize = ptileArcMask->tRegion.tSize;
            tWheelSize.iWidth *= 2;
            tWheelSize.iHeight *= 2;

            arm_2d_align_centre(*ptRegion, tWheelSize) {
                tDrawRegion = __centre_region;
                ptRegion = &tDrawRegion;
            }
        }
    }

    arm_2d_container(ptTarget, __wheel, ptRegion) {

        arm_2d_location_t tTargetCentre = {
            .iX = __wheel_canvas.tLocation.iX + (__wheel_canvas.tSize.iWidth >> 1),
            .iY = __wheel_canvas.tLocation.iY + (__wheel_canvas.tSize.iHeight >> 1),
        };

        if (    (chState == START) 
            &&  bIsNewFrame 
            &&  this.tCFG.bUseDirtyRegions) {
            /* initialize fsm */
            this.chLastQuadrant = 0;
            chState = DRAW_WHOLE_WHEEL;
        }

        if ( WAIT_CHANGE == chState) {
            if (this.iLastProgress != this.iProgress && bIsNewFrame) {
                
                int8_t chQuadrantChange = chCurrentQuadrant - this.chLastQuadrant;
                int8_t chDelta = ABS(chQuadrantChange);

                ARM_2D_LOG_INFO(
                    CONTROLS, 
                    0, 
                    "Progress Wheel", 
                    "iProgress Changed, from [%d] to [%d] Quadrant: Last[%d] Current[%d]",
                    this.iLastProgress,
                    this.iProgress,
                    this.chLastQuadrant,
                    chCurrentQuadrant
                );
                int16_t iProgressDelta = this.iProgress - this.iLastProgress;
                this.iLastProgress = this.iProgress;
                
                bool bSmallMove = ABS(iProgressDelta) <= 90;

                if (chDelta < 2) {

                    if (bSmallMove) {
                        ARM_2D_LOG_INFO(
                            CONTROLS, 
                            0, 
                            "Progress Wheel", 
                            "Progress in small steps, updating the regions of end point is sufficient.",
                            chCurrentQuadrant
                        );

                        /* remove extra area */
                        arm_2d_helper_dirty_region_item_set_extra_region(&this.tTransHelper.tItem,
                                                                         &__wheel,
                                                                         &__wheel_canvas,
                                                                         NULL);

                    } else {
                        ARM_2D_LOG_INFO(
                            CONTROLS, 
                            0, 
                            "Progress Wheel", 
                            "Progress in middle steps, use the minimal enclosure of the regions of the end point.",
                            chCurrentQuadrant
                        );

                        enum {
                            PROGRESS_INCREASE = 0,
                            PROGRESS_DECREASE,
                        };

                        static const struct {
                            int16_t s1Q0X       :2;
                            int16_t s1Q0Y       :2;
                            int16_t s1Q1X       :2;
                            int16_t s1Q1Y       :2;
                            int16_t s1Q2X       :2;
                            int16_t s1Q2Y       :2;
                            int16_t s1Q3X       :2;
                            int16_t s1Q3Y       :2;
                        } c_tQuadrantTable[2][4] = {
                            [PROGRESS_INCREASE] = {
                                [PROGRESS_WHEEL_START_POSITION_TOP] = {
                                    .s1Q0X = 1,
                                    .s1Q0Y = 0,

                                    .s1Q1X = 0,
                                    .s1Q1Y = 1,

                                    .s1Q2X = -1,
                                    .s1Q2Y = 0,
                                },

                                [PROGRESS_WHEEL_START_POSITION_RIGHT] = {
                                    .s1Q0X = 0,
                                    .s1Q0Y = 1,

                                    .s1Q1X = -1,
                                    .s1Q1Y = 0,

                                    .s1Q2X = 0,
                                    .s1Q2Y = -1,
                                },

                                [PROGRESS_WHEEL_START_POSITION_BOTTOM] = {

                                    .s1Q0X = -1,
                                    .s1Q0Y = 0,

                                    .s1Q1X = 0,
                                    .s1Q1Y = -1,

                                    .s1Q2X = 1,
                                    .s1Q2Y = 0,
                                },

                                [PROGRESS_WHEEL_START_POSITION_LEFT] = {

                                    .s1Q0X = 0,
                                    .s1Q0Y = -1,

                                    .s1Q1X = 1,
                                    .s1Q1Y = 0,

                                    .s1Q2X = 0,
                                    .s1Q2Y = 1,
                                },

                            },
                            [PROGRESS_DECREASE] = {
                                [PROGRESS_WHEEL_START_POSITION_TOP] = {
                                    .s1Q3X = -1,
                                    .s1Q3Y = 0,

                                    .s1Q2X = 0,
                                    .s1Q2Y = 1,

                                    .s1Q1X = 1,
                                    .s1Q1Y = 0,
                                },

                                [PROGRESS_WHEEL_START_POSITION_RIGHT] = {
                                    .s1Q3X = 0,
                                    .s1Q3Y = -1,

                                    .s1Q2X = -1,
                                    .s1Q2Y = 0,

                                    .s1Q1X = 0,
                                    .s1Q1Y = 1,
                                },

                                [PROGRESS_WHEEL_START_POSITION_BOTTOM] = {
                                    .s1Q3X = 1,
                                    .s1Q3Y = 0,

                                    .s1Q2X = 0,
                                    .s1Q2Y = -1,

                                    .s1Q1X = -1,
                                    .s1Q1Y = 0,
                                },

                                [PROGRESS_WHEEL_START_POSITION_LEFT] = {

                                    .s1Q3X = 0,
                                    .s1Q3Y = 1,

                                    .s1Q2X = 1,
                                    .s1Q2Y = 0,

                                    .s1Q1X = 0,
                                    .s1Q1Y = -1,
                                },

                            },
                        };

                        arm_2d_region_t tExtraRefreshArea = {0};
                        bool bExtraRefreshAreaInitialized = false;
                        int_fast8_t chStartPosition = this.tCFG.u2StartPosition;
                        int16_t iRadius = (this.tCFG.iWheelDiameter / 2) - 1;

                        if (iProgressDelta > 0) {
                            /* increase */
                            switch (this.chLastQuadrant) {
                                case 0:
                                    if (chCurrentQuadrant > 0) {
                                        arm_2d_region_t tRightMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q0X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q0Y,
                                            .tSize = {1,1},
                                        };

                                        bExtraRefreshAreaInitialized = true;
                                        tExtraRefreshArea = tRightMostPoint;
                                    }
                                    // fall_through
                                case 1:
                                    if (chCurrentQuadrant > 1) {
                                        arm_2d_region_t tBottomMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q1X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q1Y,
                                            .tSize = {1,1},
                                        };

                                        if (bExtraRefreshAreaInitialized) {
                                            bExtraRefreshAreaInitialized = true;
                                            tExtraRefreshArea = tBottomMostPoint;
                                        } else {
                                            /* update extra area */
                                            arm_2d_region_get_minimal_enclosure(&tBottomMostPoint,
                                                                                &tExtraRefreshArea,
                                                                                &tExtraRefreshArea);
                                        }
                                    }
                                    // fall_through
                                case 2:
                                    if (chCurrentQuadrant > 2) {
                                        arm_2d_region_t tLeftMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q2X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_INCREASE][chStartPosition].s1Q2Y,
                                            .tSize = {1,1},
                                        };

                                        if (bExtraRefreshAreaInitialized) {
                                            bExtraRefreshAreaInitialized = true;
                                            tExtraRefreshArea = tLeftMostPoint;
                                        } else {
                                            /* update extra area */
                                            arm_2d_region_get_minimal_enclosure(&tLeftMostPoint,
                                                                                &tExtraRefreshArea,
                                                                                &tExtraRefreshArea);
                                        }
                                    }
                                    break;
                                case 3:
                                default:
                                    /* this won't happen */
                                    break;
                            }

                            arm_2d_helper_dirty_region_item_set_extra_region(
                                                                            &this.tTransHelper.tItem,
                                                                            &__wheel,
                                                                            &__wheel_canvas,
                                                                            &tExtraRefreshArea);

                        } else {
                            /* decrease */
                            switch (this.chLastQuadrant) {
                                
                                case 3:
                                    if (chCurrentQuadrant < 3) {
                                        arm_2d_region_t tLeftMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q3X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q3Y,
                                            .tSize = {1,1},
                                        };

                                        bExtraRefreshAreaInitialized = true;
                                        tExtraRefreshArea = tLeftMostPoint;
                                    }
                                    // fall_through
                                case 2:
                                    if (chCurrentQuadrant < 2) {
                                        arm_2d_region_t tBottomMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q2X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q2Y,
                                            .tSize = {1,1},
                                        };

                                        if (bExtraRefreshAreaInitialized) {
                                            bExtraRefreshAreaInitialized = true;
                                            tExtraRefreshArea = tBottomMostPoint;
                                        } else {
                                            /* update extra area */
                                            arm_2d_region_get_minimal_enclosure(&tBottomMostPoint,
                                                                                &tExtraRefreshArea,
                                                                                &tExtraRefreshArea);
                                        }
                                    }
                                    // fall through
                                case 1:
                                    if (chCurrentQuadrant < 2) {
                                        arm_2d_region_t tRightMostPoint = {
                                            .tLocation.iX = tTargetCentre.iX 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q1X,
                                            .tLocation.iY = tTargetCentre.iY 
                                                          + iRadius 
                                                          * c_tQuadrantTable[PROGRESS_DECREASE][chStartPosition].s1Q1Y,
                                            .tSize = {1,1},
                                        };

                                        if (bExtraRefreshAreaInitialized) {
                                            bExtraRefreshAreaInitialized = true;
                                            tExtraRefreshArea = tRightMostPoint;
                                        } else {
                                            /* update extra area */
                                            arm_2d_region_get_minimal_enclosure(&tRightMostPoint,
                                                                                &tExtraRefreshArea,
                                                                                &tExtraRefreshArea);
                                        }
                                    }
                                    // fall through
                                    break;
                                default:
                                case 0:
                                    /* this won't happen */
                                    break;
                            }

                            arm_2d_helper_dirty_region_item_set_extra_region(
                                                                            &this.tTransHelper.tItem,
                                                                            &__wheel,
                                                                            &__wheel_canvas,
                                                                            &tExtraRefreshArea);

                        }

                        chState = DRAW_MIDDLE_STEP;
                    }

                    this.chLastQuadrant = chCurrentQuadrant;
                } else {

                    ARM_2D_LOG_INFO(
                        CONTROLS, 
                        0, 
                        "Progress Wheel", 
                        "Progress changes too much, we decide to redraw the whole progress wheel"
                    );

                    chState = DRAW_WHOLE_WHEEL;
                }
            }
        }

        if (DRAW_WHOLE_WHEEL == chState) {
            ARM_2D_LOG_INFO(
                CONTROLS, 
                0, 
                "Progress Wheel", 
                "Draw Whole Wheel, progress: last [%d] current [%d] Quadrant: Last[%d] Current[%d]",
                this.iLastProgress,
                this.iProgress,
                this.chLastQuadrant,
                chCurrentQuadrant
            );

            /* remove extra area */
            arm_2d_helper_dirty_region_item_set_extra_region(&this.tTransHelper.tItem,
                                                             &__wheel,
                                                             &__wheel_canvas,
                                                             NULL);

            arm_2d_dynamic_dirty_region_update( &this.tDirtyRegion,
                                                &__wheel,
                                                &__wheel_canvas,
                                                WAIT_CHANGE);
            this.chLastQuadrant = chCurrentQuadrant;
        }
    
    /*------------------------------------------------------------------------*
     * Quadrant related variables won't cross this line                       *
     *------------------------------------------------------------------------*/

        arm_2d_region_t tRotationRegion = __wheel_canvas;

        tRotationRegion.tSize.iWidth = ((__wheel_canvas.tSize.iWidth + 1) >> 1);
        tRotationRegion.tSize.iHeight = ((__wheel_canvas.tSize.iHeight + 1) >> 1);

        arm_2d_location_t tCentre = {
            .iX = ptileArcMask->tRegion.tSize.iWidth - 1 - !bNoScale,
            .iY = ptileArcMask->tRegion.tSize.iHeight - 1 - !bNoScale,
        };

        if (this.iProgress >= 900) {
            if (this.iProgress == 900) {
                bIgnoreCurve = true;
            }

            arm_2d_region_t tQuater = tRotationRegion;

            switch (this.tCFG.u2StartPosition) {
                default:
                case PROGRESS_WHEEL_START_POSITION_TOP:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_RIGHT:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_LEFT:
                    break;
            }
            
            
            if (bNoScale) {
                switch (this.tCFG.u2StartPosition) {
                    default:
                    case PROGRESS_WHEEL_START_POSITION_TOP:
                        arm_2d_fill_colour_with_mask_x_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_RIGHT:
                        arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                        arm_2d_fill_colour_with_mask_y_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_LEFT:
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                }
                
            } else {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                &this.tOP[3],
                                                ptileArcMask,
                                                &__wheel,
                                                &tQuater,
                                                tCentre,
                                                ARM_2D_ANGLE((1 + (int)this.tCFG.u2StartPosition) * 90.0f),
                                                this.fScale,
                                                tWheelColour,
                                                chOpacity,
                                                &tTargetCentre);

                ARM_2D_OP_WAIT_ASYNC(&this.tOP[3]);
            } while(0);

        }

        if (this.iProgress >= 1800) {

            if (this.iProgress == 1800) {
                bIgnoreCurve = true;
            }

            arm_2d_region_t tQuater = tRotationRegion;

            switch (this.tCFG.u2StartPosition) {
                default:
                case PROGRESS_WHEEL_START_POSITION_TOP:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_RIGHT:
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                    break;
                case PROGRESS_WHEEL_START_POSITION_LEFT:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    break;
            }

            if (bNoScale) {

                switch (this.tCFG.u2StartPosition) {
                    default:
                    case PROGRESS_WHEEL_START_POSITION_TOP:
                        arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_RIGHT:
                        arm_2d_fill_colour_with_mask_y_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_LEFT:
                        arm_2d_fill_colour_with_mask_x_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                }
            } else {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                &this.tOP[1],
                                                ptileArcMask,
                                                &__wheel,
                                                &tQuater,
                                                tCentre,
                                                ARM_2D_ANGLE((2 + (int)this.tCFG.u2StartPosition) * 90.0f),
                                                this.fScale,
                                                tWheelColour,
                                                chOpacity,
                                                &tTargetCentre);
                
                ARM_2D_OP_WAIT_ASYNC(&this.tOP[1]);
            }
        }

        if (this.iProgress >= 2700) {
            if (this.iProgress == 2700) {
                bIgnoreCurve = true;
            }

            arm_2d_region_t tQuater = tRotationRegion;

            switch (this.tCFG.u2StartPosition) {
                default:
                case PROGRESS_WHEEL_START_POSITION_TOP:
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_RIGHT:
                    break;
                case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_LEFT:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
            }

            if (bNoScale) {
            
                switch (this.tCFG.u2StartPosition) {
                    default:
                    case PROGRESS_WHEEL_START_POSITION_TOP:
                        arm_2d_fill_colour_with_mask_y_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_RIGHT:
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                        arm_2d_fill_colour_with_mask_x_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_LEFT:
                        arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                }

            } else {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                &this.tOP[2],
                                                ptileArcMask,
                                                &__wheel,
                                                &tQuater,
                                                tCentre,
                                                ARM_2D_ANGLE((3 + (int)this.tCFG.u2StartPosition) * 90.0f),
                                                this.fScale,
                                                tWheelColour,
                                                chOpacity,
                                                &tTargetCentre);
                    
                ARM_2D_OP_WAIT_ASYNC(&this.tOP[2]);
            }
        }


        if (this.iProgress >= 3600) {
            //if (this.iProgress == 3600) {
                bIgnoreCurve = true;
            //}

            arm_2d_region_t tQuater = tRotationRegion;

            switch (this.tCFG.u2StartPosition) {
                default:
                case PROGRESS_WHEEL_START_POSITION_TOP:
                    break;
                case PROGRESS_WHEEL_START_POSITION_RIGHT:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                    tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
                case PROGRESS_WHEEL_START_POSITION_LEFT:
                    tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                    break;
            }

            if (bNoScale) {
            
                switch (this.tCFG.u2StartPosition) {
                    default:
                    case PROGRESS_WHEEL_START_POSITION_TOP:
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_RIGHT:
                        arm_2d_fill_colour_with_mask_x_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_BOTTOM:
                        arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                    case PROGRESS_WHEEL_START_POSITION_LEFT:
                        arm_2d_fill_colour_with_mask_y_mirror_and_opacity(
                            &__wheel,
                            &tQuater,
                            ptileArcMask,
                            (__arm_2d_color_t){tWheelColour},
                            chOpacity);
                        break;
                }

            } else {
                arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                &this.tOP[2],
                                                ptileArcMask,
                                                &__wheel,
                                                &tQuater,
                                                tCentre,
                                                ARM_2D_ANGLE((4 + (int)this.tCFG.u2StartPosition) * 90.0f),
                                                this.fScale,
                                                tWheelColour,
                                                chOpacity,
                                                &tTargetCentre);
                    
                ARM_2D_OP_WAIT_ASYNC(&this.tOP[2]);
            }
        }


        do {
            if (bIgnoreCurve) {
                break;
            }
            arm_2d_region_t tQuater = tRotationRegion;

            if (this.fAngle < ARM_2D_ANGLE(90.0f)) {
                tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);

            } else if (this.fAngle < ARM_2D_ANGLE(180.0f)) {
                tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
                tQuater.tLocation.iX += ((__wheel_canvas.tSize.iWidth + 1) >> 1);

            } else if (this.fAngle < ARM_2D_ANGLE(270.0f)) {
                tQuater.tLocation.iY += ((__wheel_canvas.tSize.iHeight + 1) >> 1);
            } 

            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &this.tOP[0],
                                            ptileArcMask,
                                            &__wheel,
                                            &tQuater,
                                            tCentre,
                                            this.fAngle,
                                            this.fScale + 0.003f * bNoScale,
                                            tWheelColour,
                                            chOpacity,
                                            &tTargetCentre);

            ARM_2D_OP_WAIT_ASYNC(&this.tOP[0]);

        } while(0);


        const arm_2d_tile_t *ptileDotMask = this.tCFG.ptileDotMask;
        if (NULL != ptileDotMask) {
            arm_2d_region_t tQuater = tRotationRegion;

            arm_2d_location_t tDotCentre = {
                .iX = (ptileDotMask->tRegion.tSize.iWidth + 1) >> 1,
                .iY = ptileArcMask->tRegion.tSize.iHeight - 1,
            };
            if (!this.tCFG.bIgnoreDot) {
                /* draw the starting point */
                if (bNoScale) {
                    tQuater.tLocation.iX = tQuater.tLocation.iX 
                                        + tQuater.tSize.iWidth 
                                        - ptileDotMask->tRegion.tSize.iWidth / 2;
                    tQuater.tLocation.iY += 1;

                    arm_2d_tile_t tDotMaskHalf = 
                        impl_child_tile(*ptileDotMask, 
                                        0, 
                                        0, 
                                        ptileDotMask->tRegion.tSize.iWidth / 2, 
                                        ptileDotMask->tRegion.tSize.iHeight);

                    arm_2d_fill_colour_with_mask_and_opacity(
                        &__wheel,
                        &tQuater,
                        &tDotMaskHalf,
                        (__arm_2d_color_t){tWheelColour},
                        chOpacity);
                    
                    ARM_2D_OP_WAIT_ASYNC();

                } else {
                    arm_2d_location_t tPivot = tTargetCentre;
                    tPivot.iY += 1;

                    arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                                    &this.tOP[4],
                                                    ptileDotMask,
                                                    &__wheel,
                                                    &tQuater,
                                                    tDotCentre,
                                                    0.0f,
                                                    this.fScale - 0.002f,
                                                    tWheelColour,
                                                    chOpacity,
                                                    &tPivot);
                    
                    ARM_2D_OP_WAIT_ASYNC(&this.tOP[4]);
                }
            }

            uint8_t chDotOpacity;
            if (this.tCFG.bIgnoreDot) {
                chDotOpacity = 0;
            } else {
                chDotOpacity = 255;
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
                                            chDotOpacity,
                                            &tTargetCentre);

            if (this.tCFG.bUseDirtyRegions) {
                /* apply transform region patch */
                this.tTransHelper.tItem.tRegionPatch.tLocation.iX = -1;
                this.tTransHelper.tItem.tRegionPatch.tSize.iWidth = 1;

                if (bIsNewFrame) {
                    arm_2d_helper_dirty_region_transform_force_to_use_minimal_enclosure(
                                                    &this.tTransHelper,
                                                        (chState == DRAW_MIDDLE_STEP)
                                                    ||  this.tCFG.bIgnoreDot
                                                );
                    
                    arm_2d_helper_dirty_region_transform_suspend_update(
                                                    &this.tTransHelper,
                                                    (chState == DRAW_WHOLE_WHEEL)
                                                );
                }

                arm_2d_helper_dirty_region_transform_update(&this.tTransHelper,
                                                            &__wheel_canvas,
                                                            bIsNewFrame);
            }

            ARM_2D_OP_WAIT_ASYNC((arm_2d_op_core_t *)&this.tOP[5]);

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
                                                this.fScale * 0.788f,
                                                this.tCFG.tDotColour,
                                                255,
                                                &tTargetCentre);

                ARM_2D_OP_WAIT_ASYNC((arm_2d_op_core_t *)&this.tOP[6]);
            }
        }
    }
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
