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

#define __USER_SCENE_BALLS_IMPLEMENT__
#include "arm_2d_scene_balls.h"

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
#   define c_tileBackground         c_tileBackgroundSmallGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565
#   define c_tileBackground         c_tileBackgroundSmallRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#   define c_tileBackground         c_tileBackgroundSmallCCCA8888

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
extern const arm_2d_tile_t c_tileGlassBall40A4Mask;
extern const arm_2d_tile_t c_tileRadialGradientMask;
extern const arm_2d_tile_t c_tileRadialGradientA4Mask;
extern const arm_2d_tile_t c_tileRadialGradientA2Mask;

extern 
const arm_2d_tile_t c_tileBackground;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE double get_current_ms(void)
{
    return (double)arm_2d_helper_convert_ticks_to_ms(arm_2d_helper_get_system_timestamp());
}

#define PHYSAC_IMPLEMENTATION
#define PHYSAC_NO_THREADS
#define PHYSAC_STANDALONE

#define     PHYSAC_MALLOC(size)                                                 \
                __arm_2d_allocate_scratch_memory(   size,                       \
                                                    4,                          \
                                                    ARM_2D_MEM_TYPE_UNSPECIFIED)
#define     PHYSAC_FREE(ptr)                                                    \
                __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptr)

#ifndef _STDBOOL_H
#   define _STDBOOL_H
#endif
#define PHYSAC_COLLISION_ITERATIONS     2
#define PHYSAC_GET_CURRENT_TIME()       get_current_ms()
#define PHYSAC_MAX_VERTICES             4
#define PHYSAC_MAX_MANIFOLDS            128
#define PHYSAC_MAX_BODIES               (DEMO_BALL_COUNT + 4)

#include "./Physac/src/physac.h"

static void __on_scene_balls_load(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_add_items(   &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                            this.tDirtyRegionItems,
                                            dimof(this.tDirtyRegionItems));

    this.bDirtyRegionOptimizationStatus 
        = arm_2d_helper_pfb_disable_dirty_region_optimization(
            &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t);
}

static void __after_scene_balls_switching(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    this.bSuspendPhysics = false;

}

static void __on_scene_balls_depose(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    arm_2d_helper_dirty_region_remove_items(   &this.use_as__arm_2d_scene_t.tDirtyRegionHelper,
                                            this.tDirtyRegionItems,
                                            dimof(this.tDirtyRegionItems));
    ClosePhysics();

    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }
    ptScene->ptPlayer = NULL;
    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene balls                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_balls_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_balls_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_balls_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);


}

static void apply_force(void)
{
    srand(arm_2d_helper_get_system_timestamp());

    for (int32_t nIndex = 0; nIndex < GetPhysicsBodiesCount(); nIndex++) {
        PhysicsBody tBody = GetPhysicsBody(nIndex);

        if (tBody->shape.type != PHYSICS_CIRCLE) {
            continue;
        }
        PhysicsAddForce(tBody, (Vector2){ (rand() & 0xFFF) - 0x7FF , (rand() & 0xFFF) - 0x7FF });
    }
}

static void __on_scene_balls_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    if (this.bSuspendPhysics) {
        return ;
    }

    if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        apply_force();
    }

    RunPhysicsStep();

    for (int32_t nIndex = 0; nIndex < GetPhysicsBodiesCount(); nIndex++) {
        PhysicsBody tBody = GetPhysicsBody(nIndex);

        if (tBody->shape.type != PHYSICS_CIRCLE) {
            continue;
        }
        
        if (tBody->collision) {
            tBody->collision = false;
            tBody->User = 255;
        } else if (tBody->User > (64 + 32)) {
            tBody->User -= 4;
        }
    }
}

