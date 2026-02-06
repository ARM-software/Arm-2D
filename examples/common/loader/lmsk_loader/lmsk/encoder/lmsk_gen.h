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
#ifndef __LMSK_GEN_H__
#   define __LMSK_GEN_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/* macros for colour attributes */
#define ARM_2D_M_COLOUR_SZ_1BIT             0       //!< 1 bit:black and white
#define ARM_2D_M_COLOUR_SZ_2BIT             1       //!< 4 colours or 4 gray-levels
#define ARM_2D_M_COLOUR_SZ_4BIT             2       //!< 16 colours or 16 gray-levels
#define ARM_2D_M_COLOUR_SZ_8BIT             3       //!< 256 colours
#define ARM_2D_M_COLOUR_SZ_16BIT            4       //!< 16bits
#define ARM_2D_M_COLOUR_SZ_32BIT            5       //!< true colour
#define ARM_2D_M_COLOUR_SZ_24BIT            6       //!< true colour

#define ARM_2D_M_COLOUR_SZ_1BIT_msk         (ARM_2D_M_COLOUR_SZ_1BIT << 1)      //!< bitmask for 1bit colour formats
#define ARM_2D_M_COLOUR_SZ_2BIT_msk         (ARM_2D_M_COLOUR_SZ_2BIT << 1)      //!< bitmask for 2bit colour formats
#define ARM_2D_M_COLOUR_SZ_4BIT_msk         (ARM_2D_M_COLOUR_SZ_4BIT << 1)      //!< bitmask for 4bit colour formats
#define ARM_2D_M_COLOUR_SZ_8BIT_msk         (ARM_2D_M_COLOUR_SZ_8BIT << 1)      //!< bitmask for 8bit colour formats
#define ARM_2D_M_COLOUR_SZ_16BIT_msk        (ARM_2D_M_COLOUR_SZ_16BIT<< 1)      //!< bitmask for 16bit colour formats
#define ARM_2D_M_COLOUR_SZ_32BIT_msk        (ARM_2D_M_COLOUR_SZ_32BIT<< 1)      //!< bitmask for 32bit colour formats
#define ARM_2D_M_COLOUR_SZ_24BIT_msk        (ARM_2D_M_COLOUR_SZ_24BIT<< 1)      //!< bitmask for 24bit colour formats
#define ARM_2D_M_COLOUR_SZ_msk              (0x07 << 1),                        //!< bitmask for the SZ bitfield

#define ARM_2D_M_COLOUR_LITTLE_ENDIAN       0       //!< pixels are stored in little endian
#define ARM_2D_M_COLOUR_BIG_ENDIAN          1       //!< pixels are stored big endian

#define ARM_2D_M_COLOUR_LITTLE_ENDIAN_msk   (ARM_2D_M_COLOUR_LITTLE_ENDIAN << 4)//!< bitmask for little-endian
#define ARM_2D_M_COLOUR_BIG_ENDIAN_msk      (ARM_2D_M_COLOUR_BIG_ENDIAN    << 4)//!< bitmask for big-endian

#define ARM_2D_M_COLOUR_NO_ALPHA            0       //!< there is no alpha channel in each pixel
#define ARM_2D_M_COLOUR_HAS_ALPHA           1       //!< there is an alpha channel in each pixel

#define ARM_2D_M_COLOUR_NO_ALPHA_msk        (ARM_2D_M_COLOUR_NO_ALPHA  << 0)    //!< bitmask for no-alpha-channel-in-pixel
#define ARM_2D_M_COLOUR_HAS_ALPHA_msk       (ARM_2D_M_COLOUR_HAS_ALPHA << 0)    //!< bitmask for has-alpha-channel-in-pixel

#define ARM_2D_M_COLOUR_VARIANT_pos         5                                   //!< offset for the VARIANT bitfield
#define ARM_2D_M_COLOUR_VARIANT_msk         (0x03<<ARM_2D_M_COLOUR_VARIANT_pos) //!< bitmask for the VARIANT bitfield

/*!
 * \brief local variable decoration for pointers: restrict
 */
#ifndef __RESTRICT
#   define __RESTRICT                __restrict
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*----------------------------------------------------------------------------*
 * OOC and Private Protection                                                 *
 *----------------------------------------------------------------------------*/
