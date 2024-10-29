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

#if defined(_RTE_)
#   include "RTE_Components.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include "arm_2d.h"

#define __USER_SCENE_FAN_IMPLEMENT__
#include "arm_2d_scene_fan.h"

#include "arm_2d_helper.h"
#include "arm_2d_example_controls.h"

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
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
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
enum {
    DIRTY_REGION_PIVOT,
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileWhiteDotMiddleA4Mask;
extern const arm_2d_tile_t c_tileFanBladeMask;

extern 
const arm_2d_tile_t c_tileListCoverLineMask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}   ARM_2D_FONT_Arial14_A8,
    ARM_2D_FONT_Arial14_A4,
    ARM_2D_FONT_Arial14_A2,
    ARM_2D_FONT_Arial14_A1;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        0
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

static arm_2d_location_t s_tFanCentre;

static const
struct {
    COLOUR_INT tColour;
    float fSpeed;
} c_tFanLevel[] = {
    [FAN_LEVEL_STOP] = {
        .tColour = GLCD_COLOR_DARK_GREY,
        .fSpeed = 0.0f,
    },
    [FAN_LEVEL_ECO] = {
        .tColour = GLCD_COLOR_GREEN,
        .fSpeed = 3.0f,
    },
    [FAN_LEVEL_NORMAL] = {
        .tColour = GLCD_COLOR_BLUE,
        .fSpeed = 5.0f,
    },
    [FAN_LEVEL_COOLING] = {
        .tColour = __RGB(0xFF, 0xA5, 0x00),
        .fSpeed = 8.0f,
    },
};

const static __disp_string_t c_strLevels[] = {
    [FAN_LEVEL_STOP]    = "STOP",
    [FAN_LEVEL_ECO]     = "ECO Mode",
    [FAN_LEVEL_NORMAL]  = "Normal Mode",
    [FAN_LEVEL_COOLING] = "Cooling Mode",
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_fan_load(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_foreach(spin_zoom_widget_t, this.tFanBlades, ptFanBlade) {
        spin_zoom_widget_on_load(ptFanBlade);
    }
}

static void __on_scene_fan_depose(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_foreach(spin_zoom_widget_t, this.tFanBlades, ptFanBlade) {
        spin_zoom_widget_depose(ptFanBlade);
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene fan                                                                  *
 *----------------------------------------------------------------------------*/

static void __on_scene_fan_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_fan_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_fan_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    bool bMoveToStop = false;

    /* demo code */
    if (arm_2d_helper_is_time_out(4000, &this.lTimestamp[1])) {
        this.chLevel++;
        if (this.chLevel >= 4) {
            this.chLevel = 0;
            bMoveToStop = true;
        }

        text_list_move_selection(&this.tLevelList, 1, 300);
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_PIVOT],
                                            false); 
        
    } else {
        arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[DIRTY_REGION_PIVOT],
                                            true); 
    }

    /* implement physical acceleration effect */
    do {
        float fTargetSpeed = c_tFanLevel[this.chLevel].fSpeed;

        if (this.fCurrentSpeed != fTargetSpeed) {
            int32_t nSpeed = 0;

            if (arm_2d_helper_time_liner_slider(this.fCurrentSpeed * 10.0f, 
                                                fTargetSpeed * 10.0f,
                                                3000,
                                                &nSpeed,
                                                &this.lTimestamp[2])) {
                this.fCurrentSpeed = fTargetSpeed;
                this.lTimestamp[2] = 0;
            } else {
                this.fCurrentSpeed = nSpeed / 10.0f;
            }
        }
    } while(0);

    this.fAngle += this.fCurrentSpeed;
    this.fAngle = fmodf(this.fAngle, 120.0f);

    text_list_on_frame_start(&this.tLevelList);

    arm_foreach(spin_zoom_widget_t, this.tFanBlades, ptFanBlade) {
        spin_zoom_widget_set_colour(ptFanBlade, c_tFanLevel[this.chLevel].tColour);
        spin_zoom_widget_on_frame_start(ptFanBlade, (int32_t)(this.fAngle * 10.0f), 1.0f);
    }
}

