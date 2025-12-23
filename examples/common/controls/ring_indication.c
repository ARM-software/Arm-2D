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
#define __RING_INDICATION_IMPLEMENT__

#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "ring_indication.h"
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
void ring_indication_init( ring_indication_t *ptThis,
                          ring_indication_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->ptTransformMode);
    assert(NULL != ptCFG->Foreground.ptTile);

    memset(ptThis, 0, sizeof(ring_indication_t));

    meter_pointer_cfg_t tCFG = {
        .tSpinZoom = {
            .Indicator = ptCFG->tIndication,
            .ptTransformMode = ptCFG->ptTransformMode,
            .Source = {
                .ptMask = ptCFG->QuarterSector.ptMask,
                .tCentreFloat = ptCFG->QuarterSector.tCentre,
            },
            .Extra = {
                .ptTile = ptCFG->Foreground.ptTile,
                .ptMask = ptCFG->Foreground.ptMask,
            },
            .Target = {
                .ptMask = ptCFG->Background.ptMask,
            },
            .ptScene = ptCFG->ptScene,
            .bUseFloatPointInCentre = true,
        },

        .tPISliderCFG = ptCFG->tPISliderCFG,

    };

    tCFG.tSpinZoom.Indicator.UpperLimit.fAngleInDegree 
        = fmodf(tCFG.tSpinZoom.Indicator.UpperLimit.fAngleInDegree, 360.0f);
    
    tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree 
        = fmodf(tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree, 360.0f);

    if (tCFG.tSpinZoom.Indicator.UpperLimit.fAngleInDegree <= tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree) {
        tCFG.tSpinZoom.Indicator.UpperLimit.fAngleInDegree += 360.0f;
    }

    meter_pointer_init(&this.tSector, &tCFG);
    spin_zoom_widget_set_angle_offset(  &this.tSector.use_as__spin_zoom_widget_t, 
                                        ptCFG->QuarterSector.fAngleOffset);

    this.Foreground.ptTile = ptCFG->Foreground.ptTile;
    this.Foreground.ptMask = ptCFG->Foreground.ptMask;
    this.Background.ptMask = ptCFG->Background.ptMask;

    int16_t iRadius = (this.Foreground.ptTile->tRegion.tSize.iWidth + 1) >> 1;
    int16_t iSectorMaskRadius = ptCFG->QuarterSector.ptMask->tRegion.tSize.iWidth - 2;

    this.iDiameter = this.Foreground.ptTile->tRegion.tSize.iWidth;

    this.fSectorScale = ((float)iRadius / (float)iSectorMaskRadius) + 0.05;

    if (tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree >= 270.0f) {
        this.u2StartFrom = RING_INDICATOR_START_FROM_Y_MINUS;
    } else if (tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree >= 180.0f) {
        this.u2StartFrom = RING_INDICATOR_START_FROM_X_MINUS;
    } else if (tCFG.tSpinZoom.Indicator.LowerLimit.fAngleInDegree >= 90.0f) {
        this.u2StartFrom = RING_INDICATOR_START_FROM_Y_PLUS;
    } else {
        this.u2StartFrom = RING_INDICATOR_START_FROM_X_PLUS;
    }
}

ARM_NONNULL(1)
arm_2d_size_t ring_indication_get_size(ring_indication_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.Foreground.ptTile);

    return (arm_2d_size_t) {
        .iHeight = this.iDiameter,
        .iWidth = this.iDiameter,
    };
}

ARM_NONNULL(1)
void ring_indication_depose( ring_indication_t *ptThis)
{
    assert(NULL != ptThis);
    meter_pointer_depose(&this.tSector);
}


ARM_NONNULL(1)
void ring_indication_on_load( ring_indication_t *ptThis)
{
    assert(NULL != ptThis);

    meter_pointer_on_load(&this.tSector);
}

ARM_NONNULL(1)
bool ring_indication_on_frame_start( ring_indication_t *ptThis, int32_t nValue)
{
    assert(NULL != ptThis);
    return meter_pointer_on_frame_start(&this.tSector, 
                                        nValue, 
                                        this.fSectorScale);
}

ARM_NONNULL(1)
bool ring_indication_on_frame_start_f32(ring_indication_t *ptThis, 
                                        float fTargetValue)
{
    return meter_pointer_on_frame_start_f32(&this.tSector, 
                                            fTargetValue, 
                                            this.fSectorScale);
}

