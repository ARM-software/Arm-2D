/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d.h"
 * Description:  Public header file to contain the APIs for colour space
 *               conversions
 *
 * $Date:        29. April 2024
 * $Revision:    V.1.0.6
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_CONVERSION_H__
#define __ARM_2D_CONVERSION_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wnarrowing"
#elif defined(__IS_COMPILER_IAR__)
#   pragma diag_suppress=Go029 
#endif



/*============================ MACROS ========================================*/

/*! 
 *  \addtogroup Deprecated
 *  @{
 */
#define arm_2dp_convert_colour_to_rgb888   arm_2dp_convert_colour_to_cccn888
#define arm_2d_convert_colour_to_rgb888    arm_2d_convert_colour_to_cccn888
/*! @} */


/*!
 * \addtogroup gConversion 6 Conversion Operations
 * @{
 */

/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_convert_colour_to_gray8( __SRC_ADDR, /* source tile address */   \
                                        __DES_ADDR  /* target tile address */)  \
            arm_2dp_convert_colour_to_gray8(NULL,                               \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR))

#define arm_2d_tile_copy_to_gray8(  __SRC_ADDR,     /* source tile address */   \
                                    __DES_ADDR,     /* target tile address */   \
                                    __DES_REGION)   /* target region address */ \
            arm_2dp_tile_copy_to_gray8( NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_REGION))

#define arm_2d_convert_colour_to_rgb565(__SRC_ADDR, /* source tile address */   \
                                        __DES_ADDR  /* target tile address */)  \
            arm_2dp_convert_colour_to_rgb565(   NULL,                           \
                                                (__SRC_ADDR),                   \
                                                (__DES_ADDR))

#define arm_2d_tile_copy_to_rgb565( __SRC_ADDR,     /* source tile address */   \
                                    __DES_ADDR,     /* target tile address */   \
                                    __DES_REGION)   /* target region address */ \
            arm_2dp_tile_copy_to_rgb565(NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__DES_ADDR),                           \
                                        (__DES_REGION))

#define arm_2d_convert_colour_to_cccn888(__SRC_ADDR, /* source tile address */  \
                                        __DES_ADDR  /* target tile address */)  \
            arm_2dp_convert_colour_to_cccn888(  NULL,                           \
                                                (__SRC_ADDR),                   \
                                                (__DES_ADDR))

#define arm_2d_tile_copy_to_cccn888(__SRC_ADDR,     /* source tile address */   \
                                    __DES_ADDR,     /* target tile address */   \
                                    __DES_REGION)   /* target region address */ \
            arm_2dp_tile_copy_to_cccn888(   NULL,                               \
                                            (__SRC_ADDR),                       \
                                            (__DES_ADDR),                       \
                                            (__DES_REGION))

#define arm_2d_pixel_ccca8888_to_rgb565(__COLOUR)                               \
            ({__arm_2d_color_fast_rgb_t ARM_2D_SAFE_NAME(tChannels);            \
            __arm_2d_ccca8888_unpack((__COLOUR), &ARM_2D_SAFE_NAME(tChannels)); \
            __arm_2d_rgb565_pack(&ARM_2D_SAFE_NAME(tChannels));})

#define arm_2d_pixel_ccca8888_to_gray8(__COLOUR)                                \
            ({__arm_2d_color_fast_rgb_t ARM_2D_SAFE_NAME(tChannels);            \
            __arm_2d_ccca8888_unpack((__COLOUR), &ARM_2D_SAFE_NAME(tChannels)); \
            __arm_2d_gray8_pack(&ARM_2D_SAFE_NAME(tChannels));})

#define arm_2d_pixel_brga8888_to_rgb565 arm_2d_pixel_ccca8888_to_rgb565
#define arm_2d_pixel_brga8888_to_gray8 arm_2d_pixel_ccca8888_to_gray8

/*============================ TYPES =========================================*/

typedef arm_2d_op_src_t arm_2d_op_cl_convt_t;

/*! \brief 3x16-bit packed RGB color
 *         autovectorizer friendly format
 */
typedef union {
    uint16_t            BGRA[4];
    struct {
        uint16_t        B;
        uint16_t        G;
        uint16_t        R;
        uint16_t        A;
    };
} __arm_2d_color_fast_rgb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * Colour channels extraction/packing                                         *
 *----------------------------------------------------------------------------*/

/*!
 * \brief unpack a 8bit colour into a given __arm_2d_color_fast_rgb_t object
 * \param[in] wColour the target brga888 colour
 * \param[in] ptRGB a __arm_2d_color_fast_rgb_t object
 */
ARM_NONNULL(2)
__STATIC_INLINE void __arm_2d_gray8_unpack( uint8_t chColor,
                                            __arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);

    ptRGB->B = (uint16_t) chColor;
    ptRGB->G = (uint16_t) chColor;
    ptRGB->R = (uint16_t) chColor;
    ptRGB->A = (uint16_t) 0xFF;
}

/*!
 * \brief unpack a rgb565 colour into a given __arm_2d_color_fast_rgb_t object
 * \param[in] hwColour the target rgb565 colour
 * \param[in] ptRGB a __arm_2d_color_fast_rgb_t object
 */
ARM_NONNULL(2)
__STATIC_INLINE void __arm_2d_rgb565_unpack(uint16_t hwColor,
                                            __arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    /* uses explicit extraction, leading to a more efficient autovectorized code */
    uint16_t maskRunpk = 0x001f, maskGunpk = 0x003f;

    ptRGB->B = (uint16_t) ((hwColor & maskRunpk) << 3);
    ptRGB->R = (uint16_t) ((hwColor >> 11) << 3);
    ptRGB->G = (uint16_t) (((hwColor >> 5) & maskGunpk) << 2);
    
    ptRGB->A = 0xFF;
}

ARM_NONNULL(2)
__STATIC_INLINE void __arm_2d_rgb565_unpack_comp(uint16_t hwColor,
                                            __arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    /* uses explicit extraction, leading to a more efficient autovectorized code */
    uint16_t maskRunpk = 0x001f, maskGunpk = 0x003f;

    if (hwColor) {
        ptRGB->B = ((uint16_t) ((hwColor & maskRunpk) << 3)) | 0x7;
        ptRGB->R = ((uint16_t) ((hwColor >> 11) << 3)) | 0x7;
        ptRGB->G = ((uint16_t) (((hwColor >> 5) & maskGunpk)) << 2) | 0x03;
    } else {
        ptRGB->B = (uint16_t) ((hwColor & maskRunpk) << 3);
        ptRGB->R = (uint16_t) ((hwColor >> 11) << 3);
        ptRGB->G = (uint16_t) (((hwColor >> 5) & maskGunpk) << 2);
    }
    
    ptRGB->A = 0xFF;
}

/*!
 * \brief unpack a 32bit colour into a given __arm_2d_color_fast_rgb_t object
 * \param[in] wColour the target brga888 colour
 * \param[in] ptRGB a __arm_2d_color_fast_rgb_t object
 */
ARM_NONNULL(2)
__STATIC_INLINE void __arm_2d_ccca8888_unpack( uint32_t wColor,
                                            __arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);

    uint8_t *pchChannel = (uint8_t *)&wColor;

    ptRGB->B = (uint16_t) pchChannel[0];
    ptRGB->G = (uint16_t) pchChannel[1];
    ptRGB->R = (uint16_t) pchChannel[2];
    ptRGB->A = (uint16_t) pchChannel[3];
}


/*!
 * \brief generate a gray8 colour from a __arm_2d_color_fast_rgb_t object 
 * \param[in] ptRGB the target __arm_2d_color_fast_rgb_t object
 * \return uint8_t a gray8 colour
 */
ARM_NONNULL(1)
__STATIC_INLINE uint8_t __arm_2d_gray8_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    uint16_t tGrayScale = (ptRGB->R * 77 + ptRGB->G * 151 + ptRGB->B * 28) >> 8;

    return (uint8_t)(   (tGrayScale <= 255) * tGrayScale 
                    +   (tGrayScale > 255) * 255);
}


/*!
 * \brief generate a rgb565 colour from a __arm_2d_color_fast_rgb_t object 
 * \param[in] ptRGB the target __arm_2d_color_fast_rgb_t object
 * \return uint16_t a rgb565 colour
 */
ARM_NONNULL(1)
__STATIC_INLINE uint16_t __arm_2d_rgb565_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    arm_2d_color_rgb565_t tOutput = {
        .u5B = (uint16_t) (ptRGB->B >> 3),
        .u6G = (uint16_t) (ptRGB->G >> 2),
        .u5R = (uint16_t) (ptRGB->R >> 3),
    };
    return tOutput.tValue;
}

/*!
 * \brief generate a cccn888 colour from a __arm_2d_color_fast_rgb_t object 
 * \param[in] ptRGB the target __arm_2d_color_fast_rgb_t object
 * \return uint32_t a cccn888 colour
 * \note the alpha channel will be kept in the output value
 */
ARM_NONNULL(1)
__STATIC_INLINE uint32_t __arm_2d_ccca888_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    arm_2d_color_bgra8888_t tOutput = {
        .u8B = (uint16_t) ptRGB->B,
        .u8G = (uint16_t) ptRGB->G,
        .u8R = (uint16_t) ptRGB->R,
        .u8A = (uint16_t) ptRGB->A,
    };
    return tOutput.tValue;
}


/*----------------------------------------------------------------------------*
 * Colour Conversion                                                          *
 *----------------------------------------------------------------------------*/

/*!
 * \brief convert the colour format of a given tile to gray8
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_gray8(   arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget);

/*!
 * \brief copy a given tile to a gray8 target tile
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_tile_copy_to_gray8(arm_2d_op_cl_convt_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion);

/*!
 * \brief convert the colour format of a given tile to rgb565
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_rgb565(  arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget);

/*!
 * \brief copy a given tile to a rgb565 target tile
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_tile_copy_to_rgb565(   arm_2d_op_cl_convt_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*!
 * \brief convert the colour format of a given tile to cccn888
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_convert_colour_to_cccn888( arm_2d_op_cl_convt_t *ptOP,
                                                const arm_2d_tile_t *ptSource,
                                                const arm_2d_tile_t *ptTarget);

/*!
 * \brief copy a given tile to a cccn888 target tile
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptTarget the output tile (holding a buffer)
 * \param[in] ptRegion the target region, NULL means using the region of the 
 *            target tile.
 * \return arm_fsm_rt_t the operation result
 */
extern
ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_tile_copy_to_cccn888(  arm_2d_op_cl_convt_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_IAR__)
#   pragma diag_warning=Go029 
#endif

#ifdef   __cplusplus
}
#endif

#endif
