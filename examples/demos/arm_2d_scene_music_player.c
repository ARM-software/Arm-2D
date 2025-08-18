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

#define __USER_SCENE_MUSIC_PLAYER_IMPLEMENT__
#include "arm_2d_scene_music_player.h"

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
#   define c_tileAlbumCover         c_tileAlbumCoverGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileAlbumCover         c_tileAlbumCoverRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCN888
#   define c_tileAlbumCover         c_tileAlbumCoverCCCN888

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

extern const arm_2d_tile_t c_tileAlbumCover;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_LiberationSansRegular14_A1,
  ARM_2D_FONT_Arial14_A2,
  ARM_2D_FONT_LiberationSansRegular14_A4,
  ARM_2D_FONT_LiberationSansRegular14_A8;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
} ARM_2D_FONT_ALARM_CLOCK_32_A4;

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

const static char c_chLyrics[] = {
    "Never Gonna Give You Up\n"
    "Song by Rick Astley 1987\n"
    "\n\n\n\n\n\n\n\n"
    "We're no strangers to love\n\n"
    "You know the rules and so do I\n\n"
    "A full commitment's what I'm thinkin' of\n\n"
    "You wouldn't get this from any other guy\n\n"
    "I just wanna tell you how I'm feeling\n\n"
    "Gotta make you understand\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "We've known each other for so long\n\n"
    "Your heart's been aching, but you're too shy to say it\n\n"
    "Inside, we both know what's been going on\n\n"
    "We know the game and we're gonna play it\n\n"
    "And if you ask me how I'm feeling\n\n"
    "Don't tell me you're too blind to see\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "\n\n\n\n"
    "(Ooh, give you up)\n\n"
    "(Ooh, give you up)\n\n"
    "never gonna give, never gonna give, (give you up)\n\n"
    "never gonna give, never gonna give, (give you up)\n\n"
    "\n\n"
    "We've known each other for so long\n\n"
    "Your heart's been aching, but you're too shy to say it\n\n"
    "Inside, we both know what's been going on\n\n"
    "We know the game and we're gonna play it\n\n"
    "I just wanna tell you how I'm feeling\n\n"
    "Gotta make you understand\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "Never gonna give you up, never gonna let you down\n\n"
    "Never gonna run around and desert you\n\n"
    "Never gonna make you cry, never gonna say goodbye\n\n"
    "Never gonna tell a lie and hurt you\n\n"
    "\n\n"
    "Songwriters: Peter Alan Waterman / Matthew James Aitken / Michael Stock\n"
    "Never Gonna Give You Up lyrics(C) Sids Songs Ltd., Mike Stock Publishing Limited, Pw Ballads\n"
};

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_music_player_load(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                         this.tDirtyRegionItems,
                                         dimof(this.tDirtyRegionItems));

    spin_zoom_widget_on_load(&this.AlbumCover.tWidget);
    text_box_on_load(&this.Lyrics.tTextBox);
    
}