/*  minimal support for OOPC */
#undef __implement_ex
#undef __implement
#undef implement
#undef implement_ex
#undef inherit
#undef inherit_ex

/*!
 * \note do NOT use this macro directly
 */
#ifdef   __cplusplus
#   define __implement_ex(__type, __name)   __type  __name
#else
#   define __implement_ex(__type, __name)                                       \
            union {                                                             \
                __type  __name;                                                 \
                __type;                                                         \
            }
#endif
/*!
 * \note do NOT use this macro
 */
#define __inherit_ex(__type, __name)   __type  __name


/*!
 * \note do NOT use this macro directly
 */
#define __implement(__type)             __implement_ex( __type,                 \
                                                        use_as__##__type)


/*!
 * \note do NOT use this macro directly
 */
#define __inherit(__type)               __inherit_ex(__type,use_as__##__type)


/*!
 * \brief inherit a given class
 * \param __type the base class, you can use .use_as_xxxxx for referencing 
 *               the base.
 * \note this macro supports microsoft extensions (-fms-extensions)
 */
#define implement(__type)               __implement(__type)

/*!
 * \brief inherit a given class and give it an alias name
 * \param __type the base class
 * \param __name an alias name for referencing the base class
 * \note this macro supports microsoft extensions (-fms-extensions)
 */
#define implement_ex(__type, __name)    __implement_ex(__type, __name)


/*!
 * \brief inherit a given class
 * \param __type the base class, you can use .use_as_xxxxx for referencing 
 *               the base.
 * \note this macro does NOT support microsoft extensions (-fms-extensions)
 */
#define inherit(__type)                 __inherit(__type)

/*!
 * \brief inherit a given class and give it an alias name
 * \param __type the base class
 * \param __name an alias name for referencing the base class
 * \note this macro does NOT support microsoft extensions (-fms-extensions)
 */
#define inherit_ex(__type, __name)      __inherit_ex(__type, __name)

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT2(__A, __B)                        __A##__B

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT2_ALT(__A, __B)                    __A##__B

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT3(__A, __B, __C)                   __A##__B##__C

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT4(__A, __B, __C, __D)              __A##__B##__C##__D

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT5(__A, __B, __C, __D, __E)         __A##__B##__C##__D##__E

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                            \
                                    __A##__B##__C##__D##__E##__F

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                       \
                                    __A##__B##__C##__D##__E##__F##__G

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                  \
                                    __A##__B##__C##__D##__E##__F##__G##__H

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)             \
                                    __A##__B##__C##__D##__E##__F##__G##__H##__I

/*! 
 * \brief connect two symbol names as one
 */
#define ARM_CONNECT2(__A, __B)                  __ARM_CONNECT2(__A, __B)

/*! 
 * \brief connect two symbol names as one
 */
#define ARM_CONNECT2_ALT(__A, __B)              __ARM_CONNECT2_ALT(__A, __B)

/*! 
 * \brief connect three symbol names as one
 */
#define ARM_CONNECT3(__A, __B, __C)             __ARM_CONNECT3(__A, __B, __C)

/*! 
 * \brief connect four symbol names as one
 */
#define ARM_CONNECT4(__A, __B, __C, __D)        __ARM_CONNECT4(__A, __B, __C, __D)

/*! 
 * \brief connect five symbol names as one
 */
#define ARM_CONNECT5(__A, __B, __C, __D, __E)                                   \
                __ARM_CONNECT5(__A, __B, __C, __D, __E)

/*! 
 * \brief connect six symbol names as one
 */
#define ARM_CONNECT6(__A, __B, __C, __D, __E, __F)                              \
                __ARM_CONNECT6(__A, __B, __C, __D, __E, __F)

/*! 
 * \brief connect seven symbol names as one
 */
#define ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)                         \
                __ARM_CONNECT7(__A, __B, __C, __D, __E, __F, __G)

/*! 
 * \brief connect eight symbol names as one
 */
#define ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)                    \
                __ARM_CONNECT8(__A, __B, __C, __D, __E, __F, __G, __H)

/*! 
 * \brief connect nine symbol names as one
 */
#define ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)               \
                __ARM_CONNECT9(__A, __B, __C, __D, __E, __F, __G, __H, __I)

/*! 
 * \brief connect up to 9 symbol names as one
 */
