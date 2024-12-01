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

#define __USER_SCENE_MONO_ICON_MENU_IMPLEMENT__
#include "arm_2d_scene_mono_icon_menu.h"

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

#define c_tileCarMask               c_tileCar24A1Mask
#define c_tileProcessorMask         c_tileProcessor24A1Mask
#define c_tileSettingsMask          c_tileSettings24A1Mask
#define c_tileDroneMask             c_tileDrone24A1Mask
#define c_tileUSBStorageMask        c_tileUSBStorage24A1Mask
#define c_tileDiagramMask           c_tileDiagram24A1Mask
#define c_tileHealthcareMask        c_tileHealthcare24A1Mask
#define c_tileMobileMask            c_tileMobile24A1Mask
#define c_tileItemsMask             c_tileItems24A1Mask
#define c_tileSmartMeterMask        c_tileSmartMeter24A1Mask
#define c_tileCameraMask            c_tileCamera24A1Mask
#define c_tileWearableMask          c_tileWearable24A1Mask

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCarMask;
extern const arm_2d_tile_t c_tileProcessorMask;
extern const arm_2d_tile_t c_tileSettingsMask;
extern const arm_2d_tile_t c_tileDroneMask;
extern const arm_2d_tile_t c_tileUSBStorageMask;
extern const arm_2d_tile_t c_tileDiagramMask;
extern const arm_2d_tile_t c_tileHealthcareMask;
extern const arm_2d_tile_t c_tileMobileMask;
extern const arm_2d_tile_t c_tileItemsMask;
extern const arm_2d_tile_t c_tileSmartMeterMask;
extern const arm_2d_tile_t c_tileCameraMask;
extern const arm_2d_tile_t c_tileWearableMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

enum {
    ICON_IDX_CAR = 0,
    ICON_IDX_PROCESSOR,
    ICON_IDX_SETTINGS,
    ICON_IDX_DRONE,
    ICON_IDX_USB_STORAGE,
    ICON_IDX_DIAGRAM,
    ICON_IDX_HEALTHCARE,
    ICON_IDX_MOBILE,
    ICON_IDX_ITEMS,
    ICON_IDX_SMART_METER,
    ICON_IDX_CAMERA,
    ICON_IDX_WEARABLE,
};

static
const arm_2d_tile_t *c_pIconTable[] = {
    [ICON_IDX_CAR] =            &c_tileCarMask,
    [ICON_IDX_PROCESSOR] =      &c_tileProcessorMask,
    [ICON_IDX_SETTINGS] =       &c_tileSettingsMask,
    [ICON_IDX_DRONE] =          &c_tileDroneMask,
    [ICON_IDX_USB_STORAGE] =    &c_tileUSBStorageMask,
    [ICON_IDX_DIAGRAM] =        &c_tileDiagramMask,
    [ICON_IDX_HEALTHCARE] =     &c_tileHealthcareMask,
    [ICON_IDX_MOBILE] =         &c_tileMobileMask,
    [ICON_IDX_ITEMS] =          &c_tileItemsMask,
    [ICON_IDX_SMART_METER] =    &c_tileSmartMeterMask,
    [ICON_IDX_CAMERA] =         &c_tileCameraMask,
    [ICON_IDX_WEARABLE] =       &c_tileWearableMask,
};

static const char *c_pIconNameTable[] = {
    [ICON_IDX_CAR] =            "Car",
    [ICON_IDX_PROCESSOR] =      "MCU",
    [ICON_IDX_SETTINGS] =       "Settings",
    [ICON_IDX_DRONE] =          "Drone",
    [ICON_IDX_USB_STORAGE] =    "USB Stick",
    [ICON_IDX_DIAGRAM] =        "Diagram",
    [ICON_IDX_HEALTHCARE] =     "Healthcare",
    [ICON_IDX_MOBILE] =         "Mobile",
    [ICON_IDX_ITEMS] =          "Objects",
    [ICON_IDX_SMART_METER] =    "Smart Meter",
    [ICON_IDX_CAMERA] =         "Camera",
    [ICON_IDX_WEARABLE] =       "Wearable",
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_mono_icon_menu_load(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                            this.tDirtyRegionItems,
                                            dimof(this.tDirtyRegionItems));

}

static void __after_scene_mono_icon_menu_switching(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_icon_menu_depose(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    arm_2d_helper_dirty_region_remove_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                            this.tDirtyRegionItems,
                                            dimof(this.tDirtyRegionItems));

    icon_list_depose(&this.tList);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene mono_icon_menu                                                       *
 *----------------------------------------------------------------------------*/

static void __on_scene_mono_icon_menu_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_icon_menu_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_mono_icon_menu_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (arm_2d_helper_is_time_out(1000, &this.lTimestamp[1])) {

        /* move list */
        icon_list_move_selection(&this.tList, 1, 100);
    }


    arm_2d_helper_dirty_region_item_suspend_update(
        &this.tDirtyRegionItems[1],
        !__arm_2d_list_core_is_list_moving(
            &this.tList.use_as____simple_list_t.use_as____arm_2d_list_core_t));


    icon_list_on_frame_start(&this.tList);

}

static void __on_scene_mono_icon_menu_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 12s */
    if (arm_2d_helper_is_time_out(12000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_mono_icon_menu_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mono_icon_menu_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_mono_icon_menu_t *ptThis = (user_scene_mono_icon_menu_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
        

        arm_2d_dock_vertical(__top_canvas, 64, 8) {

            arm_2d_layout(__vertical_region, BOTTOM_UP) {

                __item_line_dock_vertical(10, 0, 0, 0, 4) {

                    do {
                        if (__arm_2d_list_core_is_list_moving(
                            &this.
                                tList.
                                    use_as____simple_list_t.
                                        use_as____arm_2d_list_core_t)) {
                            break;
                        }

                        /* print label */
                        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
                        arm_lcd_text_set_draw_region(&__item_region);
                        arm_lcd_text_set_display_mode(ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR);
                        
                        arm_lcd_text_reset_display_region_tracking();
                        arm_lcd_printf_label(
                            ARM_2D_ALIGN_CENTRE, "%s", 
                            c_pIconNameTable[
                                icon_list_get_selected_item_id(&this.tList)]);
                    

                        /* update dirty region */
                        arm_2d_helper_dirty_region_update_item( 
                                        &this.tDirtyRegionItems[0],
                                        (arm_2d_tile_t *)ptTile,
                                        &__item_region,
                                        arm_lcd_text_get_last_display_region());

                    } while(0);
                }

                __item_line_dock_vertical(0, 0, 4, 0) {
                    
                    while(arm_fsm_rt_cpl != icon_list_show( &this.tList, 
                                                            ptTile, 
                                                            &__item_region, 
                                                            bIsNewFrame));
                    
                    arm_2d_align_centre(__item_region, 32, 32) {
                        arm_2d_filter_reverse_colour(ptTile, &__centre_region);
                    }
                }


            }
        }

    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

static 
arm_fsm_rt_t __arm_2d_icon_list_draw_list_core_item( 
                                      arm_2d_list_item_t *ptItem,
                                      const arm_2d_tile_t *ptTile,
                                      bool bIsNewFrame,
                                      arm_2d_list_item_param_t *ptParam)
{
    icon_list_t *ptThis = (icon_list_t *)ptItem->ptListView;

    ARM_2D_UNUSED(ptItem);
    ARM_2D_UNUSED(bIsNewFrame);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(ptParam);

    arm_2d_canvas(ptTile, __item_canvas) {

        do {
            const arm_2d_tile_t *ptileIcon = icon_list_get_item_icon(ptThis, ptItem->hwID);
            if (NULL == ptTile) {
                break;
            } else if ( ptTile->tInfo.bHasEnforcedColour 
                     && ptTile->tInfo.tColourInfo.chScheme != ARM_2D_COLOUR_1BIT) {
                /* we only draw a4 mask by default */
                break;
            }

            arm_2d_align_centre(__item_canvas, ptileIcon->tRegion.tSize) {

                arm_2d_fill_colour_with_a1_mask(ptTile, 
                                            &__centre_region, 
                                            ptileIcon, 
                                            (__arm_2d_color_t){GLCD_COLOR_WHITE});
                
            }

        } while(0);
    }
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_mono_icon_menu_t *__arm_2d_scene_mono_icon_menu_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_mono_icon_menu_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_mono_icon_menu_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_mono_icon_menu_t),
                                                        __alignof__(user_scene_mono_icon_menu_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_mono_icon_menu_t));

    *ptThis = (user_scene_mono_icon_menu_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_mono_icon_menu_load,
            .fnScene        = &__pfb_draw_scene_mono_icon_menu_handler,
            //.fnAfterSwitch  = &__after_scene_mono_icon_menu_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_mono_icon_menu_background_start,
            //.fnOnBGComplete = &__on_scene_mono_icon_menu_background_complete,
            .fnOnFrameStart = &__on_scene_mono_icon_menu_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_mono_icon_menu_switching_out,
            .fnOnFrameCPL   = &__on_scene_mono_icon_menu_frame_complete,
            .fnDepose       = &__on_scene_mono_icon_menu_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_mono_icon_menu_t begin ---------------*/

#if 1
    /* initialize icon list */
    do {

        icon_list_cfg_t tCFG = {
            .Icon = {
                .pptileMasks = c_pIconTable,
                .hwCount = dimof(c_pIconTable),
            },

            .use_as____simple_list_cfg_t = {
                .hwCount = dimof(c_pIconTable),
                
                .tFontColour = GLCD_COLOR_WHITE,
                .tBackgroundColour = GLCD_COLOR_BLACK,
                //.bIgnoreBackground = true,

                .bUseMonochromeMode = true,
                .bShowScrollingBar = true,
                .chScrollingBarAutoDisappearTimeX100Ms = 10,
                .ScrollingBar.tColour = GLCD_COLOR_WHITE,
                .bPlaceScrollingBarOnTopOrLeft = true,
                
                //.bDisableRingMode = true,     /* you can disable the list ring mode here */

                //.chNextPadding = 1,
                //.chPreviousPadding = 1,
                .tListSize = {
                    .iHeight = 0,           /* automatically set the height */
                    .iWidth = 0,            /* automatically set the width */
                },
                .tItemSize = {
                    .iHeight = 32,
                    .iWidth = 32,
                },
                .tTextAlignment = ARM_2D_ALIGN_MIDDLE_LEFT,
                .fnOnDrawListItem = &__arm_2d_icon_list_draw_list_core_item,

                .bUseDirtyRegion = true,
                .ptTargetScene = &this.use_as__arm_2d_scene_t,
            }
        };
        icon_list_init(&this.tList, &tCFG);

        icon_list_move_selection(&this.tList, 0, 0);
    } while(0);
#endif
    /* ------------   initialize members of user_scene_mono_icon_menu_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


