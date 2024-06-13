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

#define __USER_SCENE_VIRTUAL_RESOURCE_IMPLEMENT__
#include "arm_2d_scene_virtual_resource.h"

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
#   define c_bmpHelium              c_bmpHeliumGRAY8
#   define c_tileHelium             c_tileHeliumGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_bmpHelium              c_bmpHeliumRGB565
#   define c_tileHelium             c_tileHeliumRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_bmpHelium              c_bmpHeliumCCCN888
#   define c_tileHelium             c_tileHeliumCCCN888

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

extern const COLOUR_INT c_bmpHelium[];
extern const arm_2d_tile_t c_tileHelium;
extern const arm_2d_tile_t c_tileDigitsFontA4Mask;
extern const uint8_t c_bmpDigitsFontA4Alpha[];

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

/*-----------------------------------------------------------------------------*
 * Virtual Resources                                                           *
 *-----------------------------------------------------------------------------*/
static arm_2d_vres_t s_tBigImage = 
    disp_adapter0_impl_vres(   
        ARM_2D_COLOUR, 
        320, 
        256, 
        .pTarget = (uintptr_t)&c_bmpHelium
    );

static
const arm_2d_tile_t c_tChildImage = 
    impl_child_tile(
        s_tBigImage.tTile,
        160,
        128,
        160,
        128
    );

static arm_2d_vres_t s_vresA4Font = 
    disp_adapter0_impl_vres(   
        ARM_2D_COLOUR_MASK_A4, 
        15, 
        336, 
        .pTarget = (uintptr_t)c_bmpDigitsFontA4Alpha,
    );

/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Virtual Resource Helper User Implemented Interfaces                        *
 *----------------------------------------------------------------------------*/
void __disp_adapter0_vres_read_memory(  intptr_t pObj, 
                                        void *pBuffer,
                                        uintptr_t pAddress,
                                        size_t nSizeInByte)
{
    ARM_2D_UNUSED(pObj);
    /* it is just a demo, in real application, you can place a function to 
     * read SPI Flash 
     */
    memcpy(pBuffer, (void * const)pAddress, nSizeInByte);
}

uintptr_t __disp_adapter0_vres_get_asset_address(   uintptr_t pObj,
                                                    arm_2d_vres_t *ptVRES)
{
    ARM_2D_UNUSED(ptVRES);
    
    /* if you don't care about OOC, you can ignore the following example code, 
     * instead, you should return the address of the target asset (pictures, 
     * masks etc) in the external memory, e.g. SPI Flash 
     */
    
    return pObj;
}


/*----------------------------------------------------------------------------*
 * Virtual Resource Font                                                      *
 *----------------------------------------------------------------------------*/

static
IMPL_FONT_DRAW_CHAR(__digit_font_a4_draw_char)
{
    ARM_2D_UNUSED(fScale);
    return arm_2d_fill_colour_with_a4_mask_and_opacity( ptTile, 
                                            ptRegion,
                                            ptileChar,
                                            (__arm_2d_color_t){tForeColour},
                                            (uint8_t)chOpacity);
}

static
IMPL_FONT_GET_CHAR_DESCRIPTOR(__digit_font_get_char_descriptor);

struct {
    implement(arm_2d_user_font_t);

    arm_2d_char_idx_t tNumbers;
    arm_2d_char_idx_t tABCDEF;
    arm_2d_char_idx_t tMinor;
    arm_2d_char_idx_t tPlus;
    arm_2d_char_idx_t tDot;
    arm_2d_char_idx_t tE;
    arm_2d_char_idx_t tBlank;
} ARM_2D_FONT_VRES_A4_DIGITS_ONLY = {

    .use_as__arm_2d_user_font_t = {
        .use_as__arm_2d_font_t = {
            .tileFont = impl_child_tile(
                s_vresA4Font,
                0,          /* x offset */
                0,          /* y offset */
                15,         /* width */
                336         /* height */
            ),
            .tCharSize = {
                .iWidth = 15,
                .iHeight = 16,
            },
            .nCount =  20,                             //!< Character count
            .fnGetCharDescriptor = &__digit_font_get_char_descriptor,
            .fnDrawChar = &__digit_font_a4_draw_char,
        },
        .hwCount = 7,
        .hwDefaultCharIndex = 6, /* tBlank */
    },
    
    .tNumbers = {
        .chStartCode = {'0'},
        .hwCount = 10,
        .hwOffset = 0,
    },
    
    .tABCDEF = {
        .chStartCode = {'A'},
        .hwCount = 6,
        .hwOffset = 10,
    },
    
    .tMinor = {
        .chStartCode = {'-'},
        .hwCount = 1,
        .hwOffset = 16,
    },
    
    .tPlus = {
        .chStartCode = {'+'},
        .hwCount = 1,
        .hwOffset = 17,
    },

    .tDot = {
        .chStartCode = {'.'},
        .hwCount = 1,
        .hwOffset = 18,
    },

    .tE = {
        .chStartCode = {'e'},
        .hwCount = 1,
        .hwOffset = 20,
    },
    .tBlank = {
        .chStartCode = {' '},
        .hwCount = 1,
        .hwOffset = 19,
    },
};