static void __before_scene_balls_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_balls_t *ptThis = (user_scene_balls_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    this.bSuspendPhysics = true;

    if (this.bDirtyRegionOptimizationStatus) {
        arm_2d_helper_pfb_enable_dirty_region_optimization(
            &ptScene->ptPlayer->use_as__arm_2d_helper_pfb_t,
            NULL, 
            0);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_balls_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_balls_t *ptThis = (user_scene_balls_t *)pTarget;

    arm_2d_canvas(ptTile, __top_canvas) {
    /*-----------------------draw the scene begin-----------------------*/
        
    #if DEMO_BALL_SHOW_BACKGROUND
        arm_2d_align_centre(__top_canvas, c_tileBackground.tRegion.tSize) {
            arm_2d_tile_copy_only(  &c_tileBackground, 
                                    ptTile, 
                                    &__centre_region);
        }
    #endif

        /* draw the cmsis logo using mask in the centre of the screen */
        arm_2d_align_centre(__top_canvas, c_tileCMSISLogo.tRegion.tSize) {
            arm_2d_fill_colour_with_a4_mask_and_opacity(   
                                                ptTile, 
                                                &__centre_region, 
                                                &c_tileCMSISLogoA4Mask, 
                                                (__arm_2d_color_t){GLCD_COLOR_ORANGE},
                                                128);
        }


        arm_2d_region_t tBallRegion = {
            .tSize = c_tileGlassBall40A4Mask.tRegion.tSize,
        };

        arm_2d_region_t tShadowRegion = {
            .tSize = c_tileRadialGradientMask.tRegion.tSize,
        };

        int_fast8_t chBallIndex = 0;
        for (int32_t nIndex = 0; nIndex < GetPhysicsBodiesCount(); nIndex++) {

            PhysicsBody tBody = GetPhysicsBody(nIndex);
            if (NULL == tBody) {
                assert(false);
                continue;
            }

            if (tBody->shape.type == PHYSICS_CIRCLE) {

                tShadowRegion.tLocation.iX = tBody->position.x - c_tileRadialGradientMask.tRegion.tSize.iWidth / 2 - 5;
                tShadowRegion.tLocation.iY = tBody->position.y - c_tileRadialGradientMask.tRegion.tSize.iHeight / 2 + 5;

                __arm_2d_hint_optimize_for_pfb__(tShadowRegion) {
                    arm_2d_fill_colour_with_mask_and_opacity(   
                        ptTile, 
                        &tShadowRegion, 
                        &c_tileRadialGradientMask, 
                        (__arm_2d_color_t){GLCD_COLOR_ORANGE},
                        tBody->User);
                    ARM_2D_OP_WAIT_ASYNC();
                }

                tBallRegion.tLocation.iX = tBody->position.x - tBody->shape.radius;
                tBallRegion.tLocation.iY = tBody->position.y - tBody->shape.radius;

                __arm_2d_hint_optimize_for_pfb__(tBallRegion) {
                    arm_2d_fill_colour_with_a4_mask_and_opacity(   
                        ptTile, 
                        &tBallRegion, 
                        &c_tileGlassBall40A4Mask, 
                        (__arm_2d_color_t){GLCD_COLOR_WHITE},
                        255);

                    ARM_2D_OP_WAIT_ASYNC();
                }

                arm_2d_region_t tUpdateRegion;

                arm_2d_region_get_minimal_enclosure(&tShadowRegion, &tBallRegion, &tUpdateRegion);

                arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItems[chBallIndex],
                                                        (arm_2d_tile_t *)ptTile,
                                                        &__top_canvas,
                                                        &tUpdateRegion);
                
                chBallIndex++;
            } else {
                Vector2 vertexA = GetPhysicsShapeVertex(tBody, 0);
                Vector2 vertexB = GetPhysicsShapeVertex(tBody, 2);
                
                arm_2d_region_t tRect;
                arm_2d_create_region_from_corner_points(
                        &tRect, 
                        (arm_2d_location_t) {.iX = vertexA.x, .iY = vertexA.y},
                        (arm_2d_location_t) {.iX = vertexB.x, .iY = vertexB.y}
                        );
                
                if (tBody->enabled) {
                    arm_2d_fill_colour(ptTile, &tRect, GLCD_COLOR_LIGHT_GREY);
                } else {
                    arm_2d_fill_colour(ptTile, &tRect, GLCD_COLOR_DARK_GREY);
                }
            }
            

        }


        /* draw text at the top-left corner */

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        arm_lcd_text_set_draw_region(NULL);
        arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        arm_lcd_text_location(0,0);
        arm_lcd_puts("Scene balls");

    /*-----------------------draw the scene end  -----------------------*/
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_balls_t *__arm_2d_scene_balls_init(   arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_balls_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    const arm_2d_tile_t *ptCurrentTile = NULL;

    /* get the screen region */
    arm_2d_region_t __top_canvas
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);

    if (NULL == ptThis) {
        ptThis = (user_scene_balls_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_balls_t),
                                                        __alignof__(user_scene_balls_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_balls_t));

    *ptThis = (user_scene_balls_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_balls_load,
            .fnScene        = &__pfb_draw_scene_balls_handler,
            .fnAfterSwitch  = &__after_scene_balls_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_balls_background_start,
            //.fnOnBGComplete = &__on_scene_balls_background_complete,
            .fnOnFrameStart = &__on_scene_balls_frame_start,
            .fnBeforeSwitchOut = &__before_scene_balls_switching_out,
            .fnOnFrameCPL   = &__on_scene_balls_frame_complete,
            .fnDepose       = &__on_scene_balls_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_balls_t begin ---------------*/

    InitPhysics();
    this.bSuspendPhysics = true;

    float fRadius = c_tileGlassBall40A4Mask.tRegion.tSize.iWidth / 2.0f;

#define BORDER_WIDTH    16
#define RESTITUTION     1.0f
    do {
        PhysicsBody tBoarder = CreatePhysicsBodyRectangle(
                                (Vector2){ __top_canvas.tSize.iWidth / 2, __top_canvas.tSize.iHeight }, 
                                __top_canvas.tSize.iWidth - BORDER_WIDTH, 
                                BORDER_WIDTH * 2, 
                                10);
        tBoarder->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
        tBoarder->restitution = RESTITUTION;

        tBoarder = CreatePhysicsBodyRectangle(
                                (Vector2){ __top_canvas.tSize.iWidth / 2, 0 }, 
                                __top_canvas.tSize.iWidth - BORDER_WIDTH, 
                                BORDER_WIDTH * 2, 
                                10);
        tBoarder->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
        tBoarder->restitution = RESTITUTION;

        tBoarder = CreatePhysicsBodyRectangle(
                                (Vector2){ 0, __top_canvas.tSize.iHeight / 2 }, 
                                BORDER_WIDTH * 2, 
                                __top_canvas.tSize.iHeight, 
                                10);
        tBoarder->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
        tBoarder->restitution = RESTITUTION;

        tBoarder = CreatePhysicsBodyRectangle(
                                (Vector2){ __top_canvas.tSize.iWidth, __top_canvas.tSize.iHeight / 2 }, 
                                BORDER_WIDTH * 2, 
                                __top_canvas.tSize.iHeight, 
                                10);
        tBoarder->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
        tBoarder->restitution = RESTITUTION;
    } while(0);

    srand(arm_2d_helper_get_system_timestamp());

    for (uint_fast8_t n = 0; n < DEMO_BALL_COUNT; n++) {

        int16_t x = rand() % (__top_canvas.tSize.iWidth - BORDER_WIDTH * 2 - (int)fRadius);
        int16_t y = rand() % (__top_canvas.tSize.iHeight - BORDER_WIDTH * 2 - (int)fRadius);

        PhysicsBody tBall = 
            CreatePhysicsBodyCircle((Vector2){ x, y }, fRadius, 8.0f);
        if (NULL == tBall) {
            break;
        }

        tBall->restitution = RESTITUTION;
        tBall->useGravity = false;

        PhysicsAddForce(tBall, (Vector2){ (rand() & 0xFFF) - 0x7FF , (rand() & 0xFFF) - 0x7FF });
    }

    /* ------------   initialize members of user_scene_balls_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif


