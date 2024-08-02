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

#define __USER_SCENE_RULER_IMPLEMENT__
#include "arm_2d_scene_ruler.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

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
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion" 
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64,177
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
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

extern const arm_2d_tile_t c_tileClockMarkingMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),
    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 0,
            .iHeight = 8,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_ruler_load(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(
                        &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                        this.tNumberDirtyRegion,
                        dimof(this.tNumberDirtyRegion));


    arm_foreach(arm_2d_helper_dirty_region_item_t, this.tNumberDirtyRegion, ptItem) {
        ptItem->tRegionPatch.tLocation.iY = -1;
        ptItem->tRegionPatch.tSize.iHeight = 2;
    }

    arm_foreach(__ruler_meter_marking_t, this.tMarkings, ptMarking) {
        /* initialize transform helper */
        arm_2d_helper_dirty_region_transform_init(
                                    &ptMarking->tHelper,
                                    &ptScene->tDirtyRegionHelper,
                                    (arm_2d_op_t *)&ptMarking->tOP,
                                    ARM_2D_ANGLE(0.1f),
                                    0.1f);
                                    
    }


}

static void __on_scene_ruler_depose(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_foreach(__ruler_meter_marking_t, this.tMarkings, ptMarking) {
        ARM_2D_OP_DEPOSE(ptMarking->tOP);
    }

    arm_2d_helper_dirty_region_remove_items(
                            &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                            this.tNumberDirtyRegion,
                            dimof(this.tNumberDirtyRegion));

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene ruler                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_ruler_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_ruler_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_ruler_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int16_t nStepsToMove = 1; 
    uint16_t hwMoveSpeedInMs = 300;


    /* ---------- plesae do NOT modify code below unless you 100% sure ----- */
    uint16_t hwABSSteps = ABS(nStepsToMove);

    if (arm_2d_helper_is_time_out(1000, &this.lTimestamp[1])) {
        number_list_move_selection(&this.tNumberList, nStepsToMove, hwMoveSpeedInMs * hwABSSteps);
        this.lTimestamp[2] = 0;
        this.chStepLeftToMove = hwABSSteps * 2;
    }

    do {
        int32_t nResult = 0;
        int32_t nTo = (nStepsToMove < 0) ? -56 : 56;
        if (arm_2d_helper_time_liner_slider(0, 
                                            nTo, 
                                            hwMoveSpeedInMs / 2,
                                            &nResult,
                                            &this.lTimestamp[2]
                                            )) {
            if (this.chStepLeftToMove > 0) {
                if (--this.chStepLeftToMove) {
                    this.lTimestamp[2] = 0;
                }
            }
        }

        this.fAngle = -(float)nResult / 10.0f;

    } while(0);

    for (int32_t n = 0; n < dimof(this.tMarkings); n++) {

        /* update helper with new values*/
        arm_2d_helper_dirty_region_transform_update_value(  
            &this.tMarkings[n].tHelper, 
            ARM_2D_ANGLE(this.fAngle + 90.0f - 5.5f * (dimof(this.tMarkings) / 2) + n * 5.6f), 
            1.0f);

        /* call helper's on-frame-start event handler */
        arm_2d_helper_dirty_region_transform_on_frame_start(&this.tMarkings[n].tHelper);
    }

    number_list_on_frame_start(&this.tNumberList);

    /* ---------- plesae do NOT modify code above unless you 100% sure ----- */


}

static void __on_scene_ruler_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* switch to next scene after 10s */
    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
       arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }

}

