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

#include "__common.h"

#include "./spinning_wheel.h"
#include "./shape_round_corner_box.h"

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileSpinWheelMask;
extern const arm_2d_tile_t c_tileQuaterArcMask;
extern const arm_2d_tile_t c_tileGreenCircleMask;

/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/

static
const arm_2d_tile_t c_tileGreenCircleQuaterMask = 
    impl_child_tile(
        c_tileGreenCircleMask,
        27,
        27,
        28,
        28,
    );

/*============================ IMPLEMENTATION ================================*/


void spinning_wheel_init(void)
{

}

void spinning_wheel_show(const arm_2d_tile_t *ptTarget, bool bIsNewFrame)
{
    assert(NULL != ptTarget);
    ARM_2D_UNUSED(ptTarget);
    ARM_2D_UNUSED(bIsNewFrame);
    static float s_fAngle = 0.0f;

    arm_2d_align_centre(ptTarget->tRegion, 100, 100) {
        
        draw_round_corner_box(  ptTarget, 
                                &__centre_region,
                                GLCD_COLOR_BLACK,
                                64,
                                bIsNewFrame);    
        arm_2d_op_wait_async(NULL);
    }

    
    //! spin mask
    do {
        static arm_2d_op_fill_cl_msk_opa_trans_t s_tMaskRotateCB = {0};
    
        if (bIsNewFrame) {
            s_fAngle += ARM_2D_ANGLE(6.0f);
            s_fAngle = fmodf(s_fAngle,ARM_2D_ANGLE(360));
        }

        arm_2d_location_t c_tCentre = {
            .iX = c_tileSpinWheelMask.tRegion.tSize.iWidth >> 1,
            .iY = c_tileSpinWheelMask.tRegion.tSize.iHeight >> 1,
        };

        arm_2d_align_centre(ptTarget->tRegion, c_tileSpinWheelMask.tRegion.tSize) {
            arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &s_tMaskRotateCB,
                                            &c_tileSpinWheelMask,
                                            ptTarget,
                                            &__centre_region,
                                            c_tCentre,
                                            s_fAngle,
                                            1.0f,
                                            GLCD_COLOR_WHITE,
                                            254);

            arm_2d_op_wait_async(NULL);
        }
    } while(0);

}


void spinning_wheel2_show(  const arm_2d_tile_t *ptTarget, 
                            COLOUR_INT Colour, 
                            bool bIsNewFrame)
{
    assert(NULL != ptTarget);
    ARM_2D_UNUSED(ptTarget);
    ARM_2D_UNUSED(bIsNewFrame);
    static float s_fAngle = 0.0f;



#if 0

    arm_2d_align_centre(ptTarget->tRegion, 61, 61) {

        arm_2d_fill_colour_with_mask_and_opacity( ptTarget,
                                                  &__centre_region,
                                                  &c_tileQuaterArcMask,
                                                  (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY},
                                                  128);
        static arm_2d_op_rotate_t s_tMaskRotateCB = {0};
        
        const arm_2d_location_t c_tCentre = {
            .iX = 30,
            .iY = 30,
        };

        //! spin mask
        do {
            
            memset(s_tileSpinWheelMask.pchBuffer, 0, sizeof(s_tileSpinWheelMaskBuffer));
            
            arm_2d_gray8_tile_rotation(//&s_tMaskRotateBG,
                                        &c_tileQuaterArcMask,
                                        &s_tileSpinWheelMask,
                                        NULL,
                                        c_tCentre,
                                        ARM_2D_ANGLE(90.0f),
                                        0x00);

            arm_2d_op_wait_async(NULL);
            
            arm_2d_fill_colour_with_mask_and_opacity( ptTarget,
                                                      &__centre_region,
                                                      &s_tileSpinWheelMask,
                                                      (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY},
                                                      128);
            
            arm_2d_op_wait_async(NULL);
        } while (0);
        
        do {
            
            memset(s_tileSpinWheelMask.pchBuffer, 0, sizeof(s_tileSpinWheelMaskBuffer));
            
            arm_2d_gray8_tile_rotation(//&s_tMaskRotateBG,
                                        &c_tileQuaterArcMask,
                                        &s_tileSpinWheelMask,
                                        NULL,
                                        c_tCentre,
                                        ARM_2D_ANGLE(180.0f),
                                        0x00);

            arm_2d_op_wait_async(NULL);
            
            arm_2d_fill_colour_with_mask_and_opacity( ptTarget,
                                                      &__centre_region,
                                                      &s_tileSpinWheelMask,
                                                      (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY},
                                                      128);
            
            arm_2d_op_wait_async(NULL);
        } while (0);

        do {
            
            memset(s_tileSpinWheelMask.pchBuffer, 0, sizeof(s_tileSpinWheelMaskBuffer));
            
            arm_2d_gray8_tile_rotation(//&s_tMaskRotateBG,
                                        &c_tileQuaterArcMask,
                                        &s_tileSpinWheelMask,
                                        NULL,
                                        c_tCentre,
                                        ARM_2D_ANGLE(270.0f),
                                        0x00);

            arm_2d_op_wait_async(NULL);
            
            arm_2d_fill_colour_with_mask_and_opacity( ptTarget,
                                                      &__centre_region,
                                                      &s_tileSpinWheelMask,
                                                      (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY},
                                                      128);
            
            arm_2d_op_wait_async(NULL);
        } while (0);

        do {
            
            memset(s_tileSpinWheelMask.pchBuffer, 0, sizeof(s_tileSpinWheelMaskBuffer));

            if (bIsNewFrame) {
                s_fAngle += ARM_2D_ANGLE(6.0f);
                s_fAngle = fmodf(s_fAngle,ARM_2D_ANGLE(360));
            }

            arm_2dp_gray8_tile_rotation(&s_tMaskRotateCB,
                                        &c_tileQuaterArcMask,
                                        &s_tileSpinWheelMask,
                                        NULL,
                                        c_tCentre,
                                        s_fAngle,
                                        0x00);

            arm_2d_op_wait_async(NULL);
            
            arm_2d_fill_colour_with_mask_and_opacity( ptTarget,
                                                      &__centre_region,
                                                      &s_tileSpinWheelMask,
                                                      (__arm_2d_color_t){Colour},
                                                      128);
            
            arm_2d_op_wait_async(NULL);
        } while (0);
    }
#else
    arm_2d_align_centre(ptTarget->tRegion, 55, 55) {
        arm_2d_fill_colour_with_mask_and_opacity(  
                                        ptTarget,
                                        &__centre_region,
                                        &c_tileGreenCircleMask,
                                        (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY},
                                        128);
        arm_2d_op_wait_async(NULL);

        static arm_2d_op_fill_cl_msk_opa_trans_t s_tMaskRotateCB = {0};
        
        const arm_2d_location_t c_tCentre = {
            .iX = 0,
            .iY = 0,
        };

        
        if (bIsNewFrame) {
            s_fAngle += ARM_2D_ANGLE(6.0f);
            s_fAngle = fmodf(s_fAngle,ARM_2D_ANGLE(360));
        }

        arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                        &s_tMaskRotateCB,
                                        &c_tileGreenCircleQuaterMask,
                                        ptTarget,
                                        &__centre_region,
                                        c_tCentre,
                                        s_fAngle,
                                        1.0f,
                                        Colour,
                                        64
                                    );

        arm_2d_op_wait_async(NULL);
        
    }
#endif

}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
