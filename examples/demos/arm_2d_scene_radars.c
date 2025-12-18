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

#define __USER_SCENE_RADARS_IMPLEMENT__
#include "arm_2d_scene_radars.h"

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

#define LAST_STAND_DEFENCE_RADIUS   40

#define RADAR_BACKGROUND     c_tileRadarBackgroundGRAY8

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION  
#   define FILM_TOP_LEFT        this.tFilm[FILM_IDX_TOP_LEFT].tHelper.use_as__arm_2d_tile_t
#   define FILM_BOTTOM_RIGHT    this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper.use_as__arm_2d_tile_t
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
enum {
    BOGEY_UNKNOW,
    BOGEY_SCANNING,
    BOGEY_TRACKING,
    BOGEY_LOST,
};

enum {
    RADAR_IDX_SCAN_SECTOR_STYLE,
    RADAR_IDX_TORCH_LIGHT_STYLE,
    RADAR_IDX_ANIMATION,
};


/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

//extern const arm_2d_tile_t c_tileSatelliteMapSmallGRAY8;
extern const arm_2d_tile_t c_tileScanSectorMask;
extern const arm_2d_tile_t c_tileTinyDotMask;
extern const arm_2d_tile_t c_tileTinyCrossMask;
extern const arm_2d_tile_t c_tileRadarBackgroundGRAY8;

extern const arm_2d_tile_t c_tileFilmMaskMask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A4;

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

ARM_NOINIT
static arm_2d_location_t s_tScanSectorCenter;

ARM_NOINIT
static
arm_2d_location_t s_tReferencePoints[4];

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_radars_load(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_load(&this.tScanSector);

    /* 
     * NOTE: this assignment only works after calling spin_zoom_widget_on_load()
     */
    this.tScanSector.tHelper.SourceReference.ptPoints = s_tReferencePoints;
    this.tScanSector.tHelper.SourceReference.chCount = dimof(s_tReferencePoints);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    arm_foreach(this.tFilm) {
        spin_zoom_widget_on_load(&_->tSector);

        _->tSector.tHelper.SourceReference.ptPoints = s_tReferencePoints;
        _->tSector.tHelper.SourceReference.chCount = dimof(s_tReferencePoints);
    }
#endif

    arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {
        /* initialize transform helper */

        arm_2d_helper_dirty_region_add_items(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &ptBogey->tDirtyRegionItem,
                                1);
    }

    foldable_panel_on_load(&this.tScreen);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    arm_foreach(this.tQOI) {
        arm_qoi_loader_on_load(&_->tLoader);
    }
#endif

#if 0// for debug

    foldable_panel_unfold(&this.tScreen);

    this.chRadarIndex = RADAR_IDX_ANIMATION;

    spin_zoom_widget_update_transform_mode(
                                &this.tScanSector, 
                                &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK );
#endif
}

