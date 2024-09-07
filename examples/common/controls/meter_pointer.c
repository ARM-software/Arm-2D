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
#define __METER_POINTER_IMPLEMENT__
#define __SPIN_ZOOM_WIDGET_INHERIT__

#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "meter_pointer.h"
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
arm_2d_err_t meter_pointer_init(meter_pointer_t *ptThis,
                                meter_pointer_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(meter_pointer_t));

    /* initialize base class */
    do {
        spin_zoom_widget_cfg_t tCFG = ptCFG->tSpinZoom;
        if (ptCFG->Pointer.iRadius > 0) {
            /* override the source centre configuration */
            if (ptCFG->Pointer.bIsSourceHorizontal) {
                tCFG.Source.tCentre.iX = ptCFG->Pointer.iRadius;
                
                int16_t iHeight;
                if (NULL != tCFG.Source.ptSource) {
                    iHeight = tCFG.Source.ptSource->tRegion.tSize.iHeight;
                } else if (NULL != tCFG.Source.ptMask) {
                    iHeight = tCFG.Source.ptMask->tRegion.tSize.iHeight;
                } else {
                    assert(false);
                    return ARM_2D_ERR_INVALID_PARAM;
                }

                tCFG.Source.tCentre.iY = iHeight >> 1;
            } else {
                tCFG.Source.tCentre.iY = ptCFG->Pointer.iRadius;
                
                int16_t iWidth;
                if (NULL != tCFG.Source.ptSource) {
                    iWidth = tCFG.Source.ptSource->tRegion.tSize.iWidth;
                } else if (NULL != tCFG.Source.ptMask) {
                    iWidth = tCFG.Source.ptMask->tRegion.tSize.iWidth;
                } else {
                    assert(false);
                    return ARM_2D_ERR_INVALID_PARAM;
                }

                tCFG.Source.tCentre.iX = iWidth >> 1;
            }
        }

        tCFG.Indicator.LowerLimit.nValue;
        tCFG.Indicator.UpperLimit.nValue;

        spin_zoom_widget_init(&this.use_as__spin_zoom_widget_t, 
                              &tCFG);
    } while(0);

    /* initialize PI helper */
    do {
        this.tPISliderCFG = ptCFG->tPISliderCFG;
        if (    (this.tPISliderCFG.fProportion <= 0.0f) 
        ||   (this.tPISliderCFG.nInterval <= 0)) {
            /* use default parameters */
            arm_2d_helper_pi_slider_init(&this.tPISlider, 
                                         NULL, 
                                        ptCFG->nPISliderStartPosition);
        } else {
            arm_2d_helper_pi_slider_init(&this.tPISlider, 
                                        &this.tPISliderCFG, 
                                        ptCFG->nPISliderStartPosition);
        }
    } while(0);

    return ARM_2D_ERR_NONE;

}

ARM_NONNULL(1)
void meter_pointer_depose( meter_pointer_t *ptThis)
{
    assert(NULL != ptThis);
    
    spin_zoom_widget_depose(&this.use_as__spin_zoom_widget_t);
}

ARM_NONNULL(1)
void meter_pointer_on_load( meter_pointer_t *ptThis)
{
    assert(NULL != ptThis);
    
    spin_zoom_widget_on_load(&this.use_as__spin_zoom_widget_t);
}

#include <stdio.h>

ARM_NONNULL(1)
bool meter_pointer_on_frame_start(  meter_pointer_t *ptThis, 
                                    int32_t nTargetValue,
                                    float fScale)
{
    assert(NULL != ptThis);

    bool bFinished = arm_2d_helper_pi_slider_f32(&this.tPISlider, 
                                                (float)nTargetValue, 
                                                &this.fCurrentValue);

    spin_zoom_widget_on_frame_start_f32(&this.use_as__spin_zoom_widget_t,
                                        this.fCurrentValue,
                                        fScale);
    

    return bFinished;
}

ARM_NONNULL(1)
bool meter_pointer_on_frame_start_f32(  meter_pointer_t *ptThis, 
                                        float fTargetValue,
                                        float fScale)
{
    assert(NULL != ptThis);

    bool bFinished = arm_2d_helper_pi_slider_f32(&this.tPISlider, 
                                                fTargetValue, 
                                                &this.fCurrentValue);

    spin_zoom_widget_on_frame_start_f32(&this.use_as__spin_zoom_widget_t,
                                        this.fCurrentValue,
                                        fScale);
    

    return bFinished;
}

ARM_NONNULL(1)
void meter_pointer_on_frame_complete( meter_pointer_t *ptThis)
{
    assert(NULL != ptThis);

    spin_zoom_widget_on_frame_complete(&this.use_as__spin_zoom_widget_t);
}

ARM_NONNULL(1, 2)
void meter_pointer_show(meter_pointer_t *ptThis,
                        const arm_2d_tile_t *ptTile,
                        const arm_2d_region_t *ptRegion,
                        const arm_2d_location_t *ptPivot,
                        uint8_t chOpacity)
{
    spin_zoom_widget_show(&this.use_as__spin_zoom_widget_t,
                          ptTile,
                          ptRegion,
                          ptPivot,
                          chOpacity);

}

ARM_NONNULL(1)
void meter_pointer_set_colour(  meter_pointer_t *ptThis, 
                                COLOUR_INT_TYPE tColour)
{
    assert(NULL != ptThis);
    spin_zoom_widget_set_colour(&this.use_as__spin_zoom_widget_t, tColour);
}

ARM_NONNULL(1)
int32_t meter_pointer_get_current_value(meter_pointer_t *ptThis)
{
    assert(NULL != ptThis);

    return (int32_t)this.fCurrentValue;
}

ARM_NONNULL(1)
int32_t meter_pointer_set_current_value(meter_pointer_t *ptThis, int32_t nValue)
{
    assert(NULL != ptThis);

    this.fCurrentValue = (float)nValue;
    arm_2d_helper_pi_slider_set_current(&this.tPISlider, nValue);

    return (int32_t)this.fCurrentValue;
}

ARM_NONNULL(1)
float meter_pointer_get_current_value_f32(meter_pointer_t *ptThis)
{
    assert(NULL != ptThis);

    return this.fCurrentValue;
}

ARM_NONNULL(1)
float meter_pointer_set_current_value_f32(meter_pointer_t *ptThis, float fValue)
{
    assert(NULL != ptThis);

    this.fCurrentValue = fValue;
    arm_2d_helper_pi_slider_set_current_f32(&this.tPISlider, fValue);

    return this.fCurrentValue;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
