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

#define __USER_SCENE_MATRIX_IMPLEMENT__
#include "arm_2d_scene_matrix.h"

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

enum {
    STAGE_FAR,
    STAGE_MID,
    STAGE_NEAR,
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}   ARM_2D_FONT_Arial14_A1,
    ARM_2D_FONT_Arial14_A2,
    ARM_2D_FONT_Arial14_A4,
    ARM_2D_FONT_Arial14_A8;

extern 
const arm_2d_a1_font_t ARM_2D_FONT_6x8;
/*============================ PROTOTYPES ====================================*/



/*============================ LOCAL VARIABLES ===============================*/

const 
struct {
    float fSpeed;             /* letter train moving speed (pixels per ms)*/
    float fHeightRatio;
    enum {
        STAGE_SHOW_LINE,
        STAGE_SHOW_FONT_SMALL,
        STAGE_SHOW_FONT_LARGE,
    } Display;
} c_StageParams[] = {
    [STAGE_FAR] = {
        .fSpeed         = 0.15f,
        .fHeightRatio   = 1.0f,
        .Display        = STAGE_SHOW_LINE,
    },
    [STAGE_MID] = {
        .fSpeed         = 0.25f,
        .fHeightRatio   = 2.0f,
        .Display        = STAGE_SHOW_FONT_SMALL,
    },
    [STAGE_NEAR] = {
        .fSpeed         = 0.4f,
        .fHeightRatio   = 1.0f,
        .Display        = STAGE_SHOW_FONT_LARGE,
    },

};


/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
static void letter_train_relocate(__letter_train_t *ptThis, arm_2d_size_t tScreenSize, int16_t iOffset)
{
    assert(NULL != ptThis);

    this.u8RandomSeed = rand();

    this.tRegion.tLocation.iX = (rand() % (tScreenSize.iWidth - 10)) + 10;
    
    int16_t iHeight = rand() % reinterpret_s16_q16(
                                    mul_n_q16(
                                        reinterpret_q16_f32( c_StageParams[this.u2Stage].fHeightRatio ),
                                        tScreenSize.iHeight));
    iHeight += tScreenSize.iHeight;



    this.tRegion.tLocation.iY = -iHeight;
    //this.tRegion.tLocation.iY += iOffset;

    this.tRegion.tSize.iHeight = iHeight;


    if (this.u2Stage == STAGE_NEAR) {

        arm_2d_size_t tCharSize = ARM_2D_FONT_Arial14_A1
                                        .use_as__arm_2d_user_font_t
                                            .use_as__arm_2d_font_t
                                                .tCharSize;

        this.u16NumberOfChars = iHeight / tCharSize.iHeight;
        this.tRegion.tSize.iWidth = tCharSize.iWidth;

    } else if (this.u2Stage == STAGE_MID) {
        arm_2d_size_t tCharSize = ARM_2D_FONT_6x8
                                    .use_as__arm_2d_font_t.tCharSize;
        this.u16NumberOfChars = iHeight / tCharSize.iHeight;
        this.tRegion.tSize.iWidth = tCharSize.iWidth;
    } else {
        this.tRegion.tSize.iWidth = 3;
    }
    
}




static void __on_scene_matrix_load(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    this.lTimestamp[0] = arm_2d_helper_get_system_timestamp();

}

static void __after_scene_matrix_switching(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_matrix_depose(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if MATRIX_LETTER_TRAIN_USE_BLUR
    ARM_2D_OP_DEPOSE(this.tBlurOP);
#endif

    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene matrix                                                               *
 *----------------------------------------------------------------------------*/

static void __on_scene_matrix_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_matrix_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_matrix_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;

    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area( 
            &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t);

    int64_t lTimestamp = arm_2d_helper_get_system_timestamp();
    int64_t lDelta = lTimestamp - this.lTimestamp[0];
    int32_t nElapsedMs = arm_2d_helper_convert_ticks_to_ms(lDelta);
    this.lTimestamp[0] = lTimestamp;

    bool bUpdateLetters = false;
    if (arm_2d_helper_is_time_out(30, &this.lTimestamp[1])) {
        bUpdateLetters = true;
    }

    /* update random generator */
    srand(lTimestamp);

    /* update trains */
    arm_foreach(__letter_train_t, this.tTrains, ptTrain) {

        /* update location */
        ptTrain->tRegion.tLocation.iY += reinterpret_s16_q16(
                            mul_n_q16(
                                reinterpret_q16_f32(c_StageParams[ptTrain->u2Stage].fSpeed),
                                nElapsedMs));
        
        if (bUpdateLetters) {
            ptTrain->u8RandomSeed = rand();
        }

        /* whether the letter train moves out of the screen */
        if (ptTrain->tRegion.tLocation.iY >= tScreen.tSize.iHeight) {

            /* relocate the letter train */
            letter_train_relocate(  ptTrain, tScreen.tSize, 
                                    ptTrain->tRegion.tLocation.iY - 
                                    tScreen.tSize.iHeight);
        }

    }

}

static void __on_scene_matrix_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if MATRIX_LETTER_TRAIN_USE_BLUR
    arm_2dp_filter_iir_blur_depose(&this.tBlurOP);
#endif
}