static void __after_scene_radars_switching(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_radars_depose(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    
    spin_zoom_widget_depose(&this.tScanSector);

    arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {
        arm_2d_helper_dirty_region_remove_items(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &ptBogey->tDirtyRegionItem,
                                1);
    }

    foldable_panel_depose(&this.tScreen);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    arm_foreach(this.tQOI) {
        arm_qoi_loader_depose(&_->tLoader);
    }

    arm_foreach(this.tFilm) {
        spin_zoom_widget_depose(&_->tSector);
    }
#endif
    /*---------------------- insert your depose code end  --------------------*/

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene radars                                                                    *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_radars_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_radars_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

static arm_fsm_rt_t __scene_radars_actions(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

ARM_PT_BEGIN(this.chPT)

    /* wait for 1s at the begining */
    ARM_PT_DELAY_MS(1000, &this.lTimestamp[1]);
    this.chRadarIndex = RADAR_IDX_SCAN_SECTOR_STYLE;
    spin_zoom_widget_update_transform_mode(
                                &this.tScanSector, 
                                &SPIN_ZOOM_MODE_FILL_COLOUR );

    foldable_panel_unfold(&this.tScreen);
    ARM_PT_DELAY_MS(10000, &this.lTimestamp[1]);

    foldable_panel_fold(&this.tScreen);

    /* wait for 1s before next round */
    ARM_PT_DELAY_MS(2000, &this.lTimestamp[1]);
    this.chRadarIndex = RADAR_IDX_TORCH_LIGHT_STYLE;

    spin_zoom_widget_update_transform_mode(
                                &this.tScanSector, 
                                &SPIN_ZOOM_MODE_FILL_COLOUR_WITH_TARGET_MASK );

    foldable_panel_unfold(&this.tScreen);
    ARM_PT_DELAY_MS(20000, &this.lTimestamp[1]);

    foldable_panel_fold(&this.tScreen);
    ARM_PT_DELAY_MS(1000, &this.lTimestamp[1]);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    ARM_PT_DELAY_MS(1000, &this.lTimestamp[1]);
    this.chRadarIndex = RADAR_IDX_ANIMATION;
    
    foldable_panel_unfold(&this.tScreen);
    ARM_PT_DELAY_MS(20000, &this.lTimestamp[1]);

    foldable_panel_fold(&this.tScreen);
    ARM_PT_DELAY_MS(1000, &this.lTimestamp[1]);
#endif

ARM_PT_END();

    return arm_fsm_rt_cpl;
}

static void __on_scene_radars_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int32_t nResult; 
    bool bIsNewScan = false;
    if (arm_2d_helper_time_liner_slider(0, 3600, 10000ul, &nResult, &this.lTimestamp[0])) {
        this.lTimestamp[0] = 0;
        bIsNewScan = true;
        nResult = 0;
    }

    arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {

        if (ptBogey->chOpacity > 64) {
            ptBogey->chOpacity -= 2;
        } else {
            ptBogey->chOpacity = 64;
        }

        if (bIsNewScan) {
            ptBogey->bIsLocationUpdated = false;
        }

        if (nResult >= ptBogey->iAngle && !ptBogey->bIsLocationUpdated) {
            ptBogey->bAllowUpdateLocation = true;
            ptBogey->bIsLocationUpdated = true;
        }
    }

    __scene_radars_actions(ptScene);

    spin_zoom_widget_on_frame_start(&this.tScanSector, nResult, 1.0f);

    foldable_panel_on_frame_start(&this.tScreen);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    arm_foreach(this.tFilm) {
        spin_zoom_widget_on_frame_start(&_->tSector, nResult, 1.5f);
    }
    
    do {
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 
                                        this.tFilm[FILM_IDX_TOP_LEFT].tHelper.hwFrameNum, 
                                        ( this.tFilm[FILM_IDX_TOP_LEFT].tHelper.hwPeriodPerFrame 
                                        * this.tFilm[FILM_IDX_TOP_LEFT].tHelper.hwFrameNum),
                                        &nResult, 
                                        &this.lTimestamp[2])) {
            this.lTimestamp[2] = 0;
            nResult = 0;
        }
        arm_2d_helper_film_set_frame(&this.tFilm[FILM_IDX_TOP_LEFT].tHelper, nResult);

    } while(0);

    do {
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 
                                        this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper.hwFrameNum, 
                                        ( this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper.hwPeriodPerFrame 
                                        * this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper.hwFrameNum),
                                        &nResult, 
                                        &this.lTimestamp[3])) {
            this.lTimestamp[3] = 0;
            nResult = 0;
        }
        arm_2d_helper_film_set_frame(&this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper, nResult);
    } while(0);

    arm_foreach(this.tQOI) {
        arm_qoi_loader_on_frame_start(&_->tLoader);
    }
#endif
}

static void __on_scene_radars_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_frame_complete(&this.tScanSector);

    arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {
        ptBogey->u2State = ptBogey->u2NextState;
    }

    foldable_panel_on_frame_complete(&this.tScreen);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    arm_foreach(this.tQOI) {
        arm_qoi_loader_on_frame_complete(&_->tLoader);
    }

    arm_foreach(this.tFilm) {
        spin_zoom_widget_on_frame_complete(&_->tSector);
    }
#endif


}

