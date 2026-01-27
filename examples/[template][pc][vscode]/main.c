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
#include <stdio.h>
#include "Virtual_TFT_Port.h"
#include "arm_2d_helper.h"
#include "arm_2d_scenes.h"
#include "arm_2d_disp_adapters.h"

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
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


#if __DISP0_CFG_SCEEN_WIDTH__ == 480                                            \
 && __DISP0_CFG_SCEEN_HEIGHT__ == 480

extern const arm_2d_tile_t c_tileWatchCoverRoundGRAY8;
extern const arm_2d_tile_t c_tileGlassHaloMask;

__OVERRIDE_WEAK
IMPL_PFB_ON_DRAW(__disp_adapter0_user_draw_navigation)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(bIsNewFrame);

    arm_2d_canvas(ptTile, __watch_cover_canvas) {

        arm_2d_align_centre(__watch_cover_canvas, 
                            c_tileWatchCoverRoundGRAY8.tRegion.tSize) {
            
            arm_2d_fill_colour_with_mask(
                                        ptTile,
                                        &__centre_region,
                                        &c_tileWatchCoverRoundGRAY8, 
                                        (__arm_2d_color_t){GLCD_COLOR_BLACK});
        }

        arm_2d_align_centre(__watch_cover_canvas, 
                            c_tileGlassHaloMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask(
                                        ptTile,
                                        &__centre_region,
                                        &c_tileGlassHaloMask, 
                                        (__arm_2d_color_t){GLCD_COLOR_LIGHT_GREY});
        }
    }

    return arm_fsm_rt_cpl;
}
#endif

void run_os_command(const char *pchCommandLine) {

#define CONSOLE_OUTPUT_SIZE (64 * 1024)

    char *pchBuffer = malloc(CONSOLE_OUTPUT_SIZE);
    assert(NULL != pchBuffer);

    FILE *ptPipe = popen(pchCommandLine, "r");
    if (!ptPipe) {
        printf("Failed to execute command: [%s]\r\n", pchCommandLine);
        return;
    }

    while (fgets(pchBuffer, CONSOLE_OUTPUT_SIZE, ptPipe) != NULL) {
        printf("%s", pchBuffer);
    }

    pclose(ptPipe);
    free(pchBuffer);
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
extern int app_2d_main_thread (void *argument);

static bool __lcd_sync_handler(void *pTarget)
{
    return VT_sdl_vsync();
}

int main(int argc, char* argv[])
{
    VT_init();

    printf("\r\nArm-2D PC Template\r\n");

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    //arm_2d_helper_pfb_anti_noise_scan_mode(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, true);

    /* register a low level sync-up handler to wait LCD finish rendering the previous frame */
    do {
        arm_2d_helper_pfb_dependency_t tDependency = {
            .evtOnLowLevelSyncUp = {
                .fnHandler = &__lcd_sync_handler,
            },
        };
        arm_2d_helper_pfb_update_dependency(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, 
                                            ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP,
                                            &tDependency);
    } while(0);

    SDL_CreateThread(app_2d_main_thread, "arm-2d thread", NULL);

    while (1) {
        if(!VT_sdl_refresh_task()){
            break;
        }
    }

    VT_deinit();
    return 0;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