#define arm_connect(...)                                                        \
            ARM_CONNECT2_ALT(ARM_CONNECT, __ARM_VA_NUM_ARGS(__VA_ARGS__))       \
                (__VA_ARGS__)

/*! 
 * \brief connect up to 9 symbol names as one
 */
#define ARM_CONNECT(...)        arm_connect(__VA_ARGS__)

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING1(__declare)                                                 \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING2(__declare, __on_leave_expr)                                \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL;           \
                 (__on_leave_expr)                                              \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING3(__declare, __on_enter_expr, __on_leave_expr)               \
            for (__declare, *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;  \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )

/*!
 * \note do NOT use this macro directly
 */
#define __ARM_USING4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)          \
            for (__dcl1,__dcl2,*ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)= NULL;\
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ((__on_enter_expr),1) : 0;                                  \
                 (__on_leave_expr)                                              \
                )

/*!
 * \brief create a code segment with up to two local variables and 
 *        entering/leaving operations
 * \note prototype 1
 *       arm_using(local variable declaration) {
 *           code body 
 *       }
 * 
 * \note prototype 2
 *       arm_using(local variable declaration, {code segment before leaving the body}) {
 *           code body
 *       }
 *
 * \note prototype 3
 *       arm_using( local variable declaration, 
 *                 {code segment before entering the body},
 *                 {code segment before leaving the body}
 *                 ) {
 *           code body 
 *       }
 *
 * \note prototype 4
 *       arm_using( local variable1 declaration,
                    local variable2 with the same type as the local variable 1,
 *                 {code segment before entering the body},
 *                 {code segment before leaving the body}
 *                 ) {
 *           code body 
 *       }
 */
#define arm_using(...)                                                          \
            ARM_CONNECT2(__ARM_USING, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*! 
 * \brief get the number of items in an given array
 */
#ifndef dimof
#   define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))
#endif

#undef arm_irq_safe
#undef arm_exit_irq_safe
#define arm_irq_safe  arm_using( uint32_t ARM_2D_SAFE_NAME(temp) = 0,         \
                                 {  ARM_2D_UNUSED(ARM_2D_SAFE_NAME(temp));    \
                                    VT_enter_global_mutex();},                \
                                 {  VT_leave_global_mutex();} )
#define arm_exit_irq_safe    continue

/*!
 * \brief A macro to generate a safe name, usually used in macro template as the 
 *        name of local variables
 * 
 */
#define ARM_2D_SAFE_NAME(...)    ARM_CONNECT(__,__LINE__,##__VA_ARGS__)

/*============================ TYPES =========================================*/

/*!
 * \brief the error code for arm-2d (minimal integer: int8_t)
 *
 */
typedef enum {
    ARM_2D_ERR_INVALID_STATUS           = -13,  //!< the target service is in an invalid status for an API
    ARM_2D_ERR_NOT_AVAILABLE            = -12,  //!< service is not available or not initialissed
    ARM_2D_ERR_UNSUPPORTED_COLOUR       = -11,  //!< the specified colour is not supported
    ARM_2D_ERR_BUSY                     = -10,  //!< service is busy
    ARM_2D_ERR_INSUFFICIENT_RESOURCE    = -9,   //!< insufficient resource
    ARM_2D_ERR_IO_BUSY                  = -8,   //!< HW accelerator is busy
    ARM_2D_ERR_IO_ERROR                 = -7,   //!< Generic HW error
    ARM_2D_ERR_MISSING_PARAM            = -6,   //!< missing mandatory parameter
    ARM_2D_ERR_INVALID_OP               = -5,   //!< unsupported / invalid operation
    ARM_2D_ERR_NOT_SUPPORT              = -4,   //!< feature/service/operation is not supported
    ARM_2D_ERR_OUT_OF_REGION            = -3,   //!< the operation is out of target area
    ARM_2D_ERR_INVALID_PARAM            = -2,   //!< invalid parameter
    ARM_2D_ERR_UNKNOWN                  = -1,   //!< generic or unknown errors
    ARM_2D_ERR_NONE                     = 0,    //!< no error
    ARM_2D_RT_FALSE                     = 0,    //!< false
    ARM_2D_RT_TRUE                      = 1,    //!< true
    //ARM_2D_RT_FRAME_SKIPPED             = __arm_fsm_rt_last,    //!< frame is skipped
    //ARM_2D_RT_PFB_USER_DRAW,
} arm_2d_err_t;