static void __before_scene_radars_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__draw_simple_radar)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, RADAR_BACKGROUND.tRegion.tSize ) {
            
            arm_2d_fill_colour_with_mask_and_opacity(
                    ptTile, 
                    &__centre_region,
                    &RADAR_BACKGROUND,
                    (__arm_2d_color_t) {ARM_2D_DEMO_RADAR_COLOUR},
                    128-32);

            /* show bogeys */
            dynamic_nebula_show(&this.tNebula, 
                                ptTile, 
                                &__centre_region, 
                                ARM_2D_DEMO_RADAR_COLOUR, 
                                255,
                                bIsNewFrame);

            spin_zoom_widget_show(  &this.tScanSector, 
                                    ptTile, 
                                    &__centre_region, 
                                    NULL, 
                                    255 - 64);

            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        ARM_2D_DEMO_RADAR_COLOUR, 
                                        (arm_2d_border_opacity_t)
                                            {0, 0, 0, 0},
                                        (arm_2d_corner_opacity_t)
                                            {128, 128, 128, 128});
                
            arm_2d_size_t tLabelSize = arm_lcd_printf_to_buffer(
                (arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4, 
                "Scan Sector Style");

            arm_2d_align_centre(__centre_region, tLabelSize) {

                arm_lcd_text_set_target_framebuffer(ptTile);
                arm_lcd_text_set_draw_region(&__centre_region);
                arm_lcd_text_set_colour(ARM_2D_DEMO_RADAR_COLOUR, GLCD_COLOR_BLACK);

                arm_lcd_printf_buffer(0);

            }      
        }

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}


static
IMPL_PFB_ON_DRAW(__draw_radar_with_mono_scan_sector_pattern)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, RADAR_BACKGROUND.tRegion.tSize ) {

            arm_2d_fill_colour_with_mask_and_opacity(
                    ptTile, 
                    &__centre_region,
                    &RADAR_BACKGROUND,
                    (__arm_2d_color_t) {ARM_2D_DEMO_RADAR_COLOUR},
                    64);

            /* show bogeys */
            dynamic_nebula_show(&this.tNebula, 
                                ptTile, 
                                &__centre_region, 
                                ARM_2D_DEMO_RADAR_COLOUR, 
                                255,
                                bIsNewFrame);
            
            spin_zoom_widget_show(  &this.tScanSector, 
                                    ptTile, 
                                    &__centre_region, 
                                    NULL, 
                                    255);

            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        ARM_2D_DEMO_RADAR_COLOUR, 
                                        (arm_2d_border_opacity_t)
                                            {0, 0, 0, 0},
                                        (arm_2d_corner_opacity_t)
                                            {128, 128, 128, 128});

            arm_2d_size_t tLabelSize = arm_lcd_printf_to_buffer(
                (arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4, 
                "Torch Light Style");

            arm_2d_align_centre(__centre_region, tLabelSize) {

                arm_lcd_text_set_target_framebuffer(ptTile);
                arm_lcd_text_set_draw_region(&__centre_region);
                arm_lcd_text_set_colour(ARM_2D_DEMO_RADAR_COLOUR, GLCD_COLOR_BLACK);

                arm_lcd_printf_buffer(0);

            }

        }

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
static
IMPL_PFB_ON_DRAW(__draw_radar_with_animation)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        /* following code is just a demo, you can remove them */

        arm_2d_align_centre(__top_canvas, RADAR_BACKGROUND.tRegion.tSize ) {


            arm_2d_fill_colour_with_mask_and_opacity(
                    ptTile, 
                    &__centre_region,
                    &RADAR_BACKGROUND,
                    (__arm_2d_color_t) {ARM_2D_DEMO_RADAR_COLOUR},
                    64);

            /* show bogeys */
            dynamic_nebula_show(&this.tNebula, 
                                ptTile, 
                                &__centre_region, 
                                ARM_2D_DEMO_RADAR_COLOUR, 
                                255,
                                bIsNewFrame);

            spin_zoom_widget_show(  &this.tScanSector, 
                                    ptTile, 
                                    &__centre_region, 
                                    NULL, 
                                    255);



            arm_2d_align_top_left(__centre_region, FILM_TOP_LEFT.tRegion.tSize) {

                arm_2d_location_t tPivot = __top_left_region.tLocation;
                tPivot.iX += __top_left_region.tSize.iWidth - 1;
                tPivot.iY += __top_left_region.tSize.iHeight - 1;


                spin_zoom_widget_show(  &this.tFilm[FILM_IDX_TOP_LEFT].tSector, 
                                        ptTile, 
                                        &__top_left_region, 
                                        &tPivot,
                                        255);
            }

            arm_2d_align_bottom_right(__centre_region, FILM_BOTTOM_RIGHT.tRegion.tSize) {

                spin_zoom_widget_show(  &this.tFilm[FILM_IDX_BOTTOM_RIGHT].tSector, 
                                        ptTile, 
                                        &__bottom_right_region, 
                                        &__bottom_right_region.tLocation,
                                        255);
            }

        
            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        ARM_2D_DEMO_RADAR_COLOUR, 
                                        (arm_2d_border_opacity_t)
                                            {0, 0, 0, 0},
                                        (arm_2d_corner_opacity_t)
                                            {128, 128, 128, 128});

            arm_2d_size_t tLabelSize = arm_lcd_printf_to_buffer(
                (arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A4, 
                "Animation");

            arm_2d_align_centre(__centre_region, tLabelSize) {

                arm_lcd_text_set_target_framebuffer(ptTile);
                arm_lcd_text_set_draw_region(&__centre_region);
                arm_lcd_text_set_colour(ARM_2D_DEMO_RADAR_COLOUR, GLCD_COLOR_BLACK);

                arm_lcd_printf_buffer(0);

            }

        }

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}
#endif

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_radars_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        arm_2d_align_centre(__top_canvas, 
                            RADAR_BACKGROUND.tRegion.tSize) {
            arm_2d_tile_t *ptPanel = foldable_panel_show(
                                        &this.tScreen,
                                        ptTile, 
                                        &__centre_region,
                                        bIsNewFrame);

            assert(NULL != ptPanel);

            switch (this.chRadarIndex) {
                case RADAR_IDX_SCAN_SECTOR_STYLE:
                    __draw_simple_radar(pTarget, ptPanel, bIsNewFrame);
                    break;

                default:
                case RADAR_IDX_TORCH_LIGHT_STYLE:
                    __draw_radar_with_mono_scan_sector_pattern(pTarget, ptPanel, bIsNewFrame);
                    break;
            #if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
                case RADAR_IDX_ANIMATION:
                    __draw_radar_with_animation(pTarget, ptPanel, bIsNewFrame);
                    break;
            #endif
            }

        }

    #if 0 /* for debug */
        arm_2d_align_mid_left(__top_canvas, FILM_TOP_LEFT.tRegion.tSize) {
            arm_2d_tile_copy(   &FILM_TOP_LEFT,
                                ptTile,
                                &__mid_left_region);
        }

        arm_2d_align_mid_right(__top_canvas, FILM_BOTTOM_RIGHT.tRegion.tSize) {
            arm_2d_tile_copy(   &FILM_BOTTOM_RIGHT,
                                ptTile,
                                &__mid_right_region);
        }
    #endif

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

