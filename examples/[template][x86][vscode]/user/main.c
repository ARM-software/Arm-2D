#include <stdio.h>
#include "SDL.h"
#undef main
#include "Virtual_TFT_Port.h"
#include "arm_2d.h"
#include "arm_2d_disp_adapter_0.h"
#include "arm_2d_scene_benchmark_watch_panel_cover.h"
#include <time.h>


extern
void disp_adapter0_init(void);

extern
arm_fsm_rt_t disp_adapter0_task(void);

uint32_t VT_timerCallback(uint32_t interval, void *param)//回调函数
{
    //    llTimer_ticks(10);
    return interval;
}

int32_t Disp0_DrawBitmap(int16_t x,int16_t y,int16_t width,int16_t height,const uint8_t *bitmap)
{
    VT_Fill_Multiple_Colors(x, y,x+width-1,y+height-1,(color_typedef*) bitmap);

    return 0;
}

uint32_t SystemCoreClock=3600000000;

int64_t arm_2d_helper_get_system_timestamp(void)
{
    return (int64_t)clock();
}

uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    return 1000;
}

int main()
{
    printf("arm-2d sdl\n");
    VT_Init();

//    SDL_GetTicks();
//    SDL_AddTimer(10, VT_timerCallback, NULL);

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    arm_2d_run_benchmark();
//    arm_2d_scene0_init(&DISP0_ADAPTER);

    while(1)
    {
        disp_adapter0_task();
    }
}