/*!
 * \brief enumerations for colour attributes
 */
enum {
    ARM_2D_COLOUR_SZ_1BIT = 0,            //!< 1 bit:black and white
    ARM_2D_COLOUR_SZ_2BIT = 1,            //!< 4 colours or 4 gray-levels
    ARM_2D_COLOUR_SZ_4BIT = 2,            //!< 16 colours or 16 gray-levels
    ARM_2D_COLOUR_SZ_8BIT = 3,            //!< 256 colours
    ARM_2D_COLOUR_SZ_16BIT = 4,           //!< 16bits
    ARM_2D_COLOUR_SZ_32BIT = 5,           //!< true colour (32bit)
    ARM_2D_COLOUR_SZ_24BIT = 6,           //!< true colour (24bit)

    ARM_2D_COLOUR_SZ_1BIT_msk   =   ARM_2D_COLOUR_SZ_1BIT << 1,
    ARM_2D_COLOUR_SZ_2BIT_msk   =   ARM_2D_COLOUR_SZ_2BIT << 1,
    ARM_2D_COLOUR_SZ_4BIT_msk   =   ARM_2D_COLOUR_SZ_4BIT << 1,
    ARM_2D_COLOUR_SZ_8BIT_msk   =   ARM_2D_COLOUR_SZ_8BIT << 1,
    ARM_2D_COLOUR_SZ_16BIT_msk  =   ARM_2D_COLOUR_SZ_16BIT<< 1,
    ARM_2D_COLOUR_SZ_32BIT_msk  =   ARM_2D_COLOUR_SZ_32BIT<< 1,
    ARM_2D_COLOUR_SZ_24BIT_msk  =   ARM_2D_COLOUR_SZ_24BIT<< 1,
    ARM_2D_COLOUR_SZ_pos        =   1,
    ARM_2D_COLOUR_SZ_msk        =   (0x07 << ARM_2D_COLOUR_SZ_pos),
    

    ARM_2D_COLOUR_LITTLE_ENDIAN       = 0,
    ARM_2D_COLOUR_BIG_ENDIAN          = 1,

    ARM_2D_COLOUR_LITTLE_ENDIAN_msk   = ARM_2D_COLOUR_LITTLE_ENDIAN << 4,
    ARM_2D_COLOUR_BIG_ENDIAN_msk      = ARM_2D_COLOUR_BIG_ENDIAN    << 4,

    ARM_2D_COLOUR_NO_ALPHA = 0,
    ARM_2D_COLOUR_HAS_ALPHA = 1,

    ARM_2D_COLOUR_NO_ALPHA_msk        = ARM_2D_COLOUR_NO_ALPHA      << 0,
    ARM_2D_COLOUR_HAS_ALPHA_msk       = ARM_2D_COLOUR_HAS_ALPHA     << 0,

    ARM_2D_COLOUR_VARIANT_pos = 5,
    ARM_2D_COLOUR_VARIANT_msk         = 0x03 << ARM_2D_COLOUR_VARIANT_pos,
};

/*!
 * \brief enumerations for colour types
 *
 */
enum {
    ARM_2D_COLOUR_MONOCHROME  =   ARM_2D_COLOUR_SZ_1BIT_msk | ARM_2D_COLOUR_VARIANT_msk,
    ARM_2D_COLOUR_BIN         =   ARM_2D_COLOUR_MONOCHROME,
    ARM_2D_COLOUR_1BIT        =   ARM_2D_COLOUR_MONOCHROME,

    ARM_2D_COLOUR_MASK_A1     =   ARM_2D_COLOUR_MONOCHROME,
    ARM_2D_COLOUR_MASK_A2     =   ARM_2D_M_COLOUR_SZ_2BIT_msk,
    ARM_2D_COLOUR_MASK_A4     =   ARM_2D_M_COLOUR_SZ_4BIT_msk,

    ARM_2D_COLOUR_2BIT        =   ARM_2D_M_COLOUR_SZ_2BIT_msk,
    ARM_2D_COLOUR_4BIT        =   ARM_2D_M_COLOUR_SZ_4BIT_msk,

