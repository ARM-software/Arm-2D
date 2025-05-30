/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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
 * Title:        #include "arm_2d_helper_font.h"
 * Description:  the font helper service header file
 *
 * $Date:        28 April 2025
 * $Revision:    V.2.11.1
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_FONT_H__
#define __ARM_2D_HELPER_FONT_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include "arm_2d_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wpadded"
#endif


/*============================ MACROS ========================================*/
#ifndef __GLCD_CFG_COLOUR_DEPTH__
#   warning Please specify the colour depth by defining the macro __GLCD_CFG_COLOUR_DEPTH__, default value 16 is used for now
#   define __GLCD_CFG_COLOUR_DEPTH__      16
#endif

/*!
 * \addtogroup Deprecated
 * @{
 */
/*! todo: COLOUR_INT_TYPE is deprecated, should be removed in the future */
#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define COLOUR_INT_TYPE         uint8_t 

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define COLOUR_INT_TYPE         uint16_t 

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define COLOUR_INT_TYPE         uint32_t 

#else
#   error Unsupported colour depth!
#endif

/*! @} */


#ifndef __GLCD_CFG_SCEEN_WIDTH__
#warning Please specify the screen width by defining the macro __GLCD_CFG_SCEEN_WIDTH__, default value 320 is used for now
#define __GLCD_CFG_SCEEN_WIDTH__                            320
#endif

#ifndef __GLCD_CFG_SCEEN_HEIGHT__
#   warning Please specify the screen height by defining the macro __GLCD_CFG_SCEEN_HEIGHT__, default value 240 is used for now
#   define __GLCD_CFG_SCEEN_HEIGHT__                        320
#endif


/*!
 * \addtogroup gHelper 8 Helper Services
 * @{
 */
/*============================ MACROFIED FUNCTIONS ===========================*/

