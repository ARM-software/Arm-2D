#include "Virtual_TFT_Port.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "arm_2d.h"
#include <time.h>
#include "arm_2d_disp_adapters.h"

#undef main

#define monochrome_2_RGB888(color)                (color?0x000000:0xffffff)
#define GRAY8_2_RGB888(color)                     (((color&0xFF)<<16)+((color&0xFF)<<8)+((color&0xFF)))
#define RGB565_2_RGB888(color)                    (((color&0xF800)<<8)+((color&0x7E0)<<5)+((color&0x1F)<<3))


#define RGB888_2_monochrome(color)                ((color)?0:1)
#define RGB888_2_GRAY8(color)                     (((((color&0xff0000)>>16)) + (((color&0xff00)>>8)) + (((color&0xff)))) / 3)
#define RGB888_2_RGB565(color)                    ((((color&0xff0000)>>19) << 11) + (((color&0xff00)>>10) << 5) + (((color&0xff)>>3)))


// 1 8(233) 16(565) 24(888) 32(8888)
#if VT_COLOR_DEPTH == 1
#define DEV_2_VT_RGB(color)                  monochrome_2_RGB888(color)
#define VT_RGB_2_DEV(color)                  RGB888_2_monochrome(color)
#elif VT_COLOR_DEPTH == 8
#define DEV_2_VT_RGB(color)                  GRAY8_2_RGB888(color)
#define VT_RGB_2_DEV(color)                  RGB888_2_GRAY8(color)
#elif VT_COLOR_DEPTH == 16
#define DEV_2_VT_RGB(color)                  RGB565_2_RGB888(color)
#define VT_RGB_2_DEV(color)                  RGB888_2_RGB565(color)
#elif VT_COLOR_DEPTH == 24 || VT_COLOR_DEPTH == 32
#define DEV_2_VT_RGB(color)                 (color)
#define VT_RGB_2_DEV(color)                 (color)
#endif

static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t tft_fb[VT_WIDTH * VT_HEIGHT];
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_refr_cpl = false;
static volatile bool sdl_quit_qry = false;

static bool left_button_is_down = false;
static int16_t last_x = 0;
static int16_t last_y = 0;

extern void VT_Init(void);
extern void VT_Fill_Single_Color(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef color);
extern void VT_Fill_Multiple_Colors(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef * color_p);
extern void VT_Set_Point(int32_t x, int32_t y, color_typedef color);
extern color_typedef VT_Get_Point(int32_t x, int32_t y);
extern void VT_Clear(color_typedef color);
extern bool VT_Mouse_Get_Point(int16_t *x,int16_t *y);


/*******************************************************************************
 * @name     :VT_Mouse_Get_Point
 * @brief    :get mouse click position
 * @param    :x       pointer,save click position x
 *            y       pointer,save click position y
 * @return   :true    press
 *            false   relase
 * @version  :V0.1
 * @author   :
 * @date     :2018.11.20
 * @details  :
*******************************************************************************/
bool VT_mouse_get_point(int16_t *x,int16_t *y)
{
    *x=last_x;
    *y=last_y;
    return left_button_is_down;
}

int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if(event->type == SDL_QUIT) {
        sdl_quit_qry = true;
    }

    return 1;
}

static void monitor_sdl_clean_up(void)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void monitor_sdl_init(void)
{

    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

    window = SDL_CreateWindow( "Arm-2D v" 
                                ARM_TO_STRING(ARM_2D_VERSION_MAJOR)
                                "."
                                ARM_TO_STRING(ARM_2D_VERSION_MINOR)
                                "."
                                ARM_TO_STRING(ARM_2D_VERSION_PATCH)
                                "-"
                                ARM_2D_VERSION_STR
                                " ("
                                ARM_TO_STRING(VT_WIDTH)
                                "*"
                                ARM_TO_STRING(VT_HEIGHT)
                                " "
                                ARM_TO_STRING(VT_COLOR_DEPTH)
                                "bits )"
                                ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                VT_WIDTH, VT_HEIGHT, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

#if VT_VIRTUAL_MACHINE
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
#else
    renderer = SDL_CreateRenderer(window, -1, 0);
#endif
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, VT_WIDTH, VT_HEIGHT);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    memset(tft_fb, 77, VT_WIDTH * VT_HEIGHT * sizeof(uint32_t));
    SDL_UpdateTexture(texture, NULL, tft_fb, VT_WIDTH * sizeof(uint32_t));
    sdl_refr_qry = true;
    sdl_inited = true;
}