    ARM_2D_COLOUR_8BIT        =   ARM_2D_COLOUR_SZ_8BIT_msk,
    ARM_2D_COLOUR_GRAY8       =   ARM_2D_COLOUR_SZ_8BIT_msk,
    ARM_2D_COLOUR_MASK_A8     =   ARM_2D_COLOUR_SZ_8BIT_msk | ARM_2D_COLOUR_VARIANT_msk,

    ARM_2D_COLOUR_16BIT       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB16       =   ARM_2D_COLOUR_SZ_16BIT_msk,
    ARM_2D_COLOUR_RGB565      =   ARM_2D_COLOUR_RGB16,

/*  won't support
    ARM_2D_COLOUR_RGB565_BE   =   ARM_2D_COLOUR_SZ_16BIT_msk        |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
 */

    ARM_2D_COLOUR_24BIT       =   ARM_2D_COLOUR_SZ_24BIT_msk        ,   /* not supported yet */
    ARM_2D_COLOUR_RGB24       =   ARM_2D_COLOUR_SZ_24BIT_msk        ,   /* not supported yet */

    ARM_2D_COLOUR_32BIT       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,
    ARM_2D_COLOUR_RGB32       =   ARM_2D_COLOUR_SZ_32BIT_msk        ,

    ARM_2D_COLOUR_CCCN888     =   ARM_2D_COLOUR_RGB32               ,
    ARM_2D_COLOUR_CCCA8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       ,

    ARM_2D_COLOUR_RGB888      =   ARM_2D_COLOUR_CCCN888             ,
    ARM_2D_COLOUR_BGRA8888    =   ARM_2D_COLOUR_CCCA8888            ,

/* not supported yet
    ARM_2D_COLOUR_NCCC888     =   ARM_2D_COLOUR_RGB32               |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
    ARM_2D_COLOUR_ACCC8888    =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       |
                                  ARM_2D_COLOUR_BIG_ENDIAN_msk      ,
*/

    ARM_2D_CHANNEL_8in32      =   ARM_2D_COLOUR_SZ_32BIT_msk        |
                                  ARM_2D_COLOUR_HAS_ALPHA_msk       |
                                  ARM_2D_COLOUR_VARIANT_msk   ,
};

/*!
 * \brief the colour type for gray8 (8bit gray scale)
 *
 */
typedef struct arm_2d_color_gray8_t {
    uint8_t tValue;
} arm_2d_color_gray8_t;

/*!
 * \brief the colour type for rgb565
 *
 */
typedef union arm_2d_color_rgb565_t {
    uint16_t tValue;
    struct {
        uint16_t u5B : 5;
        uint16_t u6G : 6;
        uint16_t u5R : 5;
    };
} arm_2d_color_rgb565_t;

/*!
 * \brief the colour type for brga8888
 *
 * \details In most cases four equal-sized pieces of adjacent memory are used,
 *          one for each channel, and a 0 in a channel indicates black color or
 *          transparent alpha, while all-1 bits indicates white or fully opaque
 *          alpha. By far the most common format is to store 8 bits (one byte)
 *          for each channel, which is 32 bits for each pixel.
 *
 *          (source: https://en.wikipedia.org/wiki/RGBA_color_model#ARGB32)
 */
typedef union arm_2d_color_bgra8888_t {
    uint32_t tValue;
    uint8_t chChannel[4];
    struct {
        uint32_t u8B : 8;
        uint32_t u8G : 8;
        uint32_t u8R : 8;
        uint32_t u8A : 8;
    };
} arm_2d_color_bgra8888_t;

/*!
 * \brief a type used as colour descriptor
 *
 */
typedef union {
    struct {
        uint8_t bHasAlpha       : 1;     //!< whether the target colour has alpha channel
        uint8_t u3ColourSZ      : 3;     //!< the size of the colour
        uint8_t bBigEndian      : 1;     //!< whether the colour is stored in big endian
        uint8_t u2Variant       : 2;
        uint8_t                 : 1;
    };
    struct {
        uint8_t u7ColourFormat  : 7;
        uint8_t                 : 1;
    };
    uint8_t chScheme;
} arm_2d_color_info_t;

typedef union {
    uint16_t BGRA[4];
    struct {
        uint16_t B;
        uint16_t G;
        uint16_t R;
        uint16_t A;
    };
} __arm_2d_color_fast_rgb_t;

