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
#include "./virtual_resource_demo.h"
#include "platform.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapter_0.h"
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
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

#if __DISP0_CFG_VIRTUAL_RESOURCE_HELPER__

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileHelium           c_tileHeliumGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileHelium           c_tileHeliumRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileHelium           c_tileHeliumCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    intptr_t pResource;
} resource_loader_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileHelium ;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static resource_loader_t s_tLoader = {
    .pResource = (intptr_t)&c_tileHelium,
};

static arm_2d_vres_t s_tBigImage = 
    disp_adapter0_impl_vres(   
        ARM_2D_COLOUR, 
        320, 
        256, 
        .pTarget = (uintptr_t)&s_tLoader
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


/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Virtual Resource Helper User Implemented Interfaces                        *
 *----------------------------------------------------------------------------*/
void __disp_adapter0_vres_read_memory(intptr_t pObj, 
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

uintptr_t __disp_adapter0_vres_get_asset_address(uintptr_t pObj,
                                               arm_2d_vres_t *ptVRES)
{
    ARM_2D_UNUSED(ptVRES);
    
    /* if you don't care about OOC, you can ignore the following example code, 
     * instead, you should return the address of the target asset (pictures, 
     * masks etc) in the external memory, e.g. SPI Flash 
     */
    resource_loader_t *ptLoader = (resource_loader_t *)pObj;
    arm_2d_tile_t *ptTile = (arm_2d_tile_t *)ptLoader->pResource;
    
    return ptTile->nAddress;
}

/*----------------------------------------------------------------------------*
 * Scene 0                                                                    *
 *----------------------------------------------------------------------------*/


static void __on_scene_depose(arm_2d_scene_t *ptScene)
{
    free(ptScene);
}



static void __on_scene0_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    /* switch to next scene after 2s */
    if (arm_2d_helper_is_time_out(2000)) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
}

//static
//IMPL_PFB_ON_DRAW(__pfb_draw_scene0_background_handler)
//{
//    ARM_2D_UNUSED(pTarget);
//    ARM_2D_UNUSED(bIsNewFrame);

//    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

//    arm_2d_op_wait_async(NULL);

//    return arm_fsm_rt_cpl;
//}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    /* background colour */
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    
    /* draw images to the screen center using virtual resource */
    arm_2d_align_centre(ptTile->tRegion, s_tBigImage.tTile.tRegion.tSize) {
    
        /* draw with a virtual resource */
        arm_2d_tile_copy(   &s_tBigImage.tTile,     /* source tile */
                            ptTile,                 /* target frame buffer */
                            &__centre_region, 
                            ARM_2D_CP_MODE_COPY);

        arm_2d_op_wait_async(NULL);

        /* draw a child tile of the virtual resource */
        arm_2d_tile_copy(   &c_tChildImage,         /* source tile */
                            ptTile,                 /* target frame buffer */
                            &__centre_region, 
                            ARM_2D_CP_MODE_XY_MIRROR);
    }

    /* display info */
    arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Virtual Resource Demo");

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static void __app_scene0_init(void)
{

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a region for the busy wheel */
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),
        
        /* top left corner for text display */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
    
    s_tDirtyRegions[0].tRegion.tLocation = (arm_2d_location_t){
        .iX = ((__DISP0_CFG_SCEEN_WIDTH__ - c_tileHelium.tRegion.tSize.iWidth) >> 1),
        .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ - c_tileHelium.tRegion.tSize.iHeight) >> 1),
    };
    s_tDirtyRegions[0].tRegion.tSize = c_tileHelium.tRegion.tSize;
    
    
    arm_2d_scene_t *ptScene = (arm_2d_scene_t *)malloc(sizeof(arm_2d_scene_t));
    assert(NULL != ptScene);
    
    *ptScene = (arm_2d_scene_t){
        .fnBackground   = NULL,
        .fnScene        = &__pfb_draw_scene0_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        .fnOnBGStart    = NULL,
        .fnOnBGComplete = NULL,
        .fnOnFrameStart = NULL,
        .fnOnFrameCPL   = &__on_scene0_frame_complete,
        .fnDepose       = &__on_scene_depose,
    };
    arm_2d_scene_player_append_scenes( &DISP0_ADAPTER, ptScene, 1);
}

void virtual_resource_demo_init(void)
{
    __app_scene0_init();
}

#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