static
IMPL_FONT_GET_CHAR_DESCRIPTOR(__digit_font_get_char_descriptor)
{
    assert(NULL != ptFont);
    assert(NULL != ptDescriptor);
    assert(NULL != pchCharCode);
    
    arm_2d_user_font_t *ptThis = (arm_2d_user_font_t *)ptFont;
    
    memset(ptDescriptor, 0, sizeof(arm_2d_char_descriptor_t));
    
    ptDescriptor->tileChar.tRegion.tSize = ptFont->tCharSize;
    ptDescriptor->tileChar.ptParent = (arm_2d_tile_t *)&ptFont->tileFont;
    ptDescriptor->tileChar.tInfo.bDerivedResource = true;
    ptDescriptor->chCodeLength = 1;

    ptDescriptor->iBearingX = 0;
    ptDescriptor->iBearingY = ptFont->tCharSize.iHeight;
    ptDescriptor->iAdvance = ptFont->tCharSize.iWidth;
    
    arm_foreach( arm_2d_char_idx_t, &ARM_2D_FONT_VRES_A4_DIGITS_ONLY.tNumbers, this.hwCount, ptItem) {
        if (    *pchCharCode >= ptItem->chStartCode[0] 
            &&  *pchCharCode < (ptItem->chStartCode[0] + ptItem->hwCount)) {
            int16_t iOffset = *pchCharCode - ptItem->chStartCode[0];
            
            ptDescriptor->tileChar.tRegion.tLocation.iY 
                = (int16_t)((ptItem->hwOffset + iOffset) * ptFont->tCharSize.iHeight);
            return ptDescriptor;
        }
    }

    /* default: use blank */
    ptDescriptor->tileChar.tRegion.tLocation.iY 
        = (int16_t)(   this.tLookUpTable[this.hwDefaultCharIndex].hwOffset 
                   *   ptFont->tCharSize.iHeight);

    return ptDescriptor;
}

/*----------------------------------------------------------------------------*
 * Scene                                                                      *
 *----------------------------------------------------------------------------*/

static void __on_scene_virtual_resource_load(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_virtual_resource_depose(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene virtual_resource                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_virtual_resource_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_virtual_resource_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_virtual_resource_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* just refresh once */
    arm_2d_dirty_region_item_ignore_set(&s_tDirtyRegions[0], true);

}

static void __on_scene_virtual_resource_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

static void __before_scene_virtual_resource_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_virtual_resource_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_virtual_resource_t *ptThis = (user_scene_virtual_resource_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;

    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __canvas) {
        /* draw images to the screen center using virtual resource */
        arm_2d_align_centre(__canvas, s_tBigImage.tTile.tRegion.tSize) {
        
            /* draw with a virtual resource */
            arm_2d_tile_copy_only(   &s_tBigImage.tTile,    /* source tile */
                                ptTile,                     /* target frame buffer */
                                &__centre_region);

            ARM_2D_OP_WAIT_ASYNC();

        #if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ > 1
            /* draw a child tile of the virtual resource */
            arm_2d_tile_copy(   &c_tChildImage,         /* source tile */
                                ptTile,                 /* target frame buffer */
                                &__centre_region, 
                                ARM_2D_CP_MODE_XY_MIRROR);
        #endif
        }
        
        arm_2d_size_t tCharSize = ARM_2D_FONT_VRES_A4_DIGITS_ONLY
                                    .use_as__arm_2d_user_font_t
                                        .use_as__arm_2d_font_t.tCharSize;

        /* draw a white bar */
        arm_2d_align_centre(__canvas, 
                            ptTile->tRegion.tSize.iWidth, 
                            tCharSize.iHeight * 3 ) {
            arm_2d_fill_colour_with_opacity(ptTile, 
                                            &__centre_region,
                                            (__arm_2d_color_t){GLCD_COLOR_WHITE}, 
                                            255 - 32);
        }

    #if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__ > 1
        /* draw A4 fonts that stored as a virtual resource */
        arm_2d_align_centre(__canvas, 
                            tCharSize.iWidth * 8, 
                            tCharSize.iHeight * 2 ) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_VRES_A4_DIGITS_ONLY);
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(GLCD_COLOR_DARK_GREY, GLCD_COLOR_WHITE);
            arm_lcd_puts("0123456789ABCDEF");
        }
    #endif
    
        /* display info */
        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Virtual Resource Demo");
    }

    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_virtual_resource_t *__arm_2d_scene_virtual_resource_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_virtual_resource_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
    s_tDirtyRegions[0].tRegion = tScreen;

    if (NULL == ptThis) {
        ptThis = (user_scene_virtual_resource_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_virtual_resource_t),
                                                        __alignof__(user_scene_virtual_resource_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_virtual_resource_t));

    *ptThis = (user_scene_virtual_resource_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_WHITE}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_virtual_resource_load,
            .fnScene        = &__pfb_draw_scene_virtual_resource_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_virtual_resource_background_start,
            //.fnOnBGComplete = &__on_scene_virtual_resource_background_complete,
            .fnOnFrameStart = &__on_scene_virtual_resource_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_virtual_resource_switching_out,
            .fnOnFrameCPL   = &__on_scene_virtual_resource_frame_complete,
            .fnDepose       = &__on_scene_virtual_resource_depose,

            .bUseDirtyRegionHelper = false,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_virtual_resource_t begin ---------------*/


    /* ------------   initialize members of user_scene_virtual_resource_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