#if 0 // TODO in the future version when string alignment feature is available
#define arm_lcd_banner_printf(__REGION, __FONT_PTR, __FORMAT_STR, ...)          \
        do {                                                                    \
            if (NULL != (__FONT_PTR)) {                                         \
                arm_lcd_text_set_font((const arm_2d_font_t *)(__FONT_PTR));     \
            }                                                                   \
            arm_2d_region_t ARM_2D_SAFE_NAME(tTargetRegion) = (__REGION);       \
            arm_2d_align_centre(                                                \
                            ARM_2D_SAFE_NAME(tTargetRegion),                    \
                            arm_lcd_get_string_line_box((__STR),(__FONT_PTR))) {\
                arm_lcd_text_set_draw_region(&__centre_region);                 \
                arm_lcd_printf(__FORMAT_STR, ##__VA_ARGS__);                    \
            }                                                                   \
        } while(0)
#endif

#define __arm_print_banner3(__STR, __REGION, __FONT_PTR)                        \
        do {                                                                    \
            if (NULL != (__FONT_PTR)) {                                         \
                arm_lcd_text_set_font((const arm_2d_font_t *)(__FONT_PTR));     \
            }                                                                   \
            arm_2d_region_t ARM_2D_SAFE_NAME(tTargetRegion) = (__REGION);       \
            arm_2d_align_centre(                                                \
                            ARM_2D_SAFE_NAME(tTargetRegion),                    \
                            arm_lcd_get_string_line_box((__STR),(__FONT_PTR))) {\
                arm_lcd_text_set_draw_region(&__centre_region);                 \
                arm_lcd_puts(__STR);                                            \
            }                                                                   \
        } while(0)

#define __arm_print_banner2(__STR, __REGION)                                    \
        __arm_print_banner3(__STR, __REGION, NULL)

#define __arm_print_banner1(__STR)                                              \
        do {                                                                    \
            arm_2d_tile_t *ARM_2D_SAFE_NAME(ptTile)                             \
                            = arm_2d_get_default_frame_buffer();                \
            arm_2d_canvas(ARM_2D_SAFE_NAME(ptTile), __banner_canvas) {          \
                __arm_print_banner3(__STR, __banner_canvas, NULL);              \
            }                                                                   \
        } while(0)

#define arm_print_banner(...)                                                   \
            ARM_CONNECT2(   __arm_print_banner,                                 \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define arm_lcd_print_banner(...)               arm_print_banner(__VA_ARGS__)                                

#define arm_lcd_get_string_line_box(__STR, ...)                                 \
            __arm_lcd_get_string_line_box(                                      \
                                (__STR),                                        \
                                (const arm_2d_font_t *)(NULL, ##__VA_ARGS__))

#define IMPL_FONT_DRAW_CHAR(__NAME)                                             \
            arm_fsm_rt_t __NAME(const arm_2d_tile_t *ptTile,                    \
                                const arm_2d_region_t *ptRegion,                \
                                const arm_2d_font_t *ptFont,                    \
                                arm_2d_tile_t *ptileChar,                       \
                                COLOUR_INT tForeColour,                         \
                                uint_fast8_t chOpacity,                         \
                                q16_t q16Scale)

#define IMPL_FONT_GET_CHAR_DESCRIPTOR(__NAME)                                   \
            arm_2d_char_descriptor_t *__NAME(                                   \
                                        const arm_2d_font_t *ptFont,            \
                                        arm_2d_char_descriptor_t *ptDescriptor, \
                                        uint8_t *pchCharCode)

/*============================ TYPES =========================================*/

typedef struct {
    arm_2d_tile_t tileChar;
    int16_t iAdvance;
    int16_t iBearingX;
    int16_t iBearingY;
    int8_t chCodeLength;
    int8_t          : 8;
} arm_2d_char_descriptor_t;

typedef struct arm_2d_font_t arm_2d_font_t;

typedef arm_2d_char_descriptor_t *arm_2d_font_get_char_descriptor_handler_t(
                                        const arm_2d_font_t *ptFont, 
                                        arm_2d_char_descriptor_t *ptDescriptor,
                                        uint8_t *pchCharCode);

typedef arm_fsm_rt_t arm_2d_font_draw_char_handler_t(
                                            const arm_2d_tile_t *ptTile,
                                            const arm_2d_region_t *ptRegion,
                                            const arm_2d_font_t *ptFont,
                                            arm_2d_tile_t *ptileChar,
                                            COLOUR_INT tForeColour,
                                            uint_fast8_t chOpacity,
                                            q16_t q16Scale);

/* Font definitions */
struct arm_2d_font_t {
    arm_2d_tile_t tileFont;
    arm_2d_size_t tCharSize;                                                    //!< CharSize
    uint32_t nCount;                                                            //!< Character count

    arm_2d_font_get_char_descriptor_handler_t *fnGetCharDescriptor;             //!< On-Get-Char-Descriptor event handler
    arm_2d_font_draw_char_handler_t           *fnDrawChar;                      //!< On-Draw-Char event handler
};

typedef struct arm_2d_char_idx_t {
    uint8_t chStartCode[4];
    uint16_t hwCount;
    uint16_t hwOffset;
} arm_2d_char_idx_t;

typedef struct arm_2d_user_font_t {
    implement(arm_2d_font_t);
    uint16_t hwCount;
    uint16_t hwDefaultCharIndex;
    arm_2d_char_idx_t tLookUpTable[];
} arm_2d_user_font_t;

typedef struct arm_2d_a1_font_t {
    implement(arm_2d_font_t);
    uint32_t nOffset;                                                           //!< Character offset
} arm_2d_a1_font_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_a1_font_t    ARM_2D_FONT_16x24;
extern const arm_2d_a1_font_t    ARM_2D_FONT_6x8;

extern
arm_2d_font_get_char_descriptor_handler_t
    ARM_2D_A1_FONT_GET_CHAR_DESCRIPTOR_HANDLER;

/*============================ PROTOTYPES ====================================*/

/*!
 * \brief return a valid code length of a given UTF8 char
 * \param[in] pchChar the start address of an UTF8 char
 * \retval -1 this isn't a legal UTF8 char
 * \retval >0 the UTF8 char length
 */
__STATIC_INLINE
ARM_NONNULL(1)
int8_t arm_2d_helper_get_utf8_byte_valid_length(const uint8_t *pchChar)
{

    switch(__CLZ( ~((uint32_t)pchChar[0] << 24) )) {
        case 0:                                     /* BYTE0: 0xxx-xxxx */
            return 1;
        case 1:
            break;
        case 2:                                     /* BYTE0: 110x-xxxx */
            if ((pchChar[1] & 0xC0) == 0x80) {      /* BYTE1: 10xx-xxxx */
                return 2;
            }
            break;
        case 3:                                     /* BYTE0: 1110-xxxx */
            if  (((pchChar[1] & 0xC0) == 0x80)      /* BYTE1: 10xx-xxxx */
            &&   ((pchChar[2] & 0xC0) == 0x80)) {   /* BYTE2: 10xx-xxxx */
                return 3;
            }
            break;
        case 4:
            if  (((pchChar[1] & 0xC0) == 0x80)      /* BYTE1: 10xx-xxxx */
            &&   ((pchChar[2] & 0xC0) == 0x80)      /* BYTE2: 10xx-xxxx */
            &&   ((pchChar[3] & 0xC0) == 0x80)) {   /* BYTE3: 10xx-xxxx */
                return 4;
            }
            break;
        default:
            break;
    }

    return -1;
}

/*!
 * \brief return the code length based on the first byte of a given UTF8 char
 * \param[in] pchChar the start address of an UTF8 char
 * \retval -1 this isn't a legal UTF8 char
 * \retval >0 the UTF8 char length
 */
__STATIC_INLINE
ARM_NONNULL(1)
int8_t arm_2d_helper_get_utf8_byte_length(const uint8_t *pchChar)
{
    switch(__CLZ( ~((uint32_t)pchChar[0] << 24) )) {
        case 0:
            return 1;
        case 1:
            break;
        case 2:
            return 2;
        case 3:
            return 3;
        case 4:
            return 4;
        default:
            break;
    }

    return -1;
}

/*!
 * \brief convert an UTF8 char into unicode char
 * 
 * \param[in] pchUTF8 
 * \return uint32_t generated unicode
 */
ARM_NONNULL(1)
extern
uint32_t arm_2d_helper_utf8_to_unicode(const uint8_t *pchUTF8);

/*!
 * \brief get char descriptor
 * \param[in] ptFont the target font
 * \param[in] ptDescriptor a buffer to store a char descriptor
 * \param[in] pchCharCode an UTF8 Char
 * \return arm_2d_char_descriptor_t * the descriptor
 */
ARM_NONNULL(1,2,3)
arm_2d_char_descriptor_t *
arm_2d_helper_get_char_descriptor(  const arm_2d_font_t *ptFont, 
                                    arm_2d_char_descriptor_t *ptDescriptor, 
                                    uint8_t *pchCharCode);

/*!
 * \brief get char advance with char spacing and scaling into consideration
 * \param[in] pchChar a UTF8 char buffer
 * \return int16_t the advance
 */
extern
ARM_NONNULL(1)
int16_t arm_lcd_get_char_advance(uint8_t *pchChar);
                            
/*!
 * \brief initialize lcd text display service
 * \param[in] ptScreen the default display area
 */
extern
void arm_lcd_text_init(arm_2d_region_t *ptScreen);

extern
int arm_lcd_printf(const char *format, ...);

/*!
 * \brief pre-printf a string to the internal text buffer and return the string box size
 * 
 * \param[in] ptFont the target front, passing NULL means using the previous font 
 * \param[in] format the printf format string
 * \param[in] ... the optional argument list 
 * \return arm_2d_size_t the string box size
 */
extern
arm_2d_size_t arm_lcd_printf_to_buffer(const arm_2d_font_t *ptFont, 
                                       const char *format,
                                       ...);

extern
ARM_NONNULL(1)
/*!
 * \brief put a char to line buffer
 * 
 * \param[in] pchChar a buffer storing an UTF8 char
 * \param[in] chUTF8Size the UTF8 size in byte
 * \return true successful
 * \return false line buffer is full
 */
bool arm_lcd_putchar_to_buffer(uint8_t *pchChar, uint_fast8_t chUTF8Size);

/*!
 * \brief print the text buffer
 * \param[in] number of chars to print. Here:
 *            0 means printing all chars in the text buffer
 *            >0 means printing a specified number of chars
 *            <0 means keeping a specified number of chars in the tail and printing the rest of the string
 */
extern
void arm_lcd_printf_buffer(int16_t iNumber);

/*!
 * \brief get the box size of the string in the text buffer
 * 
 * \return arm_2d_size_t the size of the text buffer
 */
extern
arm_2d_size_t arm_lcd_get_buffer_box_size(void);

/*!
 * \brief get the residual text length in the text buffer
 * 
 * \return size_t number of chars left
 */
extern
size_t arm_lcd_get_residual_text_length_in_buffer(void);

extern
ARM_NONNULL(1)
void arm_lcd_puts_label( const char *pchString,
                        arm_2d_align_t tAlignment);


extern
ARM_NONNULL(2)
int arm_lcd_printf_label(arm_2d_align_t tAlignment, const char *format, ...);

extern
void arm_lcd_puts(const char *str);

extern 
void arm_lcd_text_location(uint8_t chY, uint8_t chX);

extern
void arm_lcd_text_insert_line_space(int16_t iWidth);

extern
void arm_lcd_text_reset_display_region_tracking(void);

extern
arm_2d_region_t *arm_lcd_text_get_last_display_region(void);

/*!
 * \brief draw a char to a given location in the draw region
 * \param[in] iX the x coordinate
 * \param[in] iY the y coordinate
 * \param[in] ppchCharCode a pointer of pointer that points to the string
 *
 * \note this function will advance the pointer automatically
 * 
 * \param[in] chOpacity the opacity of the char
 *
 * \note this chOpacity has NO effect on the default monochrome(A1) font
 * 
 * \return int16_t char advance in pixels
 */
extern
int16_t lcd_draw_char(int16_t iX, 
                     int16_t iY, 
                     uint8_t **ppchCharCode, 
                     uint_fast8_t chOpacity);

extern
void arm_lcd_putchar(const char *str);

extern 
void arm_lcd_text_set_colour(   COLOUR_INT wForeground, 
                                COLOUR_INT wBackground);

extern
void arm_lcd_text_set_opacity(uint8_t chOpacity);

extern
void arm_lcd_text_set_scale(float fScale);

extern
int8_t arm_lcd_text_set_char_spacing(int8_t chNewSpacing);

extern
int8_t arm_lcd_text_set_line_spacing(int8_t chNewSpacing);

extern
arm_2d_size_t arm_lcd_text_get_actual_spacing(void);

extern
arm_2d_size_t arm_lcd_text_get_actual_char_size(void);

extern
arm_2d_region_t arm_lcd_text_get_char_validation_box(void);

extern
arm_2d_size_t arm_lcd_text_get_actual_char_box(void);

/*!
 * \brief Force all char use the same width
 * 
 * \param[in] bForced force or not
 * \return boolean the original configuration
 */
extern
bool arm_lcd_text_force_char_use_same_width(bool bForced);

extern 
void arm_lcd_text_set_target_framebuffer(const arm_2d_tile_t *ptFrameBuffer);

/*!
 * \brief set the display mode for characters
 * \param[in] wMode the display mode which could be the combination of following 
              enum values:
              - ARM_2D_DRW_PATN_MODE_COPY (default) - Copy without background colour
              - ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR - with background colour
              - ARM_2D_DRW_PATN_MODE_NO_FG_COLOR - without foreground colour
              - ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR - use complement value as the foreground colour
 */

extern
void arm_lcd_text_set_display_mode(uint32_t wMode);

extern 
void arm_lcd_text_set_draw_region(arm_2d_region_t *ptRegion);

extern
arm_2d_err_t arm_lcd_text_set_font(const arm_2d_font_t *ptFont);

extern
arm_2d_size_t __arm_lcd_get_string_line_box(const char *str, const arm_2d_font_t *ptFont);

extern
IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a8_font_draw_char);

extern
IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a4_font_draw_char);

extern
IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a2_font_draw_char);

extern
IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a1_font_draw_char);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef __cplusplus
}
#endif

#endif
