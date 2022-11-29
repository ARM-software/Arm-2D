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

#ifdef __RTE_ACCELERATION_ARM_2D_SCENE4__

#define __USER_SCENE4_IMPLEMENT__
#include "arm_2d_scene_4.h"

#include "arm_2d_helper.h"
#include "arm_extra_controls.h"

#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"  
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern arm_2d_user_font_t ARM_2D_FONT_A4_DIGITS_ONLY;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene4_depose(arm_2d_scene_t *ptScene)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;

    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene 4                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene4_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene4_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene4_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene4_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(59000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene4_background_handler)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/



    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene4_handler)
{
    user_scene_4_t *ptThis = (user_scene_4_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /*-----------------------draw the foreground begin-----------------------*/
    
    /* following code is just a demo, you can remove them */
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_BLACK);


    if (bIsNewFrame) {
        int32_t iResult;
        
        /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(0, 1000, 60000, 0, &iResult, &this.lTimestamp[1]);
        
        if (this.hwGasgauge < iResult) {
            this.tStatus = BATTERY_STATUS_CHARGING;
        } else if (this.hwGasgauge > iResult) {
            this.tStatus = BATTERY_STATUS_DISCHARGING;
        }
        this.hwGasgauge = (uint16_t)iResult;
    }

    arm_2d_align_centre( ptTile->tRegion, 100, 130) {

        battery_gasgauge_nixie_tube_show(   &this.tBatteryNixieTube, 
                                            ptTile, 
                                            &__centre_region, 
                                            this.hwGasgauge,
                                            this.tStatus,
                                            bIsNewFrame);
        
        arm_2d_op_wait_async(NULL);
        
        arm_2d_size_t tTextSize = ARM_2D_FONT_A4_DIGITS_ONLY.use_as__arm_2d_font_t.tCharSize;
        tTextSize.iWidth *= 2;
        
        arm_2d_align_bottom_centre(__centre_region, tTextSize) {
        
            __bottom_centre_region.tLocation.iX += __centre_region.tLocation.iX;
            __bottom_centre_region.tLocation.iY += __centre_region.tLocation.iY;
            
            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
            arm_lcd_text_set_draw_region(&__bottom_centre_region);
            arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_BLACK);
            arm_lcd_text_location(0,0);
            arm_lcd_printf("%02d", this.hwGasgauge / 10);
            
            arm_2d_op_wait_async(NULL);
        }
    }

    /* draw text at the top-left corner */

    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
    arm_lcd_text_set_draw_region(NULL);
    arm_lcd_text_set_colour(GLCD_COLOR_NIXIE_TUBE, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene 4");

    /*-----------------------draw the foreground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_4_t *__arm_2d_scene4_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_4_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

//    /*! define dirty regions */
//    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

//        /* a dirty region to be specified at runtime*/
//        ADD_REGION_TO_LIST(s_tDirtyRegions,
//            0  /* initialize at runtime later */
//        ),
//        
//        /* add the last region:
//         * it is the top left corner for text display 
//         */
//        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
//            .tLocation = {
//                .iX = 0,
//                .iY = 0,
//            },
//            .tSize = {
//                .iWidth = __GLCD_CFG_SCEEN_WIDTH__,
//                .iHeight = 8,
//            },
//        ),

//    END_IMPL_ARM_2D_REGION_LIST()
//    
//    /* get the screen region */
//    arm_2d_region_t tScreen
//        = arm_2d_helper_pfb_get_display_area(
//            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
//    
//    /* initialise dirty region 0 at runtime
//     * this demo shows that we create a region in the centre of a screen(320*240)
//     * for a image stored in the tile c_tileCMSISLogoMask
//     */
//    s_tDirtyRegions[0].tRegion.tLocation = (arm_2d_location_t){
//        .iX = ((tScreen.tSize.iWidth - c_tileCMSISLogoMask.tRegion.tSize.iWidth) >> 1),
//        .iY = ((tScreen.tSize.iHeight - c_tileCMSISLogoMask.tRegion.tSize.iHeight) >> 1),
//    };
//    s_tDirtyRegions[0].tRegion.tSize = c_tileCMSISLogoMask.tRegion.tSize;
    
    if (NULL == ptThis) {
        ptThis = (user_scene_4_t *)malloc(sizeof(user_scene_4_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
        memset(ptThis, 0, sizeof(user_scene_4_t));
    }

    *ptThis = (user_scene_4_t){
        .use_as__arm_2d_scene_t = {
        /* Please uncommon the callbacks if you need them
         */
        //.fnBackground   = &__pfb_draw_scene4_background_handler,
        .fnScene        = &__pfb_draw_scene4_handler,
        //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        

        //.fnOnBGStart    = &__on_scene4_background_start,
        //.fnOnBGComplete = &__on_scene4_background_complete,
        //.fnOnFrameStart = &__on_scene4_frame_start,
        .fnOnFrameCPL   = &__on_scene4_frame_complete,
        .fnDepose       = &__on_scene4_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    battery_gasgauge_nixie_tube_init(&this.tBatteryNixieTube);

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}




#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