static void __on_scene_fan_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_foreach(spin_zoom_widget_t, this.tFanBlades, ptFanBlade) {
        spin_zoom_widget_on_frame_complete(ptFanBlade);
    }

    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(13000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_fan_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_fan_t *ptThis = (user_scene_fan_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_fan_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_fan_t *ptThis = (user_scene_fan_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the foreground begin-----------------------*/

        /* draw fan*/
        arm_2d_align_centre(__top_canvas, 140, 220) {

            arm_2d_layout(__centre_region, true) {

                __item_line_dock_vertical(160) {
                
                    arm_foreach(spin_zoom_widget_t, this.tFanBlades, ptFanBlade) {
                        spin_zoom_widget_show(ptFanBlade, ptTile, &__item_region, NULL, 255);
                    }

                    /* draw fan pivot */
                    arm_2d_align_centre(__item_region, 
                                        c_tileWhiteDotMiddleA4Mask.tRegion.tSize) {
                        arm_2d_fill_colour_with_a4_mask(
                                    ptTile,
                                    &__centre_region,
                                    &c_tileWhiteDotMiddleA4Mask, 
                                    (__arm_2d_color_t){c_tFanLevel[this.chLevel].tColour});
                    }
                }

                __item_line_dock_vertical() {

                    while(arm_fsm_rt_cpl != text_list_show(   
                                                    &this.tLevelList, 
                                                    ptTile, 
                                                    &__item_region, 
                                                    bIsNewFrame));

                }

            }
        }

        

    /*-----------------------draw the foreground end  -----------------------*/
    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_number_list_draw_cover)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    number_list_t *ptThis = (number_list_t *)pTarget;

    arm_2d_canvas(ptTile, __list_cover) {
        /* draw shadow */
        arm_2d_dock_top(__list_cover, 40) {
            arm_2d_fill_colour_with_vertical_alpha_gradient(
                ptTile,
                &__top_region,
                (__arm_2d_color_t){GLCD_COLOR_BLACK},
                (arm_2d_alpha_samples_2pts_t) {
                    {255, 0,},
                });
        }

        arm_2d_dock_bottom(__list_cover, 40) {
            arm_2d_fill_colour_with_vertical_alpha_gradient(
                ptTile,
                &__bottom_region,
                (__arm_2d_color_t){GLCD_COLOR_BLACK},
                (arm_2d_alpha_samples_2pts_t) {
                    {0, 255,},
                });
        }
    }

    arm_2d_op_wait_async(NULL);
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_fan_t *__arm_2d_scene_fan_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_fan_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /*-----------------------   static dirty region   -----------------------*/
    do {
        /* get the screen region */
        arm_2d_region_t tScreen
            = arm_2d_helper_pfb_get_display_area(
                &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

        /* draw fan*/
        arm_2d_align_centre(tScreen, 140, 220) {

            arm_2d_layout(__centre_region) {

                __item_line_dock_vertical(160) {
                    /* draw fan pivot */
                    arm_2d_align_centre(__item_region, 
                                        c_tileWhiteDotMiddleA4Mask.tRegion.tSize) {
                        s_tDirtyRegions[DIRTY_REGION_PIVOT].tRegion = __centre_region;
                    }
                }

            #if 0
                __item_line_dock_vertical() {

                }
            #endif
            }
        }

        
    } while(0);
    /*-----------------------   static dirty region   -----------------------*/

    if (NULL == ptThis) {
        ptThis = (user_scene_fan_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_fan_t),
                                                        __alignof__(user_scene_fan_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_fan_t));

    *ptThis = (user_scene_fan_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_fan_load,
            .fnScene        = &__pfb_draw_scene_fan_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_fan_background_start,
            //.fnOnBGComplete = &__on_scene_fan_background_complete,
            .fnOnFrameStart = &__on_scene_fan_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_fan_switching_out,
            .fnOnFrameCPL   = &__on_scene_fan_frame_complete,
            .fnDepose       = &__on_scene_fan_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_fan_t begin ---------------*/

    s_tFanCentre.iX = (c_tileFanBladeMask.tRegion.tSize.iWidth >> 1) - 5;
    s_tFanCentre.iY = 70;

    do {
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 0.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 360.0f,
                    .nValue = 3600,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tileFanBladeMask,
                .tCentre = s_tFanCentre,
                .tColourToFill = GLCD_COLOR_RED,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tFanBlades[0], &tCFG);
    } while(0);

    do {
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 120.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 480.0f,
                    .nValue = 3600,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tileFanBladeMask,
                .tCentre = s_tFanCentre,
                .tColourToFill = GLCD_COLOR_RED,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tFanBlades[1], &tCFG);
    } while(0);

    do {
        spin_zoom_widget_cfg_t tCFG = {
            .Indicator = {
                .LowerLimit = {
                    .fAngleInDegree = 240.0f,
                    .nValue = 0,
                },
                .UpperLimit = {
                    .fAngleInDegree = 600.0f,
                    .nValue = 3600,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tileFanBladeMask,
                .tCentre = s_tFanCentre,
                .tColourToFill = GLCD_COLOR_RED,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tFanBlades[2], &tCFG);
    } while(0);

    /* initialize text list */
    do {
        text_list_cfg_t tCFG = {
            .ptStrings = (__disp_string_t *)c_strLevels,

            .use_as____simple_list_cfg_t = {
                .hwCount = dimof(c_strLevels),
                
                .tFontColour = GLCD_COLOR_WHITE,
                .tBackgroundColour = GLCD_COLOR_BLACK,
                .chNextPadding = 3,
                .chPreviousPadding = 3,
                .tListSize = {
                    .iHeight = 80,
                    .iWidth = 0,            /* automatically set the width */
                },
                .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_Arial14_A4,
                /* draw list cover */
                .fnOnDrawListCover = &__arm_2d_number_list_draw_cover,

                .bUseDirtyRegion = true,
                .ptTargetScene = &this.use_as__arm_2d_scene_t,
            }
        };
        text_list_init(&this.tLevelList, &tCFG);

        this.chLevel = FAN_LEVEL_ECO;
        text_list_move_selection(&this.tLevelList, FAN_LEVEL_ECO, 0);
    } while(0);

    /* ------------   initialize members of user_scene_fan_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