static
void __draw_bogey_handler(  void *pObj,
                            dynamic_nebula_t *ptDN,
                            const arm_2d_tile_t *ptTile,
                            arm_2d_location_t tLocation,
                            uint8_t chOpacity,
                            int16_t iDistance)
{
    assert(NULL != pObj);
    assert(NULL != ptDN);

    ARM_2D_UNUSED(chOpacity);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pObj;
    __radar_bogey_t *ptBogey = (__radar_bogey_t *) ptDN->ptCurrent;

    const arm_2d_tile_t *ptIcon = NULL;

    if (ptBogey->bAllowUpdateLocation) {
        ptBogey->bAllowUpdateLocation = false;
        
        switch (ptBogey->u2State) {
            case BOGEY_UNKNOW:
                ptBogey->u2NextState = BOGEY_SCANNING;
                ptBogey->iAngle = ptBogey->iNewAngle;
                
                break ;
            case BOGEY_SCANNING:
                ptBogey->u2NextState = BOGEY_TRACKING;
                ptBogey->iDistance = iDistance;
                ptBogey->tDetectedPos = tLocation;
                ptBogey->chOpacity = 255;
                break;
            case BOGEY_TRACKING:
                if (iDistance <= LAST_STAND_DEFENCE_RADIUS || (ptBogey->iAngle != ptBogey->iNewAngle)) {
                    ptBogey->u2NextState = BOGEY_LOST;
                } else {
                    ptBogey->tDetectedPos = tLocation;
                    ptBogey->iDistance = iDistance;
                }
                ptBogey->chOpacity = 255;
                break;
            case BOGEY_LOST:
                if (ptBogey->iAngle != ptBogey->iNewAngle) {
                    ptBogey->u2NextState = BOGEY_UNKNOW;
                }
                break;
        }
    }

    switch (ptBogey->u2State) {
        default:
        case BOGEY_UNKNOW:
        case BOGEY_SCANNING:
            return ;

        case BOGEY_TRACKING:
            ptIcon = &c_tileTinyDotMask;
            break;

        case BOGEY_LOST:
            ptIcon = &c_tileTinyCrossMask;
            break;
    }

    arm_2d_region_t tDrawRegion = {
        .tLocation = ptBogey->tDetectedPos,
        .tSize = ptIcon->tRegion.tSize,
    };

    tDrawRegion.tLocation.iX -= tDrawRegion.tSize.iWidth >> 1;
    tDrawRegion.tLocation.iY -= tDrawRegion.tSize.iHeight >> 1;

    if (0 != ptBogey->chOpacity) {
        arm_2d_fill_colour_with_mask_and_opacity(   
            ptTile, 
            &tDrawRegion, 
            ptIcon, 
            (__arm_2d_color_t){ ARM_2D_DEMO_RADAR_BOGEY_COLOUR},
            ptBogey->chOpacity);
    }

    arm_2d_helper_dirty_region_update_item( 
        &ptBogey->tDirtyRegionItem,
        ptTile,
        NULL,
        &tDrawRegion);

}

