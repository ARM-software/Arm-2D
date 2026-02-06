/****************************************************************************
*  Copyright 2026 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/
#ifndef __LMSK_COMMON_H__
#define __LMSK_COMMON_H__   1

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#define ARM_LMSK_VERSION_MAJOR      1
#define ARM_LMSK_VERSION_MINOR      2
#define ARM_LMSK_VERSION            (   (ARM_LMSK_VERSION_MAJOR << 4)   \
                                    |   ARM_LMSK_VERSION_MINOR          )

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    TAG_U2_INDEX        = 0x0,
    TAG_U2_DO           = 0x0,
    TAG_U2_WHILE        = 0x0,

    TAG_U2_REPETA       = 0x1,
    TAG_S2_DELTA_SMALL  = -2,   /*! 0b10 */
    TAG_U2_DELTA_SMALL  = 0x2,
    TAG_S2_DELTA_LARGE  = -1,   /*! 0b11 */
    TAG_U2_DELTA_LARGE  = 0x3,

    TAG_U8_ALPHA        = 0xFD,
    TAG_U8_GRADIENT     = 0xF9,
};

typedef struct arm_lsmk_setting_t {
    int16_t iWidth;
    int16_t iHeight;
  
    uint8_t u3AlphaMSBCount     : 3;    /* MSB alpha bits = u3AlphaBits + 1 */
    uint8_t bRaw                : 1;    /* whether the alpha is compressed or not */
    uint8_t u2TagSetBits        : 2;    /* must be 0x00 for now, reservef for the future */
    uint8_t                     : 2;    /* must be 0x00 for now, reserved for the future */
    uint8_t chFloorCount;
} arm_lsmk_setting_t;

/* 16byte header */
typedef struct arm_lmsk_header_t {
    uint8_t chName[5];                  /* "LMSK": Losslessly compressed MaSK */
    union {
        struct {
            uint8_t u4Minor : 4;
            uint8_t u4Major : 4;            /* 0x10 for now */
        };
        uint8_t chValue;
    } Version;
    
    arm_lsmk_setting_t tSetting;

    uint32_t                    : 32;   /* reserved */
} arm_lmsk_header_t;

typedef union arm_lmsk_tag_delta_large_t {
    struct {
        int8_t  s2Tag       : 2;
        int8_t  s6Delta     : 6; 
    };
    uint8_t chByte;
} arm_lmsk_tag_delta_large_t;

typedef union arm_lmsk_tag_delta_small_t {
    struct {
        int8_t  s2Tag       : 2;
        int8_t  s3Delta0    : 3; 
        int8_t  s3Delta1    : 3; 
    };
    uint8_t chByte;
} arm_lmsk_tag_delta_small_t;

typedef union arm_lmsk_tag_repeat_t {
    struct {
        uint8_t  u2Tag      : 2;
        uint8_t  u6Repeat   : 6; 
    };
    uint8_t chByte;
} arm_lmsk_tag_repeat_t;

typedef union arm_lmsk_tag_index_t {
    struct {
        uint8_t  u2Tag      : 2;    /* TAG_U2_INDEX */
        uint8_t  u5Index    : 5;
        uint8_t  bDoWhile   : 1;    /* must be zero */
    };
    uint8_t chByte;
} arm_lmsk_tag_index_t;

/* tag: do while */
typedef union arm_lmsk_tag_dw_t {
    struct {
        uint8_t  u2Tag      : 2;    /* TAG_U2_DO / TAG_U2_WHILE */
        uint8_t  u5Index    : 5;
        uint8_t  bDoWhile   : 1;    /* must be 1 */
    };
    uint8_t chByte;
} arm_lmsk_tag_dw_t;

typedef union arm_lmsk_tag_gradient_t {
    struct {
        uint8_t chTag;                  /* Tag: TAG_U8_GRADIENT */
        uint8_t chToAlpha;
        int16_t iSteps; 
    };
    uint32_t wWord;
} arm_lmsk_tag_gradient_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#ifdef   __cplusplus
}
#endif

#endif

