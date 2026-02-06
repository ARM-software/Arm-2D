/*
 * Copyright (c) 2009-2026 Arm Limited. All rights reserved.
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
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "lmsk_gen.h"
#include "lmsk_encoder.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


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
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
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

system_cfg_t SYSTEM_CFG = {
    0
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void __arm_2d_impl_c8bit_copy(  uint8_t *__RESTRICT pchSourceBase,
                                int16_t iSourceStride,
                                uint8_t *__RESTRICT pchTargetBase,
                                int16_t iTargetStride,
                                arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint8_t *__RESTRICT pchSource = pchSourceBase;
        uint8_t       *__RESTRICT pchTarget = pchTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {

            *pchTarget++ = *pchSource++;
        }

        pchSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}

void __arm_2d_impl_rgb565_to_gray8( uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint16_t *__RESTRICT phwSource = phwSourceBase;
        uint8_t       *__RESTRICT pchTarget = pchTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            __arm_2d_color_fast_rgb_t      hwSrcPixel;
            uint_fast16_t hwPixel = 0;

            __arm_2d_rgb565_unpack(*phwSource++, &hwSrcPixel);

            *pchTarget++ = __arm_2d_gray8_pack(&hwSrcPixel);
        }

        phwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}

void __arm_2d_impl_cccn888_to_gray8(uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    for (int_fast16_t y = 0; y < ptCopySize->iHeight; y++) {

        const uint32_t *__RESTRICT pwSource = pwSourceBase;
        uint8_t *__RESTRICT pchTarget = pchTargetBase;

        for (int_fast16_t x = 0; x < ptCopySize->iWidth; x++) {
            arm_2d_color_bgra8888_t wSrcPixel;
            uint_fast16_t hwPixel = 0;

            wSrcPixel.tValue = *pwSource++;

            uint16_t tGrayScale = (wSrcPixel.u8R * 77 + wSrcPixel.u8G * 151 + wSrcPixel.u8B * 28) >> 8;

            *pchTarget++ = (uint8_t)(   
                                    (tGrayScale <= 255) * tGrayScale 
                                +   (tGrayScale > 255) * 255);

        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}

bool file_exists(const char *path, const char *pchMode) 
{
    FILE *fp = fopen(path, pchMode);
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

void run_os_command(const char *pchCommandLine) {
    static char s_chBuffer[4096];
    FILE *ptPipe = popen(pchCommandLine, "r");
    if (!ptPipe) {
        printf("Failed to execute command: [%s]\r\n", pchCommandLine);
        return;
    }

    while (fgets(s_chBuffer, sizeof(s_chBuffer), ptPipe) != NULL) {
        printf("%s", s_chBuffer);
    }

    pclose(ptPipe);
}

static
arm_2d_err_t process_args(int argc, char* argv[])
{
    bool bInputIsValid = true;

    if (argc <= 1) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    SYSTEM_CFG.Input.u8AlphaMSBBits = 8;

    for (int n = 0; n < argc; n++) {

        if (0 == n) {
            continue;
        }

        if (0 == strncmp(argv[n], "-p", 2)) {
            n++;
            if (n >= argc) {
                bInputIsValid = false;
                break;
            }

            if (!file_exists(argv[n], "r")) {
                printf("ERROR: Cannot find the input picture from \"%s\".", argv[n]);
                bInputIsValid = false;
                break;
            }

            SYSTEM_CFG.Input.pchInputPicturePath = argv[n];
            continue;
        }

        if (0 == strncmp(argv[n], "-o", 2)) {
            n++;
            if (n >= argc) {
                bInputIsValid = false;
                break;
            }

            if (!file_exists(argv[n], "a")) {
                printf("ERROR: Cannot find the input picture from \"%s\".", argv[n]);
                bInputIsValid = false;
                break;
            }

            SYSTEM_CFG.Input.pchOutputFilePath = argv[n];
            continue;
        }

        if (0 == strncmp(argv[n], "-a", 2)) {
            n++;
            if (n >= argc) {
                bInputIsValid = false;
                break;
            }

            int32_t iAlphaMSBBits = SDL_atoi(argv[n]);

            if (iAlphaMSBBits > 0 && iAlphaMSBBits <= 8) {
                SYSTEM_CFG.Input.u8AlphaMSBBits = iAlphaMSBBits;
            } else {
                printf("ERROR: Invalid alpha MSB bits count: %"PRIi32"\r\n", iAlphaMSBBits);
                bInputIsValid = false;
                break;
            }

            continue;
        }

        if (    (0 == strncmp(argv[n], "--help", 6)) 
            ||  (0 == strncmp(argv[n], "-h", 2))) {
            bInputIsValid = false;
            return ARM_2D_ERR_MISSING_PARAM;
        }
    }

    SYSTEM_CFG.Input.bValid = bInputIsValid;
    if (!SYSTEM_CFG.Input.bValid) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if ((NULL == SYSTEM_CFG.Input.pchInputPicturePath)) {
        SYSTEM_CFG.Input.bValid = false;
        return ARM_2D_ERR_MISSING_PARAM;
    } else if ((NULL == SYSTEM_CFG.Input.pchOutputFilePath)) {
        SYSTEM_CFG.Input.bValid = false;
        return ARM_2D_ERR_MISSING_PARAM;
    }

    return ARM_2D_ERR_NONE;
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/

int app_2d_main_thread (void *argument)
{

    return 0;
}

static void print_help(void)
{
    printf("The lmsk (v0.9.0) is a command line tool that extracts alpha masks from PNG file and generate lossless compressed mask files.  \r\n");
    printf("\r\noptions:\r\n");
    printf("\t-h, --help            show this help message and exit\r\n");
    printf("\t-p <picture path>     Input picture (*.bmp, *.png etc)\r\n");
    printf("\t-o <output file path> The file path for the compressed mask image  (*.lmsk)\r\n");
    printf("\t-a <alpha bits>       The valid alpha MSB count (1~8) \r\n");
    printf("\r\n");
}


void __ccc888_to_gray8(uint8_t *pchSrc, int16_t iPitch, uint8_t *pchDes, int16_t iWidth, int16_t iHeight)
{
    for (int16_t y = 0; y < iHeight; y++) {
        uint8_t *pchLineSrc = pchSrc;
        uint8_t *pchLineDes = pchDes;

        for (int16_t x = 0; x < iWidth; x++) {

            uint16_t tGrayScale = ( (pchLineSrc[0] * 77) 
                                  + (pchLineSrc[1] * 151) 
                                  + (pchLineSrc[2] * 28)) >> 8;

            pchLineSrc += 3;
            *pchLineDes++ = (uint8_t)(   
                                    (tGrayScale <= 255) * tGrayScale 
                                +   (tGrayScale > 255) * 255);
        }

        pchSrc += iPitch;
        pchDes += iWidth;
    }
}

void __extract_alpha_from_ccca8888(uint8_t *pchSrc, int16_t iPitch, uint8_t *pchDes, int16_t iWidth, int16_t iHeight)
{
    for (int16_t y = 0; y < iHeight; y++) {
        uint8_t *pchLineSrc = pchSrc;
        uint8_t *pchLineDes = pchDes;

        for (int16_t x = 0; x < iWidth; x++) {

            *pchLineDes++ = pchLineSrc[3];

            pchLineSrc += 4;
        }

        pchSrc += iPitch;
        pchDes += iWidth;
    }
}


static bool load_mask(const char *pchPath, arm_2d_tile_t *ptMask)
{
    memset(ptMask, 0, sizeof(arm_2d_tile_t));

    SDL_Surface *ptImage = IMG_Load(pchPath);

    do {
        if (!ptImage) {
            break;
        }
        ptMask->tRegion.tSize.iHeight = ptImage->h;
        ptMask->tRegion.tSize.iWidth = ptImage->w;
        ptMask->bIsRoot = true;
        ptMask->bHasEnforcedColour = true;
        ptMask->tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8;
        ptMask->pchBuffer = malloc( ptImage->h * ptImage->w);

        if (NULL == ptMask->pchBuffer) {
            break;
        }

        printf("Resolution: %"PRIi16"x%"PRId16"\r\n", 
                ptMask->tRegion.tSize.iWidth,
                ptMask->tRegion.tSize.iHeight);

        printf("Bits per Pixel: %d\r\n", ptImage->format->BitsPerPixel);
        
        switch (ptImage->format->BitsPerPixel) {
            case 8:
                __arm_2d_impl_c8bit_copy(   (uint8_t *)ptImage->pixels,
                                            ptImage->pitch,
                                            ptMask->pchBuffer,
                                            ptMask->tRegion.tSize.iWidth,
                                            &ptMask->tRegion.tSize);
                break;
       
            case 16:
                if (ptImage->pitch != ptMask->tRegion.tSize.iWidth * 2) {
                    return false;
                }
                printf("Convert to gray scale...\r\n");
                __arm_2d_impl_rgb565_to_gray8(  (uint16_t *)ptImage->pixels,
                                                ptMask->tRegion.tSize.iWidth,
                                                ptMask->pchBuffer,
                                                ptMask->tRegion.tSize.iWidth,
                                                &ptMask->tRegion.tSize);
                break;

            case 24:
                printf("Convert to gray scale...\r\n");
                __ccc888_to_gray8(  ptImage->pixels,
                                    ptImage->pitch,
                                    ptMask->pchBuffer,
                                    ptMask->tRegion.tSize.iWidth,
                                    ptMask->tRegion.tSize.iHeight);
                break;


            case 32:
                __extract_alpha_from_ccca8888(  ptImage->pixels,
                                                ptImage->pitch,
                                                ptMask->pchBuffer,
                                                ptMask->tRegion.tSize.iWidth,
                                                ptMask->tRegion.tSize.iHeight);
                break;

            default:
                return false;
        }

        

        SDL_FreeSurface(ptImage);

        return true;

    } while(0);

    return false;
}

int main(int argc, char* argv[])
{
    int ret = 0;

    arm_2d_err_t tResult = process_args(argc, argv);
    switch (tResult) {
        case ARM_2D_ERR_NONE:
            break;
        case ARM_2D_ERR_MISSING_PARAM:
            print_help();
            //fall-through;
        default:
            return -1;
            break;
    }


    SDL_Init(SDL_INIT_VIDEO);
    
    do {
        /* load picture */
        if (!load_mask(SYSTEM_CFG.Input.pchInputPicturePath,
                       &SYSTEM_CFG.Picture.tMask)) {
            printf("ERROR: Failed to load picture, %s\n", SDL_GetError());
            ret = -1;
            break;
        };


        printf("Encoding...\r\n");

        FILE *fp = fopen(SYSTEM_CFG.Input.pchOutputFilePath, "w");
        if (NULL == fp) {
            printf("ERROR: Failed to write file %s...\r\n", SYSTEM_CFG.Input.pchOutputFilePath);
            break;
        }
        
        arm_lmsk_encoder_t tEncoder;

        arm_lmsk_encoder_init(  &tEncoder, 
                                SYSTEM_CFG.Picture.tMask.pchBuffer, 
                                SYSTEM_CFG.Picture.tMask.tRegion.tSize.iWidth,
                                SYSTEM_CFG.Picture.tMask.tRegion.tSize.iHeight);


        int32_t iSizeWritten = arm_lmsk_write_to_file(arm_lmsk_encode(&tEncoder, SYSTEM_CFG.Input.u8AlphaMSBBits), fp);

        if (iSizeWritten < 0) {
            printf("ERROR: Failed to create file %s\r\n", SYSTEM_CFG.Input.pchOutputFilePath);
        } else {
            uint32_t wRawSize = SYSTEM_CFG.Picture.tMask.tRegion.tSize.iWidth *
                                SYSTEM_CFG.Picture.tMask.tRegion.tSize.iHeight;
            double fCompressionRate = ((double)iSizeWritten / (double)wRawSize) * 100.0f;
            printf("\r\nRaw Mask Size: %d\r\n"
                "Compressed Mask File Size: %"PRIu32" [%2.2f%%]\r\n",
                    wRawSize,
                    iSizeWritten,
                    fCompressionRate );
        }

        arm_lmsk_encoder_depose(&tEncoder);

        fclose(fp);

    } while(0);

    /* release resources */
    if (NULL != SYSTEM_CFG.Picture.tMask.pchBuffer) {
        free(SYSTEM_CFG.Picture.tMask.pchBuffer);
        SYSTEM_CFG.Picture.tMask.pchBuffer = NULL;
    }

    SDL_Quit();

    return ret;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