static
void __update_bogey_handler(void *pObj,
                            dynamic_nebula_t *ptDN,
                            dynamic_nebula_particle_t *ptParticle,
                            int_fast16_t iAngle)
{
    assert(NULL != pObj);
    assert(NULL != ptParticle);
    ARM_2D_UNUSED(ptDN);

    assert(iAngle >= 0 && iAngle < 3600);

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pObj;
    __radar_bogey_t *ptBogey = (__radar_bogey_t *)ptParticle;

    ptBogey->iNewAngle = iAngle;
}

ARM_NONNULL(1)
user_scene_radars_t *__arm_2d_scene_radars_init(   
                                        arm_2d_scene_player_t *ptDispAdapter,
                                        user_scene_radars_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    /* initialise dirty region 0 at runtime
     * this demo shows that we create a region in the centre of a screen(320*240)
     * for a image stored in the tile c_tileCMSISLogoMask
     */
    arm_2d_align_centre(__top_canvas, c_tileCMSISLogoMask.tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
    }

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].tRegion.tSize.iWidth 
                                                        = __top_canvas.tSize.iWidth;

    if (NULL == ptThis) {
        ptThis = (user_scene_radars_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_radars_t),
                                                        __alignof__(user_scene_radars_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_radars_t));

    *ptThis = (user_scene_radars_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_radars_load,
            .fnScene        = &__pfb_draw_scene_radars_handler,
            .fnAfterSwitch  = &__after_scene_radars_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_radars_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_radars_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_radars_frame_start,
            .fnBeforeSwitchOut = &__before_scene_radars_switching_out,
            .fnOnFrameCPL   = &__on_scene_radars_frame_complete,
            .fnDepose       = &__on_scene_radars_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_radars_t begin ---------------*/

#if ARM_2D_DEMO_RADAR_USE_QOI


    /* initialize QOI loader */
    do {

    #if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
        do {
        #if ARM_2D_DEMO_QOI_USE_FILE
            arm_qoi_io_file_loader_init(&this.LoaderIO.tFile, "../common/asset/girl_dance.qoi");
        #else
            extern
            const uint8_t c_qoiGirlDance[190659];

            arm_qoi_io_binary_loader_init(  &this.tQOI[QOI_FILM_TOP_LEFT].LoaderIO.tBinary, 
                                            c_qoiGirlDance, 
                                            sizeof(c_qoiGirlDance));
        #endif
            arm_qoi_loader_cfg_t tCFG = {
                //.bUseHeapForVRES = true,
                .ptScene = (arm_2d_scene_t *)ptThis,
                .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

                /* you can only extract specific colour channel and use it as A8 mask */
                //.tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
                //.u2ChannelIndex = ARM_QOI_MASK_CHN_GREEN,   

                //.bInvertColour = true,
                //.bForceDisablePreBlendwithBG = true,
                .tBackgroundColour.wColour = GLCD_COLOR_WHITE,
            #if ARM_2D_DEMO_QOI_USE_FILE
                .ImageIO = {
                    .ptIO = &ARM_QOI_IO_FILE_LOADER,
                    .pTarget = (uintptr_t)&this.LoaderIO.tFile,
                },
            #else
                .ImageIO = {
                    .ptIO = &ARM_QOI_IO_BINARY_LOADER,
                    .pTarget = (uintptr_t)&this.tQOI[QOI_FILM_TOP_LEFT].LoaderIO.tBinary,
                },
            #endif
            };

            arm_qoi_loader_init(&this.tQOI[QOI_FILM_TOP_LEFT].tLoader, &tCFG);
            
            this.tFilm[FILM_IDX_TOP_LEFT].tHelper = (arm_2d_helper_film_t)
                impl_film(  this.tQOI[QOI_FILM_TOP_LEFT].tLoader.vres.tTile, 
                            100, 
                            100, 
                            1, 
                            10, 
                            84);

            /* set to the last frame */
            arm_2d_helper_film_set_frame(&this.tFilm[FILM_IDX_TOP_LEFT].tHelper, -1);
        } while(0);
    
        do {
        #if ARM_2D_DEMO_QOI_USE_FILE
            arm_qoi_io_file_loader_init(&this.LoaderIO.tFile, "../common/asset/girl_dance.qoi");
        #else
            extern
            const uint8_t c_qoiDogeDance[126958];

            arm_qoi_io_binary_loader_init(  &this.tQOI[QOI_FILM_BOTTOM_RIGHT].LoaderIO.tBinary, 
                                            c_qoiDogeDance, 
                                            sizeof(c_qoiDogeDance));
        #endif
            arm_qoi_loader_cfg_t tCFG = {
                //.bUseHeapForVRES = true,
                .ptScene = (arm_2d_scene_t *)ptThis,
                .u2WorkMode = ARM_QOI_MODE_PARTIAL_DECODED,

                /* you can only extract specific colour channel and use it as A8 mask */
                //.tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8,
                //.u2ChannelIndex = ARM_QOI_MASK_CHN_GREEN,   

                //.bInvertColour = true,
                //.bForceDisablePreBlendwithBG = true,
                .tBackgroundColour.wColour = GLCD_COLOR_WHITE,
            #if ARM_2D_DEMO_QOI_USE_FILE
                .ImageIO = {
                    .ptIO = &ARM_QOI_IO_FILE_LOADER,
                    .pTarget = (uintptr_t)&this.LoaderIO.tFile,
                },
            #else
                .ImageIO = {
                    .ptIO = &ARM_QOI_IO_BINARY_LOADER,
                    .pTarget = (uintptr_t)&this.tQOI[QOI_FILM_BOTTOM_RIGHT].LoaderIO.tBinary,
                },
            #endif
            };

            arm_qoi_loader_init(&this.tQOI[QOI_FILM_BOTTOM_RIGHT].tLoader, &tCFG);
            
            this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper = (arm_2d_helper_film_t)
                impl_film(  this.tQOI[QOI_FILM_BOTTOM_RIGHT].tLoader.vres.tTile, 
                            100, 
                            100, 
                            1, 
                            13, 
                            66);

            /* set to the last frame */
            arm_2d_helper_film_set_frame(&this.tFilm[FILM_IDX_BOTTOM_RIGHT].tHelper, -1);
        } while(0);
    #endif
    } while(0);
#endif

    // initialize second pointer
    do {
        s_tScanSectorCenter.iX = 2;
        s_tScanSectorCenter.iY = c_tileScanSectorMask.tRegion.tSize.iHeight - 3;

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
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_FILL_COLOUR,
            .Source = {
                .ptMask = &c_tileScanSectorMask,
                .tCentre = s_tScanSectorCenter,
                .tColourToFill = ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR,
            },
            .Target = {
                .ptMask = &RADAR_BACKGROUND,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tScanSector, &tCFG);
    } while(0);

#if ARM_2D_DEMO_RADAR_SHOW_ANIMATION
    // initialize top-left animation sector
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
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK,
            .Source = {
                .ptMask = &c_tileScanSectorMask,
                .tCentre = s_tScanSectorCenter,
                //.tColourToFill = ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR,
            },
            .Extra = {
                .ptTile = &FILM_TOP_LEFT,
            },

            .Target = {
                .ptMask = &c_tileFilmMaskMask,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tFilm[FILM_IDX_TOP_LEFT].tSector, &tCFG);
    } while(0);

    // initialize bottom right animation sector
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
                .Step = {
                    .fAngle = 0.0f,  //! 0.0f means very smooth, 1.0f looks like mech watches, 6.0f looks like wall clocks
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_EXTRA_TILE_COPY_WITH_TRANSFORMED_MASK_AND_TARGET_MASK,
            .Source = {
                .ptMask = &c_tileScanSectorMask,
                .tCentre = s_tScanSectorCenter,
                //.tColourToFill = ARM_2D_DEMO_RADAR_SCAN_SECTOR_COLOUR,
            },
            .Extra = {
                .ptTile = &FILM_BOTTOM_RIGHT,
            },

            .Target = {
                .ptMask = &c_tileFilmMaskMask,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tFilm[FILM_IDX_BOTTOM_RIGHT].tSector, &tCFG);
    } while(0);
