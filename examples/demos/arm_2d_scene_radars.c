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

    arm_foreach(__radar_bogey_t, this.tBogeys, ptBogey) {
        /* initialize transform helper */

        arm_2d_helper_dirty_region_add_items(
                                &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                &ptBogey->tDirtyRegionItem,
                                1);
    }

    foldable_panel_on_load(&this.tScreen);

    this.chRadarIndex = RADAR_IDX_SCAN_SECTOR_STYLE;
    spin_zoom_widget_update_transform_mode(
                                &this.tScanSector, 
                                &SPIN_ZOOM_MODE_FILL_COLOUR );

    foldable_panel_unfold(&this.tScreen);
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
    ARM_PT_DELAY_MS(30000, &this.lTimestamp[1]);

    foldable_panel_fold(&this.tScreen);
    ARM_PT_DELAY_MS(1000, &this.lTimestamp[1]);

ARM_PT_END();

    return arm_fsm_rt_cpl;
}

static void __on_scene_radars_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_radars_t *ptThis = (user_scene_radars_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    int32_t nResult; 
    bool bIsNewScan = false;
    if (arm_2d_helper_time_liner_slider(0, 3600, 20000ul, &nResult, &this.lTimestamp[0])) {
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
    
        if (!ptBogey->bIsLocationUpdated) {

            if (nResult >= ptBogey->iAngle) {
                ptBogey->bAllowUpdateLocation = true;
                ptBogey->bIsLocationUpdated = true;
            }
        }

    }

    //__scene_radars_actions(ptScene);

    spin_zoom_widget_on_frame_start(&this.tScanSector, nResult, 1.0f);
    foldable_panel_on_frame_start(&this.tScreen);

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

        arm_2d_align_centre(__top_canvas, c_tileRadarBackgroundGRAY8.tRegion.tSize ) {
            
            arm_2d_fill_colour_with_mask_and_opacity(
                    ptTile, 
                    &__centre_region,
                    &c_tileRadarBackgroundGRAY8,
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

            arm_2d_region_t tReferenceRegion;

        #if 0
            arm_2d_calculate_reference_target_region_after_transform(
                                                        (arm_2d_op_trans_t *)&this.tScanSector, 
                                                        &tReferenceRegion,
                                                        s_tReferencePoints,
                                                        dimof(s_tReferencePoints)
                                                        );
            
            arm_2d_helper_draw_box(ptTile, &tReferenceRegion, 1, GLCD_COLOR_BLUE, 255);
        #endif

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

        arm_2d_align_centre(__top_canvas, c_tileRadarBackgroundGRAY8.tRegion.tSize ) {

            arm_2d_fill_colour_with_mask_and_opacity(
                    ptTile, 
                    &__centre_region,
                    &c_tileRadarBackgroundGRAY8,
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
                            c_tileRadarBackgroundGRAY8.tRegion.tSize) {
            arm_2d_tile_t *ptPanel = foldable_panel_show(
                                        &this.tScreen,
                                        ptTile, 
                                        &__centre_region,
                                        bIsNewFrame);

            assert(NULL != ptPanel);

            if (RADAR_IDX_SCAN_SECTOR_STYLE == this.chRadarIndex) {
                __draw_simple_radar(pTarget, ptPanel, bIsNewFrame);
            } else if (RADAR_IDX_TORCH_LIGHT_STYLE == this.chRadarIndex) {
                __draw_radar_with_mono_scan_sector_pattern(pTarget, ptPanel, bIsNewFrame);
            }

        }

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

    user_scene_radars_t *ptThis = (user_scene_radars_t *)pObj;
    __radar_bogey_t *ptBogey = (__radar_bogey_t *) ptDN->ptCurrent;

    const arm_2d_tile_t *ptIcon = NULL;

    if (ptBogey->bAllowUpdateLocation) {
        ptBogey->bAllowUpdateLocation = false;

        switch (ptBogey->u2State) {
            case BOGEY_UNKNOW:
                ptBogey->u2NextState = BOGEY_SCANNING;
                ptBogey->iAngle = ptBogey->iNewAngle;
                break;
            case BOGEY_SCANNING:
                ptBogey->u2NextState = BOGEY_TRACKING;
                ptBogey->iDistance = iDistance;
                ptBogey->tDetectedPos = tLocation;
                ptBogey->chOpacity = 255;
                return;
            case BOGEY_TRACKING:
                if (iDistance <= LAST_STAND_DEFENCE_RADIUS || ptBogey->iDistance < iDistance) {
                    ptBogey->u2NextState = BOGEY_LOST;
                } else {
                    ptBogey->tDetectedPos = tLocation;
                    ptBogey->iDistance = iDistance;
                }
                ptBogey->chOpacity = 255;
                break;
            case BOGEY_LOST:
                if (ptBogey->iDistance < iDistance) {
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
                .ptMask = &c_tileRadarBackgroundGRAY8,
            },
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.tScanSector, &tCFG);
    } while(0);

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
        int16_t iRadius = c_tileRadarBackgroundGRAY8.tRegion.tSize.iWidth >> 1;
        dynamic_nebula_cfg_t tCFG = {
            .fSpeed = 0.01f,
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
            ptBogey->iAngle = ptBogey->iNewAngle;
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


