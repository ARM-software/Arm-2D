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
#define __SPIN_ZOOM_WIDGET_IMPLEMENT__

#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "spin_zoom_widget.h"
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
/*============================ PROTOTYPES ====================================*/

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_fill_colour( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity);

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_with_mask( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity);

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_only( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity);

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_colour_keying( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

spin_zoom_widget_mode_t SPIN_ZOOM_MODE_FILL_COLOUR = {
    .fnTransform = &__spin_zoom_widget_transform_mode_fill_colour,
};

spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_MASK = {
    .fnTransform = &__spin_zoom_widget_transform_mode_tile_with_mask,
};

spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_ONLY = {
    .fnTransform = &__spin_zoom_widget_transform_mode_tile_only,
};

spin_zoom_widget_mode_t SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING = {
    .fnTransform = &__spin_zoom_widget_transform_mode_tile_only,
};

/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void spin_zoom_widget_init( spin_zoom_widget_t *ptThis,
                            spin_zoom_widget_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(spin_zoom_widget_t));
    this.tCFG = *ptCFG;

    /* initialize op */
    ARM_2D_OP_INIT(this.OPCODE);
}

ARM_NONNULL(1)
void spin_zoom_widget_depose( spin_zoom_widget_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_transform_depose(&this.tHelper);
    }

    /* depose op */
    ARM_2D_OP_DEPOSE(this.OPCODE);
}

ARM_NONNULL(1)
void spin_zoom_widget_on_load( spin_zoom_widget_t *ptThis)
{
    assert(NULL != ptThis);

    float fAngleStep = 0.05f;
    float fScaleStep = 0.01f;

    if (this.tCFG.Indicator.Step.fAngle > 0.0f) {
        fAngleStep = this.tCFG.Indicator.Step.fAngle;
    }

    if (this.tCFG.Indicator.Step.fScale > 0.0f) {
        fScaleStep = this.tCFG.Indicator.Step.fScale;
    }
    
    if (NULL != this.tCFG.ptScene) {
        /* initialize transform helper */
        arm_2d_helper_dirty_region_transform_init(
                                    &this.tHelper,
                                    &this.tCFG.ptScene->tDirtyRegionHelper,
                                    (arm_2d_op_t *)&this.OPCODE,
                                    fAngleStep,
                                    fScaleStep);
    }
}

ARM_NONNULL(1)
void spin_zoom_widget_on_frame_start( spin_zoom_widget_t *ptThis, int32_t nValue, float fScale)
{
    assert(NULL != ptThis);
    
    int32_t nDelta = nValue - this.tCFG.Indicator.LowerLimit.nValue;
    float fDegreeDistance = this.tCFG.Indicator.UpperLimit.fAngleInDegree
                          - this.tCFG.Indicator.LowerLimit.fAngleInDegree;
    int32_t nValueDistance = this.tCFG.Indicator.UpperLimit.nValue
                           - this.tCFG.Indicator.LowerLimit.nValue;
    
    float fAngle = (((float)nDelta / (float)nValueDistance) * fDegreeDistance)
                 + this.tCFG.Indicator.LowerLimit.fAngleInDegree;

    /* update helper with new values*/
    arm_2d_helper_dirty_region_transform_update_value(&this.tHelper, fAngle, fScale);

    /* call helper's on-frame-start event handler */
    arm_2d_helper_dirty_region_transform_on_frame_start(&this.tHelper);
}

ARM_NONNULL(1,2)
void spin_zoom_widget_show( spin_zoom_widget_t *ptThis,
                            const arm_2d_tile_t *ptTile,
                            const arm_2d_region_t *ptRegion,
                            const arm_2d_location_t *ptPivot,
                            uint8_t chOpacity)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    if (NULL == ptTile) {
        return ;
    }

    assert(NULL != ptThis);
    assert(NULL != this.tCFG.ptTransformMode);
    
    ARM_2D_INVOKE( this.tCFG.ptTransformMode->fnTransform,
        ARM_2D_PARAM(
            ptThis, 
            ptTile,
            ptRegion,
            ptPivot,
            chOpacity
        ));
}

ARM_NONNULL(1)
void spin_zoom_widget_set_colour( spin_zoom_widget_t *ptThis, COLOUR_INT_TYPE tColour)
{
    assert(NULL != ptThis);
    this.tCFG.Source.tColourToFill = tColour;
}

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_fill_colour( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTile);

    /* draw pointer */
    arm_2dp_fill_colour_with_mask_opacity_and_transform(
                        &this.OPCODE.tFillColourTransform,
                        this.tCFG.Source.ptMask,
                        ptTile,
                        ptRegion,
                        this.tCFG.Source.tCentre,
                        ARM_2D_ANGLE(this.tHelper.fAngle),
                        this.tHelper.fScale,
                        this.tCFG.Source.tColourToFill,
                        chOpacity,
                        ptPivot);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_transform_update(&this.tHelper,
                                                    ptRegion,
                                                    bIsNewFrame);
    }

    ARM_2D_OP_WAIT_ASYNC(&this.OPCODE);

    return arm_fsm_rt_cpl;

}

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_with_mask( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTile);

    arm_2dp_tile_transform_with_src_mask_and_opacity(
                                                &this.OPCODE.tTileTransform,
                                                this.tCFG.Source.ptSource,
                                                this.tCFG.Source.ptMask,
                                                ptTile,
                                                ptRegion,
                                                this.tCFG.Source.tCentre,
                                                ARM_2D_ANGLE(this.tHelper.fAngle),
                                                this.tHelper.fScale,
                                                chOpacity,
                                                ptPivot);
    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_transform_update(&this.tHelper,
                                                    ptRegion,
                                                    bIsNewFrame);
    }

    ARM_2D_OP_WAIT_ASYNC(&this.OPCODE);

    return arm_fsm_rt_cpl;

}

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_only( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTile);

    arm_2dp_tile_transform_only_with_opacity(   &this.OPCODE.tTile,
                                                this.tCFG.Source.ptSource,
                                                ptTile,
                                                ptRegion,
                                                this.tCFG.Source.tCentre,
                                                ARM_2D_ANGLE(this.tHelper.fAngle),
                                                this.tHelper.fScale,
                                                chOpacity,
                                                ptPivot);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_transform_update(&this.tHelper,
                                                    ptRegion,
                                                    bIsNewFrame);
    }

    ARM_2D_OP_WAIT_ASYNC(&this.OPCODE);

    return arm_fsm_rt_cpl;

}

static
arm_fsm_rt_t __spin_zoom_widget_transform_mode_tile_colour_keying( 
                                            spin_zoom_widget_t *ptThis, 
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_location_t *ptPivot,
                                            uint8_t chOpacity)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTile);

    arm_2dp_tile_transform_with_opacity(&this.OPCODE.tTile,
                                        this.tCFG.Source.ptSource,
                                        ptTile,
                                        ptRegion,
                                        this.tCFG.Source.tCentre,
                                        ARM_2D_ANGLE(this.tHelper.fAngle),
                                        this.tHelper.fScale,
                                        this.tCFG.Source.tColourForKeying,
                                        chOpacity,
                                        ptPivot);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_transform_update(&this.tHelper,
                                                    ptRegion,
                                                    bIsNewFrame);
    }

    ARM_2D_OP_WAIT_ASYNC(&this.OPCODE);

    return arm_fsm_rt_cpl;

}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