#endif

    /* update reference points*/
    do {
        float fRadius = c_tileScanSectorMask.tRegion.tSize.iWidth - 2;
        s_tReferencePoints[0].iX = 1;
        s_tReferencePoints[0].iY = c_tileScanSectorMask.tRegion.tSize.iHeight - 2;

        s_tReferencePoints[1].iX = s_tReferencePoints[0].iX + (fRadius * arm_cos_f32(ARM_2D_ANGLE(45)) + 0.5f);
        s_tReferencePoints[1].iY = s_tReferencePoints[0].iY - (fRadius * arm_sin_f32(ARM_2D_ANGLE(45)) + 0.5f);

        s_tReferencePoints[2].iX = s_tReferencePoints[0].iX + fRadius;
        s_tReferencePoints[2].iY = s_tReferencePoints[0].iY;

        s_tReferencePoints[3].iX = s_tReferencePoints[0].iX + (fRadius * arm_cos_f32(ARM_2D_ANGLE(22.5f)) + 0.5f);
        s_tReferencePoints[3].iY = s_tReferencePoints[0].iY - (fRadius * arm_sin_f32(ARM_2D_ANGLE(22.5f)) + 0.5f);
    } while(0);

    /* initialize bogeys */
    do {
        int16_t iRadius = RADAR_BACKGROUND.tRegion.tSize.iWidth >> 1;
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 0.02f,
            .iRadius = iRadius,
            .iVisibleRingWidth = iRadius - LAST_STAND_DEFENCE_RADIUS,
            .hwParticleCount = dimof(this.tBogeys),
            .ptParticles = (dynamic_nebula_particle_t *)this.tBogeys,
            .hwParticleTypeSize = sizeof(__radar_bogey_t),
            //.iFullyVisibleRingWidth = iRadius,

            .bMovingOutward = false,
            .u8FadeOutEdgeWidth = 8,

            .evtOnDrawParticles = {
                .fnHandler = &__draw_bogey_handler,
                .pTarget = ptThis,
            },

            .evtOnUpdateParticle = {
                .fnHandler = &__update_bogey_handler,
                .pTarget = ptThis,
            },
        };
        dynamic_nebula_init(&this.tNebula, &tCFG);

        arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {
            ptBogey->u2State = BOGEY_UNKNOW;
            ptBogey->u2NextState = BOGEY_UNKNOW;
        }
    } while(0);

    /* init normal foldable panels */
    do {
        foldable_panel_cfg_t tCFG = {
            .bShowScanLines = true,
            .ptScene = &this.use_as__arm_2d_scene_t,
            .tLineColour.tColour = ARM_2D_DEMO_RADAR_COLOUR,
            
            .bUseDirtyRegions = true,
        };
        foldable_panel_init(&this.tScreen, &tCFG);
    } while(0);

    /* ------------   initialize members of user_scene_radars_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