static void __before_scene_matrix_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_lcd_text_force_char_use_same_width(false);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_matrix_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_matrix_t *ptThis = (user_scene_matrix_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {

    #if MATRIX_LETTER_TRAIN_USE_BLUR
        /* draw far background */
        arm_foreach(__letter_train_t, &this.tTrains[0], MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT, ptTrain) {

            __arm_2d_hint_optimize_for_pfb__(ptTrain->tRegion) {

                arm_2d_fill_colour_with_vertical_alpha_gradient_and_opacity(
                                            ptTile, 
                                            &ptTrain->tRegion,
                                            (__arm_2d_color_t){GLCD_COLOR_GREEN},
                                            255,
                                            (arm_2d_alpha_samples_2pts_t) {
                                                {0, 255},
                                            });

            }
        }

    
        /* add blur mask */
        arm_2dp_filter_iir_blur(&this.tBlurOP,
            ptTile,
            &__top_canvas,
            255 - 16);
        
        ARM_2D_OP_WAIT_ASYNC();
    #endif

        arm_lcd_text_set_target_framebuffer(NULL);
        arm_lcd_text_force_char_use_same_width(true);

        /* draw mid and near letter trains */
        arm_foreach(__letter_train_t, 
                    &this.tTrains[MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT], 
                    dimof(this.tTrains) - MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT, 
                    ptTrain) {

            arm_2d_size_t tCharSize = {0};
            uint8_t chMaxOpacity = 255;
            switch (ptTrain->u2Stage) {
                case STAGE_MID:
                    /* use default font */
                    arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_6x8); 
                    tCharSize = ARM_2D_FONT_6x8.use_as__arm_2d_font_t.tCharSize;
                    chMaxOpacity = 128;
                    break;
                
                case STAGE_NEAR:
                    arm_lcd_text_set_font((const arm_2d_font_t *)&ARM_2D_FONT_Arial14_A1);
                    tCharSize = ARM_2D_FONT_Arial14_A1.use_as__arm_2d_user_font_t.use_as__arm_2d_font_t.tCharSize;
                    break;
                default:
                    assert(false);      /* this should not happen */
                    break;
            }

            arm_lcd_text_set_draw_region(&ptTrain->tRegion);
            arm_lcd_text_set_colour(GLCD_COLOR_GREEN, GLCD_COLOR_BLACK);

            q16_t q16OpacityRatio = div_n_q16(  reinterpret_q16_s16(chMaxOpacity), 
                                                ptTrain->u16NumberOfChars);

            __arm_2d_hint_optimize_for_pfb__(ptTrain->tRegion) {

                /* reset seed */
                srand(ptTrain->u8RandomSeed);

                for (int_fast16_t n = 0; n < ptTrain->u16NumberOfChars; n++) {
                    
                    char tChar = (rand() % (126 - 32)) + 32;

                    uint8_t chOpacity = reinterpret_s16_q16( mul_n_q16(q16OpacityRatio, n));
                    arm_lcd_text_set_opacity( chOpacity );
                    arm_lcd_putchar(&tChar);
                }
            }
        }

    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_matrix_t *__arm_2d_scene_matrix_init(   
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_matrix_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    if (NULL == ptThis) {
        ptThis = (user_scene_matrix_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_matrix_t),
                                                        __alignof__(user_scene_matrix_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_matrix_t));

    *ptThis = (user_scene_matrix_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_matrix_load,
            .fnScene        = &__pfb_draw_scene_matrix_handler,
            //.fnAfterSwitch  = &__after_scene_matrix_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_matrix_background_start,
            //.fnOnBGComplete = &__on_scene_matrix_background_complete,
            .fnOnFrameStart = &__on_scene_matrix_frame_start,
            .fnBeforeSwitchOut = &__before_scene_matrix_switching_out,
            .fnOnFrameCPL   = &__on_scene_matrix_frame_complete,
            .fnDepose       = &__on_scene_matrix_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_matrix_t begin ---------------*/

    /* initialize all letter trains */
    do {
        /* far stage letter trains */
        arm_foreach(__letter_train_t, 
                    &this.tTrains[0], 
                    MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT,
                    ptTrain) {
            ptTrain->u2Stage = STAGE_FAR,

            letter_train_relocate(ptTrain, __top_canvas.tSize, 0);
        }

        /* mid stage letter trains */
        arm_foreach(__letter_train_t, 
                    &this.tTrains[MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT], 
                    MATRIX_LETTER_TRAIN_MID_STAGE_COUNT,
                    ptTrain) {
            ptTrain->u2Stage = STAGE_MID,

            letter_train_relocate(ptTrain, __top_canvas.tSize, 0);

            arm_2d_size_t tCharSize = ARM_2D_FONT_6x8
                                        .use_as__arm_2d_font_t.tCharSize;
            ptTrain->u16NumberOfChars = (-ptTrain->tRegion.tLocation.iY)    /* the train size */
                                      / tCharSize.iHeight;
        }

        /* near stage letter trains */
        arm_foreach(__letter_train_t, 
                    &this.tTrains[  MATRIX_LETTER_TRAIN_FAR_STAGE_COUNT 
                                 +  MATRIX_LETTER_TRAIN_MID_STAGE_COUNT], 
                    MATRIX_LETTER_TRAIN_NEAR_STAGE_COUNT,
                    ptTrain) {
            ptTrain->u2Stage = STAGE_NEAR,

            letter_train_relocate(ptTrain, __top_canvas.tSize, 0);
        }

        /* give each train a random start postion */
        srand(arm_2d_helper_get_system_timestamp());
        arm_foreach(this.tTrains) {
            _->tRegion.tLocation.iY += rand() % - _->tRegion.tSize.iHeight; 
        }
    } while(0);

#if MATRIX_LETTER_TRAIN_USE_BLUR
    ARM_2D_OP_INIT(this.tBlurOP);
#endif

    /* ------------   initialize members of user_scene_matrix_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