void VT_sdl_refresh_task(void)
{
    if(sdl_refr_qry != false)
    {
        if (arm_2d_helper_is_time_out(1000/60)) 
        {
            sdl_refr_qry = false;
            SDL_UpdateTexture(texture, NULL, tft_fb, VT_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);

            /*Update the renderer with the texture containing the rendered image*/
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            sdl_refr_cpl = true;
        }
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {

        switch((&event)->type)
        {
            case SDL_MOUSEBUTTONUP:
                if((&event)->button.button == SDL_BUTTON_LEFT)
                    left_button_is_down = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if((&event)->button.button == SDL_BUTTON_LEFT) {
                    left_button_is_down = true;
                    last_x = (&event)->motion.x ;
                    last_y = (&event)->motion.y ;
                }
                break;
            case SDL_MOUSEMOTION:
                last_x = (&event)->motion.x ;
                last_y = (&event)->motion.y ;
                break;

            case SDL_WINDOWEVENT:
                switch((&event)->window.event)
                {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                    case SDL_WINDOWEVENT_EXPOSED:
                        SDL_UpdateTexture(texture, NULL, tft_fb, VT_WIDTH * sizeof(uint32_t));
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, texture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

bool VT_is_request_quit(void)
{
    return sdl_quit_qry;
}

void VT_deinit(void)
{
    monitor_sdl_clean_up();
    exit(0);
}


int32_t Disp0_DrawBitmap(int16_t x,int16_t y,int16_t width,int16_t height,const uint8_t *bitmap)
{
    VT_Fill_Multiple_Colors(x, y,x+width-1,y+height-1,(color_typedef*) bitmap);

    return 0;
}

void VT_sdl_flush(int32_t nMS)
{
    nMS = MAX(1, nMS);
    while(!sdl_refr_cpl) {
        SDL_Delay(nMS);
    }
    sdl_refr_cpl = false;
    sdl_refr_qry = true;
}

#if defined(_POSIX_VERSION) || defined(CLOCK_MONOTONIC) || defined(__APPLE__)
int64_t arm_2d_helper_get_system_timestamp(void)
{
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);

    return 1000000ul * timestamp.tv_sec + timestamp.tv_nsec / 1000ul;
}

uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    return 1000000ul;
}
#else

int64_t arm_2d_helper_get_system_timestamp(void)
{
    return (int64_t)clock();
}

uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    return CLOCKS_PER_SEC;
}
#endif

void VT_init(void)
{
    monitor_sdl_init();

    while(sdl_inited == false);
}

void VT_Fill_Single_Color(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > VT_WIDTH - 1) return;
    if(y1 > VT_HEIGHT - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > VT_WIDTH - 1 ? VT_WIDTH - 1 : x2;
    int32_t act_y2 = y2 > VT_HEIGHT - 1 ? VT_HEIGHT - 1 : y2;

    int32_t x;
    int32_t y;

    for(x = act_x1; x <= act_x2; x++) {
        for(y = act_y1; y <= act_y2; y++) {
            tft_fb[y * VT_WIDTH + x] = 0xff000000|DEV_2_VT_RGB(color);
        }
    }
}

void VT_Fill_Multiple_Colors(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_typedef * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > VT_WIDTH - 1) return;
    if(y1 > VT_HEIGHT - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > VT_WIDTH - 1 ? VT_WIDTH - 1 : x2;
    int32_t act_y2 = y2 > VT_HEIGHT - 1 ? VT_HEIGHT - 1 : y2;

    int32_t x;
    int32_t y;

    for(y = act_y1; y <= act_y2; y++) {
        for(x = act_x1; x <= act_x2; x++) {
            tft_fb[y * VT_WIDTH + x] = 0xff000000|DEV_2_VT_RGB(*color_p);
            color_p++;
        }

        color_p += x2 - act_x2;
    }
}

void VT_Set_Point(int32_t x, int32_t y, color_typedef color)
{
    /*Return if the area is out the screen*/
    if(x < 0) return;
    if(y < 0) return;
    if(x > VT_WIDTH - 1) return;
    if(y > VT_HEIGHT - 1) return;

    tft_fb[y * VT_WIDTH + x] = 0xff000000|DEV_2_VT_RGB(color);
}

color_typedef VT_Get_Point(int32_t x, int32_t y)
{
    uint32_t color=0;
    /*Return if the area is out the screen*/
    if(x < 0) return 0;
    if(y < 0) return 0;
    if(x > VT_WIDTH - 1) return 0;
    if(y > VT_HEIGHT - 1) return 0;

    color=tft_fb[y * VT_WIDTH + x] ;

    return VT_RGB_2_DEV(color);
}