ARM_NONNULL(1)
void ring_indication_on_frame_complete( ring_indication_t *ptThis)
{
    assert(NULL != ptThis);
    meter_pointer_on_frame_complete(&this.tSector);
}

__attribute__((noinline))
static
void __ring_indication_draw_sector( ring_indication_t *ptThis,
                                    arm_2d_tile_t *ptTile,
                                    arm_2d_region_t *ptPeepholeRegion,
                                    arm_2d_point_float_t *ptPivot)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);
    assert(NULL != ptPeepholeRegion);

    arm_2d_tile_t tTemp, tTempCanvas;
    arm_2d_tile_t *ptNewTargetTile = 
        arm_2d_tile_create_peephole(ptTile, 
                                    ptPeepholeRegion,
                                    &tTempCanvas,
                                    &tTemp);

    meter_pointer_show( &this.tSector, 
                        ptNewTargetTile, 
                        NULL, 
                        ptPivot, 
                        255);
}

__attribute__((noinline))
static
void __ring_indication_draw_quadrant(   ring_indication_t *ptThis,
                                        arm_2d_tile_t *ptTile,
                                        arm_2d_region_t *ptPeepholeRegion)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);
    assert(NULL != ptPeepholeRegion);

    arm_2d_tile_t tTemp, tTempCanvas;
    arm_2d_tile_t *ptNewTargetTile = 
        arm_2d_tile_create_peephole(ptTile, 
                                    ptPeepholeRegion,
                                    &tTempCanvas,
                                    &tTemp);

    if (NULL != this.Foreground.ptMask) {
        if (NULL != this.Background.ptMask) {
            arm_2d_tile_copy_with_masks(  
                                this.Foreground.ptTile,
                                this.Foreground.ptMask,
                                ptNewTargetTile,
                                this.Background.ptMask,
                                NULL);
        } else {
            arm_2d_tile_copy_with_src_mask(  
                                this.Foreground.ptTile,
                                this.Foreground.ptMask,
                                ptNewTargetTile,
                                NULL);
        }
    } else if (NULL != this.Background.ptMask) {
        arm_2d_tile_copy_with_des_mask(  
                                this.Foreground.ptTile,
                                ptNewTargetTile,
                                this.Background.ptMask,
                                NULL);
    } else {
        arm_2d_tile_copy_only(  this.Foreground.ptTile,
                                ptNewTargetTile,
                                NULL);
    }
}