static void __after_scene_music_player_switching(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_music_player_depose(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /*--------------------- insert your depose code begin --------------------*/
    spin_zoom_widget_depose(&this.AlbumCover.tWidget);
    histogram_depose(&this.Histogram.tWidget);
    text_box_depose(&this.Lyrics.tTextBox);

    arm_2d_helper_dirty_region_remove_items(&this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                         this.tDirtyRegionItems,
                                         dimof(this.tDirtyRegionItems));
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
 * Scene music_player                                                         *
 *----------------------------------------------------------------------------*/
#if 0  /* deprecated */
static void __on_scene_music_player_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_music_player_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}
#endif

/* we generate fake FFT histogram data here */
static void __fill_histogram_frame(__histogram_frame_t *ptFrame)
{
    assert(NULL != ptFrame);

    srand(arm_2d_helper_get_system_timestamp());

    int_fast16_t n = dimof((*ptFrame));
    uint8_t *pchBin = *ptFrame;
    do {
        *pchBin++ = rand() & (64 - 1);
    } while(--n);

    float fDelta = ARM_2D_ANGLE(360.0f / (dimof((*ptFrame)) * 2));
    
    n = dimof((*ptFrame));
    pchBin = *ptFrame;
    float fAngle = ARM_2D_ANGLE(90.0f);
    do {
        float fRatio = arm_sin_f32(fAngle);
        fAngle += fDelta;

        fRatio = (fRatio * 32.0f + 32.0f);
        uint32_t hwValue = (uint32_t)((float)*pchBin * fRatio) >> 6;
        if (hwValue > 64) {
            hwValue = 64;
        }

        hwValue += fRatio;

        *pchBin++ = hwValue;
    } while(--n);

}

static void __on_scene_music_player_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;

    do {
        int32_t nResult;
        if (arm_2d_helper_time_liner_slider(0, 3599, 30000, &nResult, &this.lTimestamp[0])) {
            this.lTimestamp[0] = 0;
        }

        spin_zoom_widget_on_frame_start(&this.AlbumCover.tWidget, 
                                        nResult, 
                                        this.AlbumCover.fScaling);

    } while(0);

    if (arm_2d_helper_time_liner_slider_i64(-this.Lyrics.tSize.iHeight, 
                                            this.Lyrics.lFullHeight, 
                                            this.nMusicTimeInMs,
                                            &this.Lyrics.lPosition,
                                            &this.lTimestamp[1])) {

        this.lTimestamp[1] = 0;
        text_box_set_scrolling_position(&this.Lyrics.tTextBox, -this.Lyrics.tSize.iHeight);
        this.iPlayProgress = 1000;
    } else {
        text_box_set_scrolling_position(&this.Lyrics.tTextBox,
                                         this.Lyrics.lPosition);

        int32_t nElapsedMs = arm_2d_helper_convert_ticks_to_ms(
                arm_2d_helper_get_system_timestamp() - this.lTimestamp[1]
            );

        if ((this.nMusicTimeInMs - nElapsedMs) > (255 << 3)) {
            this.Lyrics.chOpacity = 255;
        } else {
            this.Lyrics.chOpacity = (this.nMusicTimeInMs - nElapsedMs) >> 3;
        }

        int32_t iSecond = (nElapsedMs / 1000) % 60;

        if (iSecond != this.u6Secends) {
            this.u6Secends = iSecond;
            this.u6Mins = nElapsedMs / 60000;

            arm_2d_helper_dirty_region_item_suspend_update(
                &this.tDirtyRegionItems[DIRTY_REGION_ITEM_PLAY_TIME],
                false
            );
        } else {
            arm_2d_helper_dirty_region_item_suspend_update(
                &this.tDirtyRegionItems[DIRTY_REGION_ITEM_PLAY_TIME],
                true
            );
        }

        this.iPlayProgress = ((int64_t)nElapsedMs * 1000) / (int64_t)this.nMusicTimeInMs;
    }

    if (arm_2d_helper_is_time_out(33, &this.lTimestamp[2])) {
        static __histogram_frame_t s_tDemoFrame;

        __fill_histogram_frame(&s_tDemoFrame);

        this.Histogram.ptFrame = &s_tDemoFrame;
    }

    histogram_on_frame_start(&this.Histogram.tWidget);
    text_box_on_frame_start(&this.Lyrics.tTextBox);
}

static void __on_scene_music_player_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    spin_zoom_widget_on_frame_complete(&this.AlbumCover.tWidget);
    text_box_on_frame_complete(&this.Lyrics.tTextBox);
}