/*!
 * \brief a type for the size of an rectangular area
 *
 */
typedef struct arm_2d_size_t {
    int16_t iWidth;                     //!< width of an rectangular area
    int16_t iHeight;                    //!< height of an rectangular area
} arm_2d_size_t;

/*!
 * \brief a type for coordinates (integer)
 *
 */
typedef struct arm_2d_location_t {
    int16_t iX;                         //!< x in Cartesian coordinate system
    int16_t iY;                         //!< y in Cartesian coordinate system
} arm_2d_location_t;

/*!
 * \brief a type for an rectangular area
 *
 */
typedef struct arm_2d_region_t {
    implement_ex(arm_2d_location_t, tLocation); //!< the location (top-left corner)
    implement_ex(arm_2d_size_t, tSize);         //!< the size
} arm_2d_region_t;

/*!
 * \brief a type for tile
 *
 */
typedef struct arm_2d_tile_t arm_2d_tile_t;
struct arm_2d_tile_t {
    implement_ex(struct {
        uint8_t    bIsRoot              : 1;                                    //!< is this tile a root tile
        uint8_t    bHasEnforcedColour   : 1;                                    //!< does this tile contains enforced colour info
        uint8_t    bDerivedResource     : 1;                                    //!< indicate whether this is a derived resources (when bIsRoot == 0)
        uint8_t    bVirtualResource     : 1;                                    //!< indicate whether the resource should be loaded on-demand
        uint8_t    bVirtualScreen       : 1;                                    //!< DO NOT USE! indicate whether the tile is considered as the virtual screen, it is used in dirty region calculation
        uint8_t    u3ExtensionID        : 3;                                    //!< Tile Extension ID
        arm_2d_color_info_t    tColourInfo;                                     //!< enforced colour

        union {
            uint16_t                    : 16;
            struct {
                uint8_t bIsNewFrame     : 1;
                uint8_t bIsDryRun       : 1;
            }PFB;
            struct {
                int16_t iTargetStride;
            }VRES;
        } Extension;
        
    }, tInfo);

    implement_ex(arm_2d_region_t, tRegion);                                     //!< the region of the tile

    union {
        /* when bIsRoot is true, phwBuffer is available,
         * otherwise ptParent is available
         */
        arm_2d_tile_t       *ptParent;                                          //!< a pointer points to the parent tile
        uint8_t             *pchBuffer;                                         //!< a pointer points to a buffer in a 8bit colour type
        uint16_t            *phwBuffer;                                         //!< a pointer points to a buffer in a 16bit colour type
        uint32_t            *pwBuffer;                                          //!< a pointer points to a buffer in a 32bit colour type

        intptr_t            nAddress;                                           //!< a pointer in integer
    };
};


typedef struct system_cfg_t {
    struct {
        char *pchInputPicturePath;
        char *pchOutputFilePath;

        uint32_t bValid                 : 1;
        uint32_t                        : 7;
        uint32_t u8AlphaMSBBits         : 8;
        uint32_t                        : 16;
    } Input;

    struct {
        arm_2d_tile_t tMask;
    } Picture;

} system_cfg_t;


/*============================ GLOBAL VARIABLES ==============================*/
extern system_cfg_t SYSTEM_CFG;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
/*!
 * \brief unpack a rgb565 colour into a given __arm_2d_color_fast_rgb_t object
 * \param[in] hwColour the target rgb565 colour
 * \param[in] ptRGB a __arm_2d_color_fast_rgb_t object
 */

static inline void __arm_2d_rgb565_unpack(uint16_t hwColor,
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

/*!
 * \brief generate a gray8 colour from a __arm_2d_color_fast_rgb_t object 
 * \param[in] ptRGB the target __arm_2d_color_fast_rgb_t object
 * \return uint8_t a gray8 colour
 */
static inline uint8_t __arm_2d_gray8_pack(__arm_2d_color_fast_rgb_t * ptRGB)
{
    assert(NULL != ptRGB);
    
    uint16_t tGrayScale = (ptRGB->R * 77 + ptRGB->G * 151 + ptRGB->B * 28) >> 8;

    return (uint8_t)(   (tGrayScale <= 255) * tGrayScale 
                    +   (tGrayScale > 255) * 255);
}

#ifdef   __cplusplus
}
#endif

#endif

