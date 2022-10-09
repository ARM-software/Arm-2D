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
#include "./shape_round_corner_box.h"
#include <time.h>

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
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_gray8_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_gray8_t

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_rgb565_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_rgb565_t

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#define arm_2d_fill_colour_with_mask                                            \
            arm_2d_cccn888_fill_colour_with_mask
#   define __arm_2d_color_t         arm_2d_color_cccn888_t

#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/


/*============================ PROTOTYPES ====================================*/

/*============================ LOCAL VARIABLES ===============================*/



/*============================ IMPLEMENTATION ================================*/
void progress_wheel_transform_show(
                const arm_2d_tile_t 	*ptTarget, 			//!< target tile
                const arm_2d_tile_t 	*c_tile_myCIRCLE,	//!< source tile
                const arm_2d_region_t  	alignment_region,	//!< target region
                float 					fAngle,				//!< rotation angle
                const uint16_t 			fCircleDiameter,	//!< DIAMETER						
                bool 					bIsNewFrame){

	
    static arm_2d_op_trans_opa_t ptOP;
    static arm_2d_op_trans_opa_t ptOP3;
    static arm_2d_op_trans_opa_t ptOP2;
    static arm_2d_op_trans_opa_t ptOP1;
    static float s_fAngle = 0.0f;

    float  scale = (float)(fCircleDiameter/(c_tile_myCIRCLE->tRegion.tSize.iHeight*2.0f-1.0f));
    arm_2d_region_t rotate_region = alignment_region;

    const arm_2d_location_t c_tCentre = {
        .iX = 0,
        .iY = c_tile_myCIRCLE->tRegion.tSize.iHeight -1,
    };
    const arm_2d_location_t c_tTargetCentre = {            
        .iX = alignment_region.tLocation.iX + (alignment_region.tSize.iWidth>>1),
        .iY = alignment_region.tLocation.iY +(alignment_region.tSize.iWidth>>1),
    };

    if (bIsNewFrame) {           
        s_fAngle = ARM_2D_ANGLE(fAngle);
    }
    if(s_fAngle >= ARM_2D_ANGLE(270)){			
        rotate_region.tSize.iWidth = (alignment_region.tSize.iWidth +1)>>1;

    }
    arm_2dp_rgb565_tile_transform_with_opacity(
                &ptOP,				//__CB_ADDR
                c_tile_myCIRCLE,	//__SRC_TILE_ADDR
                ptTarget,			//__DES_TILE_ADDR 
                &rotate_region,		//__DES_REGION_ADDR
                c_tCentre,			//__CENTRE
                s_fAngle,			//__ANGLE
                scale,				//__SCALE
                GLCD_COLOR_BLACK,	//__MSK_COLOUR
                255,  //
                &c_tTargetCentre

    );

    if(s_fAngle < ARM_2D_ANGLE(90)){	
    //=======3==================
        arm_2dp_rgb565_tile_transform_with_opacity(
            &ptOP3,				//__CB_ADDR
            c_tile_myCIRCLE,	//__SRC_TILE_ADDR
            ptTarget,			//__DES_TILE_ADDR 
            &rotate_region,
            c_tCentre,			//__CENTRE
            ARM_2D_ANGLE(90),	//__ANGLE
            scale,				//__SCALE
            GLCD_COLOR_BLACK,	//__MSK_COLOUR
            255,
            &c_tTargetCentre ); //
    }
    if(s_fAngle < ARM_2D_ANGLE(180)){	
        //=======2==================
        arm_2dp_rgb565_tile_transform_with_opacity(
            &ptOP2,				//__CB_ADDR
            c_tile_myCIRCLE,	//__SRC_TILE_ADDR
            ptTarget,			//__DES_TILE_ADDR 
            &rotate_region,		//__DES_REGION_ADDR
            c_tCentre,			//__CENTRE
            ARM_2D_ANGLE(180),	//__ANGLE
            scale,				//__SCALE
            GLCD_COLOR_BLACK,	//__MSK_COLOUR
            255,
            &c_tTargetCentre ); //
    } 
    if(s_fAngle < ARM_2D_ANGLE(270)){	
    //=======1==================
        arm_2dp_rgb565_tile_transform_with_opacity(
            &ptOP1,				//__CB_ADDR
            c_tile_myCIRCLE,	//__SRC_TILE_ADDR
            ptTarget,			//__DES_TILE_ADDR 
            &rotate_region,		//__DES_REGION_ADDR
            c_tCentre,			//__CENTRE
            ARM_2D_ANGLE(270),	//__ANGLE
            scale,//__SCALE
            GLCD_COLOR_BLACK,	//__MSK_COLOUR
            255,
            &c_tTargetCentre ); //
    }
	
		
}






#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