static void __before_scene_music_player_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_music_player_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
        int16_t iLength = this.AlbumCover.iRadius + this.AlbumCover.iPivotOffset;
        arm_2d_align_top_right(__top_canvas, iLength, iLength) {

            arm_2d_point_float_t tPivot = {
                .fX = (float)__top_right_region.tLocation.iX 
                    + (float)this.AlbumCover.iRadius 
                    + 1.0f,
                .fY = (float)__top_right_region.tLocation.iY
                    + this.AlbumCover.iPivotOffset,
            };                            

            spin_zoom_widget_show(  &this.AlbumCover.tWidget, 
                                    ptTile, 
                                    &__top_right_region, 
                                    &tPivot, 
                                    255);

        }

        /* draw histogram at the bottom */
        arm_2d_dock_bottom( __top_canvas, 
                            histogram_get_size(&this.Histogram.tWidget).iHeight + 20) {

            arm_2d_layout(__bottom_region) {

                __item_line_dock_vertical(4) {
                    progress_bar_flowing_show(  ptTile, 
                                                &__item_region, 
                                                this.iPlayProgress, 
                                                bIsNewFrame,
                                                GLCD_COLOR_GRAY(128),
                                                GLCD_COLOR_WHITE,
                                                GLCD_COLOR_BLACK);
                    
                    /* update dirty region */
                    arm_2d_helper_dirty_region_update_item( 
                        &this.use_as__arm_2d_scene_t.tDirtyRegionHelper.tDefaultItem,
                        (arm_2d_tile_t *)ptTile,
                        &__top_canvas,
                        &__item_region);
                }

                __item_line_dock_vertical() {

                    arm_2d_dock_with_margin(__item_region, 10, 10, 10, 10) {
                    
                        arm_2d_dock_vertical(
                            __dock_region, 
                            histogram_get_size(&this.Histogram.tWidget).iHeight) {
                            histogram_show( &this.Histogram.tWidget,
                                                            ptTile,
                                                            &__vertical_region,
                                                            128);
                        }


                        arm_lcd_text_set_scale(0.7f);
                        arm_2d_size_t tPlayTimeSize 
                            = arm_lcd_printf_to_buffer( 
                                (const arm_2d_font_t *)&ARM_2D_FONT_ALARM_CLOCK_32_A4,
                                "%02d:%02d",
                                this.u6Mins,
                                this.u6Secends);
                        tPlayTimeSize.iWidth += 2;  /* minor fix */
                        
                        arm_2d_align_bottom_right(__dock_region, tPlayTimeSize) {

                            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                            arm_lcd_text_set_draw_region(&__bottom_right_region);
                            arm_lcd_text_set_colour(__RGB(0xB6, 0xC7, 0xE7), GLCD_COLOR_BLACK);
                            arm_lcd_printf_buffer(0);

                            /* update dirty region */
                            arm_2d_helper_dirty_region_update_item( 
                                &this.tDirtyRegionItems[DIRTY_REGION_ITEM_PLAY_TIME],
                                (arm_2d_tile_t *)ptTile,
                                &__dock_region,
                                &__bottom_right_region);
                        }
                        arm_lcd_text_set_scale(1.0f);

                    }

                    draw_glass_bar(ptTile, &__item_region, 64, true);
                }
            }
        }


        arm_2d_size_t tLyricsBoxSize = {
            .iWidth = __top_canvas.tSize.iWidth - this.AlbumCover.iRadius,
            .iHeight = __top_canvas.tSize.iHeight - histogram_get_size(&this.Histogram.tWidget).iHeight - 20,
        };

        if (__top_canvas.tSize.iWidth <= 240) {
            tLyricsBoxSize.iWidth = __top_canvas.tSize.iWidth - 10;
        }

        if (__top_canvas.tSize.iHeight < 128) {
            tLyricsBoxSize.iHeight = __top_canvas.tSize.iHeight - 20;
        }

        arm_2d_align_top_left(__top_canvas, tLyricsBoxSize) {
            arm_2d_dock_with_margin(__top_left_region, 10, 10, 20, 10) {

                arm_lcd_text_set_char_spacing(1);
                arm_lcd_text_set_line_spacing(4);

                text_box_show(  &this.Lyrics.tTextBox, 
                                ptTile, 
                                &__dock_region,
                                (__arm_2d_color_t) {__RGB(0xB6, 0xC7, 0xE7)},
                                this.Lyrics.chOpacity,
                                bIsNewFrame);

            }
        }

        /* draw text at the top-left corner */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene music_player");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

static
int32_t histogram_get_bin_value(void *pTarget, 
                                histogram_t *ptHistogram, 
                                uint_fast16_t hwBinIndex)
{
    ARM_2D_UNUSED(ptHistogram);
    user_scene_music_player_t *ptThis = (user_scene_music_player_t *)pTarget;

    if (NULL == this.Histogram.ptFrame) {
        return 0;
    }

    return (int32_t)(*this.Histogram.ptFrame)[hwBinIndex];
}