ARM_NONNULL(1)
void ring_indication_show(  ring_indication_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion,
                            bool bIsNewFrame)
{
    assert(NULL!= ptThis);
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    arm_2d_container(ptTile, __visible_window, ptRegion) {

        arm_2d_align_centre(__visible_window_canvas, 
                            this.iDiameter,
                            this.iDiameter) {

            arm_2d_align_top_centre(__centre_region, 
                                    this.Foreground.ptTile->tRegion.tSize) {

                arm_2d_container(   &__visible_window, 
                                    __ring_indicator_panel, 
                                    &__top_centre_region) {

                    arm_2d_align_top_centre(__ring_indicator_panel_canvas,
                                            this.iDiameter,
                                            this.iDiameter) {

                        arm_2d_size_t tQuadrantSize = {
                            .iWidth = this.iDiameter >> 1,
                            .iHeight = this.iDiameter >> 1,
                        };

                        float fCurrentAngle 
                            = spin_zoom_widget_get_current_angle(
                                &this.tSector.use_as__spin_zoom_widget_t);

                        fCurrentAngle = fmodf(fCurrentAngle, 360.0f);

                        uint_fast8_t chQuadrantIndex;

                        if (fCurrentAngle >= 270.0f) {
                            /* Y- X+ */
                            chQuadrantIndex = 3;
                        } else if (fCurrentAngle >= 180.0f) {
                            /* Y- X- */
                            chQuadrantIndex = 2;
                        } else if (fCurrentAngle >=90.0f) {
                            /* Y+ X- */
                            chQuadrantIndex = 1;
                        } else {
                            /* Y+ X+ */
                            chQuadrantIndex = 0;
                        }

                        bool bIsSectorDrawn = false;

                        arm_2d_point_float_t tPivot = {
                                .fX = (float)__top_centre_region.tLocation.iX 
                                    + (float)(__top_centre_region.tSize.iWidth - 1) / 2.0f,
                                .fY = (float)__top_centre_region.tLocation.iY 
                                    + (float)(__top_centre_region.tSize.iHeight - 1) / 2.0f,
                            };

                        switch (this.u2StartFrom) {
                            case RING_INDICATOR_START_FROM_Y_PLUS:
                                switch (chQuadrantIndex) {
                                    case 0: /* Y+ X+ */
                                        arm_2d_align_bottom_right_open(__top_centre_region, tQuadrantSize) {

                                            __ring_indication_draw_sector(  ptThis, 
                                                                            &__ring_indicator_panel, 
                                                                            &__bottom_right_region, 
                                                                            &tPivot);

                                            bIsSectorDrawn = true;
                                        }
                                        
                                    case 3: /* Y- X+ */
                                        arm_2d_align_top_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region );
                                            }
                                        }
                                    case 2: /* Y- X- */
                                        arm_2d_align_top_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region );
                                            }
                                        }
                                    case 1: /* Y+ X- */
                                        arm_2d_align_bottom_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region );
                                            }
                                        }
                                }
                                break;

                            case RING_INDICATOR_START_FROM_X_MINUS:
                                switch (chQuadrantIndex) {
                                    case 1: /* Y+ X- */
                                        arm_2d_align_bottom_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region );
                                            }
                                        }
                                    case 0: /* Y+ X+ */
                                        arm_2d_align_bottom_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {
                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region );
                                            }
                                        }
                                        
                                    case 3: /* Y- X+ */
                                        arm_2d_align_top_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region );
                                            }
                                        }
                                    case 2: /* Y- X- */
                                        arm_2d_align_top_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region );
                                            }
                                        }
                                }
                                break;

                            case RING_INDICATOR_START_FROM_Y_MINUS:
                                switch (chQuadrantIndex) {
                                    case 2: /* Y- X- */
                                        arm_2d_align_top_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region );
                                            }
                                        }
                                    case 1: /* Y+ X- */
                                        arm_2d_align_bottom_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region );
                                            }
                                        }
                                    case 0: /* Y+ X+ */
                                        arm_2d_align_bottom_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {
                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region );
                                            }
                                        }
                                        
                                    case 3: /* Y- X+ */
                                        arm_2d_align_top_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region );
                                            }
                                        }
                                }
                                break;

                            case RING_INDICATOR_START_FROM_X_PLUS:
                                switch (chQuadrantIndex) {
                                    case 3: /* Y- X+ */
                                        arm_2d_align_top_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_right_region );
                                            }
                                        }
                                    case 2: /* Y- X- */
                                        arm_2d_align_top_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region, 
                                                                                &tPivot);

                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__top_left_region );
                                            }
                                        }
                                    case 1: /* Y+ X- */
                                        arm_2d_align_bottom_left_open(__top_centre_region, tQuadrantSize) {
                                            if (!bIsSectorDrawn) {

                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_left_region );
                                            }
                                        }
                                    case 0: /* Y+ X+ */
                                        arm_2d_align_bottom_right_open(__top_centre_region, tQuadrantSize) {

                                            if (!bIsSectorDrawn) {
                                                __ring_indication_draw_sector(  ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region, 
                                                                                &tPivot);
                                                bIsSectorDrawn = true;
                                            } else {
                                                __ring_indication_draw_quadrant(ptThis, 
                                                                                &__ring_indicator_panel, 
                                                                                &__bottom_right_region );
                                            }
                                        }
                                }
                                break;
                        }
                    }
                }
            }
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
}

ARM_NONNULL(1)
int32_t ring_indication_get_current_value(ring_indication_t *ptThis)
{
    assert(NULL != ptThis);

    return meter_pointer_get_current_value(&this.tSector);
}

ARM_NONNULL(1)
int32_t ring_indication_set_current_value(ring_indication_t *ptThis, int32_t nValue)
{
    assert(NULL != ptThis);
    return meter_pointer_set_current_value(&this.tSector, nValue);
}

ARM_NONNULL(1)
float ring_indication_get_current_value_f32(ring_indication_t *ptThis)
{
    assert(NULL != ptThis);

    return meter_pointer_get_current_value_f32(&this.tSector);
}

ARM_NONNULL(1)
float ring_indication_set_current_value_f32(ring_indication_t *ptThis, float fValue)
{
    assert(NULL != ptThis);
    return meter_pointer_set_current_value_f32(&this.tSector, fValue);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