static void __before_scene_ruler_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_ruler_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_ruler_t *ptThis = (user_scene_ruler_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_location_t tMarkingCentre = {
            .iX = c_tileClockMarkingMask.tRegion.tSize.iWidth >> 1,
            .iY = 284 + 60,
        };

        arm_2d_align_centre(__top_canvas, 76, 284 ) {

            arm_2d_container(ptTile, __ruler, &__centre_region) {

                /* draw meter markings */
                do {
                    arm_foreach(__ruler_meter_marking_t, this.tMarkings, ptMarking) {

                        arm_2d_location_t tRulerCentre = {
                            .iX = __ruler_canvas.tLocation.iX - 284 + c_tileClockMarkingMask.tRegion.tSize.iHeight,
                            .iY = __ruler_canvas.tLocation.iY + 284 / 2,
                        };

                        /* draw pointer */
                        arm_2dp_fill_colour_with_mask_opacity_and_transform(
                                            &ptMarking->tOP,
                                            &c_tileClockMarkingMask,
                                            &__ruler,
                                            &__ruler_canvas,
                                            tMarkingCentre,
                                            ptMarking->tHelper.fAngle,
                                            0.80f,
                                            GLCD_COLOR_WHITE,
                                            128,
                                            &tRulerCentre);

                        arm_2d_helper_dirty_region_transform_update(
                                                        &ptMarking->tHelper,
                                                        &__ruler_canvas,
                                                        bIsNewFrame);

                        ARM_2D_OP_WAIT_ASYNC(&ptMarking->tOP);
                    }
                } while(0);


                while(arm_fsm_rt_cpl != number_list_show(   &this.tNumberList, 
                                                            &__ruler,
                                                            &__ruler_canvas,
                                                            bIsNewFrame));
            
                /* draw shadow */
                arm_2d_dock_top(__ruler_canvas, 60) {
                    arm_2d_fill_colour_with_vertical_alpha_gradient(
                        &__ruler,
                        &__top_region,
                        (__arm_2d_color_t){GLCD_COLOR_BLACK},
                        (arm_2d_alpha_samples_2pts_t) {128, 0,});
                }

                arm_2d_dock_bottom(__ruler_canvas, 60) {
                    arm_2d_fill_colour_with_vertical_alpha_gradient(
                        &__ruler,
                        &__bottom_region,
                        (__arm_2d_color_t){GLCD_COLOR_BLACK},
                        (arm_2d_alpha_samples_2pts_t) {0, 128,});
                }
            }
        }

        /* draw border only when necessary */
        if (tScreenSize.iWidth > 76 && tScreenSize.iHeight > 284) {
            arm_2d_align_centre(__top_canvas, 76+4, 284+4 ) {
                draw_round_corner_border(   ptTile, 
                                            &__centre_region, 
                                            GLCD_COLOR_WHITE, 
                                            (arm_2d_border_opacity_t)
                                                {64, 64, 255-64, 255-64},
                                            (arm_2d_corner_opacity_t)
                                                {0, 128, 128, 128});
            }
        }
        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene ruler");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __ruler_number_list_draw_list_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    number_list_t *ptThis = (number_list_t *)ptItem->ptListView;
    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_size_t tListSize = __arm_2d_list_core_get_list_size(&this.use_as____arm_2d_list_core_t);

    int16_t iHalfLength = tListSize.iHeight >> 1;
    int16_t iRadius = tListSize.iHeight;

    float fRatio = ((float)(iHalfLength - ptParam->hwRatio) / (float)iHalfLength);

    COLOUR_INT_TYPE tColour =  arm_2d_pixel_from_brga8888( 
                                            __arm_2d_helper_colour_slider(
                                                __RGB32(0xFF, 0x80, 0), 
                                                __RGB32(0x80, 0x40, 0),
                                                iHalfLength,
                                                ptParam->hwRatio));

    arm_2d_canvas(ptTile, __top_container) {

        arm_lcd_text_set_scale(fRatio + 0.3f);
        arm_2d_size_t tTextSize = arm_lcd_get_string_line_box("00", &ARM_2D_FONT_A4_DIGITS_ONLY);

        arm_2d_align_centre(__top_container, tTextSize) {

            /* adjust item position around a curve*/
            do {
                float fYOffset =  (float)ptParam->hwRatio;
                fYOffset = (float)iRadius - sqrtf((float)(iRadius * iRadius) - (float)(fYOffset * fYOffset));

                __centre_region.tLocation.iX -= (int16_t) fYOffset + ((1.0f - fRatio) * (float)tTextSize.iWidth);
                __centre_region.tLocation.iX += 15;
                __centre_region.tLocation.iY += 3;
            } while(0);

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(tColour, GLCD_COLOR_BLACK);
            arm_lcd_text_location(0,0);
            
            arm_lcd_text_set_scale(fRatio + 0.3f);

            arm_lcd_text_reset_display_region_tracking();
            arm_lcd_printf("%02d", ptItem->hwID);

            //! update dirty regions

            do {
                if (!arm_2d_helper_pfb_is_region_active(ptTile, &__centre_region, true)) {
                    break;
                }
                
                user_scene_ruler_t *ptScene = (user_scene_ruler_t *)ptItem->pTarget;

                arm_2d_helper_dirty_region_item_t *ptDirtyRegionItem 
                    = &ptScene->tNumberDirtyRegion[ptItem->hwID & 0x07];

                arm_2d_region_t *ptTextRegion = arm_lcd_text_get_last_display_region();
                arm_2d_helper_dirty_region_update_item( &ptScene->use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                                        ptDirtyRegionItem,
                                                        (arm_2d_tile_t *)ptTile,
                                                        NULL,
                                                        ptTextRegion);

            } while(0);


            ARM_2D_OP_WAIT_ASYNC();
        }
    }
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_ruler_t *__arm_2d_scene_ruler_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_ruler_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].tRegion.tSize.iWidth 
                                                        = tScreen.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_ruler_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_ruler_t),
                                                        __alignof__(user_scene_ruler_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_ruler_t));

    *ptThis = (user_scene_ruler_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_ruler_load,
            .fnScene        = &__pfb_draw_scene_ruler_handler,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_ruler_background_start,
            //.fnOnBGComplete = &__on_scene_ruler_background_complete,
            .fnOnFrameStart = &__on_scene_ruler_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_ruler_switching_out,
            .fnOnFrameCPL   = &__on_scene_ruler_frame_complete,
            .fnDepose       = &__on_scene_ruler_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_ruler_t begin ---------------*/

    /* initialize number list */
    do {
        number_list_cfg_t tCFG = {
            .hwCount = 100,
            .nStart = 0,
            .iDelta = 1,

            .bIgnoreBackground = true,

            .tListSize = {
                .iHeight = 284,
                .iWidth = 76,
            },

            .tItemSize = {
                .iWidth = 76,
                .iHeight = 54,
            },

            .fnOnDrawListItem = &__ruler_number_list_draw_list_item,

            .bUseDirtyRegion = false,
            .ptTargetScene = &this.use_as__arm_2d_scene_t,
        };
        number_list_init(&this.tNumberList, &tCFG);
    } while(0);

    number_list_move_selection(&this.tNumberList, 50, 0);

    arm_foreach(__ruler_meter_marking_t, this.tMarkings, ptMarking) {
        ARM_2D_OP_INIT(ptMarking->tOP);
    }

    /* ------------   initialize members of user_scene_ruler_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