ARM_NONNULL(1)
user_scene_music_player_t *__arm_2d_scene_music_player_init(   
                                    arm_2d_scene_player_t *ptDispAdapter, 
                                    user_scene_music_player_t *ptThis)
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
        ptThis = (user_scene_music_player_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_music_player_t),
                                                        __alignof__(user_scene_music_player_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_music_player_t));

    *ptThis = (user_scene_music_player_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_music_player_load,
            .fnScene        = &__pfb_draw_scene_music_player_handler,
            .fnAfterSwitch  = &__after_scene_music_player_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            
            //.fnOnBGStart    = &__on_scene_music_player_background_start,        /* deprecated */
            //.fnOnBGComplete = &__on_scene_music_player_background_complete,     /* deprecated */
            .fnOnFrameStart = &__on_scene_music_player_frame_start,
            .fnBeforeSwitchOut = &__before_scene_music_player_switching_out,
            .fnOnFrameCPL   = &__on_scene_music_player_frame_complete,
            .fnDepose       = &__on_scene_music_player_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    if (__top_canvas.tSize.iWidth < 320) {
        /* disable dynamic dirty region for small screen */
        this.use_as__arm_2d_scene_t.bUseDirtyRegionHelper = false;
    }

    /* ------------   initialize members of user_scene_music_player_t begin ---------------*/
    // initialize album cover
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
                    .fAngle = 0.1f,
                },
            },
            .ptTransformMode = &SPIN_ZOOM_MODE_TILE_WITH_COLOUR_KEYING,

            .bUseFloatPointInCentre = true,
            .Source = {
                .ptSource = &c_tileAlbumCover,
                .tCentreFloat = (arm_2d_point_float_t){
                    .fX = c_tileAlbumCover.tRegion.tSize.iWidth / 2.0f,
                    .fY = c_tileAlbumCover.tRegion.tSize.iHeight / 2.0f,
                },
                .tColourToFill = GLCD_COLOR_BLACK,
            },
            
            .ptScene = (arm_2d_scene_t *)ptThis,
        };
        spin_zoom_widget_init(&this.AlbumCover.tWidget, &tCFG);

        int16_t iAlbumCoverWidth = c_tileAlbumCover.tRegion.tSize.iWidth - 86;
        if (__top_canvas.tSize.iWidth > 240 && __top_canvas.tSize.iWidth <= 320) {
            this.AlbumCover.fScaling = 1.0f;
            
        } else {
            this.AlbumCover.fScaling = (float)__top_canvas.tSize.iWidth / (float)iAlbumCoverWidth;
            this.AlbumCover.fScaling = MAX(0.5f, this.AlbumCover.fScaling);
            this.AlbumCover.fScaling = MIN(1.5f, this.AlbumCover.fScaling);

            iAlbumCoverWidth *= this.AlbumCover.fScaling;
        }

        this.AlbumCover.iRadius = iAlbumCoverWidth >> 1;
        if (__top_canvas.tSize.iWidth <= 240) {
            this.AlbumCover.iPivotOffset = 0;
        } else if (__top_canvas.tSize.iWidth <= 320) {
            this.AlbumCover.iPivotOffset = this.AlbumCover.iRadius >> 2;
        } else {
            this.AlbumCover.iPivotOffset = this.AlbumCover.iRadius >> 1;
        }
    } while(0);

    do {
        histogram_cfg_t tCFG = {
            .Bin = {
                .tSize = {(__top_canvas.tSize.iWidth - 20) >> 6, __top_canvas.tSize.iHeight >> 3},
                .chPadding = 1,
                .bUseScanLine = true,
                .nMaxValue = 128,

                .ptItems = this.Histogram.tBins,
                .hwCount = dimof(this.Histogram.tBins),
            },

            .Colour = {
                .wFrom =  __RGB32(0xB6, 0xC7, 0xE7),
                .wTo =    __RGB32(0xB6, 0xC7, 0xE7),
            },

            .ptParent = &this.use_as__arm_2d_scene_t,

            .evtOnGetBinValue = {
                .fnHandler = &histogram_get_bin_value,
                .pTarget = ptThis,
            },
        };

        if (__top_canvas.tSize.iWidth < 320) {
            /* disable dynamic dirty region when screen is too small */
            tCFG.ptParent = NULL;
        }

        if (tCFG.Bin.tSize.iWidth == 0) {
            tCFG.Bin.chPadding = 0;
            if (__top_canvas.tSize.iWidth < (128 + 20)) {
                __top_canvas.tSize.iWidth = 1;
            }
        } else if (tCFG.Bin.tSize.iWidth == 1) {
            tCFG.Bin.chPadding = 0;
        } else {
            tCFG.Bin.tSize.iWidth -= 1;
            tCFG.Bin.chPadding = 1;
        }

        if (tCFG.Bin.tSize.iHeight < 32) {
            tCFG.Bin.tSize.iHeight = 32;
            tCFG.Bin.bUseScanLine = false;
        }

        histogram_init(&this.Histogram.tWidget, &tCFG);
    } while(0);

    

    /* initialize Lyrics Box */
    do {

        /*
         * NOTE: If you want to change music, please update:
         *       1. this.nMusicTimeInMs  
         *       2. this.Lyrics.tStringReader with the new lyrics
         *       
         *       Please do NOT modify other part unless you know 
         *       what you are doing.
         */
        this.nMusicTimeInMs = (60 * 3 + 33) * 1000ul;
        text_box_c_str_reader_init( &this.Lyrics.tStringReader,
                                    c_chLyrics,
                                    sizeof(c_chLyrics));

        text_box_cfg_t tCFG = {
            .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_LiberationSansRegular14_A8,
            .tStreamIO = {
                .ptIO       = &TEXT_BOX_IO_C_STRING_READER,
                .pTarget    = (uintptr_t)&this.Lyrics.tStringReader,
            },
            .u2LineAlign = TEXT_BOX_LINE_ALIGN_CENTRE,
            //.fScale = 1.2f,
            //.chSpaceBetweenParagraph = 20,

            .ptScene = (arm_2d_scene_t *)ptThis,
            .bUseDirtyRegions = true,
        };

        if (__top_canvas.tSize.iWidth < 320) {
            tCFG.ptFont = NULL;
        }

        arm_lcd_text_set_char_spacing(1);
        arm_lcd_text_set_line_spacing(4);

        text_box_init(&this.Lyrics.tTextBox, &tCFG);

        do {
            arm_2d_size_t tLyricsBoxSize = {
                .iWidth = __top_canvas.tSize.iWidth - this.AlbumCover.iRadius,
                .iHeight = __top_canvas.tSize.iHeight - histogram_get_size(&this.Histogram.tWidget).iHeight - 20,
            };

            if (__top_canvas.tSize.iWidth <= 240) {
                tLyricsBoxSize.iWidth = __top_canvas.tSize.iWidth - 10;
            }

            if (__top_canvas.tSize.iHeight < 128) {
                tLyricsBoxSize.iHeight = __top_canvas.tSize.iHeight - 20;
            }

            arm_2d_align_top_left(__top_canvas, tLyricsBoxSize) {
                arm_2d_dock_with_margin(__top_left_region, 10, 10, 20, 10) {

                    this.Lyrics.tSize = __dock_region.tSize;
                }
            }
        } while(0);

        this.Lyrics.lFullHeight = text_box_get_line_count(  &this.Lyrics.tTextBox, 
                                                            this.Lyrics.tSize.iWidth);
        if (this.Lyrics.lFullHeight > 0) {
            this.Lyrics.lFullHeight *= (int64_t)text_box_get_line_height(&this.Lyrics.tTextBox);
            this.Lyrics.lFullHeight -= (this.Lyrics.tSize.iHeight >> 1);
        }

        /* set initial location */
        text_box_set_scrolling_position(&this.Lyrics.tTextBox, -this.Lyrics.tSize.iHeight);
        this.Lyrics.chOpacity = 0;

    } while(0);

    


    /* ------------   initialize members of user_scene_music_player_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


