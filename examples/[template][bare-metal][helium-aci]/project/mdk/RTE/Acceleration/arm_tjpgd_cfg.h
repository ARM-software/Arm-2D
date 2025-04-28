/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/

#include "arm_2d_cfg.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>TJpgDec System Configurations R0.03
// =======================
//

// <o>Input Buffer Size
//      <128=>          128B
//      <256=>          256B
//      <512=>          0.5KB
//      <1024=>         1.0KB
//      <2048=>         2.0KB
//      <4096=>         4.0KB
//      <8192=>         8.0KB
// <i> Specifies size of stream input buffer.
#ifndef JD_SZBUF
#   define JD_SZBUF         128
#endif

// <q> Enable output descaling
// <i> Switches output descaling feature.
#ifndef JD_USE_SCALE
#   define  JD_USE_SCALE    0
#endif

// <q> Enable Table Conversion for Saturation Arithmetic
// <i> Use table conversion for saturation arithmetic. A bit faster, but increases 1 KB of code size.
#ifndef JD_TBLCLIP
#   define JD_TBLCLIP       1
#endif


// <o>Optimization level
//      <0=>    Basic
//      <1=>    32-bit barrel shifter
//      <2=>    Table conversion for huffman decoding
// <i>  0: Basic optimization. Suitable for 8/16-bit MCUs.
// <i>  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
// <i>  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
#ifndef JD_FASTDECODE
#   define JD_FASTDECODE    1
#endif
//</h>

// <<< end of configuration section >>>

// <o>Pixel Format
//      <0=>    RGB888(24-bit/pix)
//      <1=>    RGB565(16-bit/pix)
//      <2=>    Gray8(8-bit/pix)
// <i> Specifies output pixel format.
#undef JD_FORMAT

#if     __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define JD_FORMAT   2
#   define JD_USE_INTERNAL_32BIT_PIXEL  0
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define JD_FORMAT   1
#   define JD_USE_INTERNAL_32BIT_PIXEL  0
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define JD_FORMAT   0
#   define JD_USE_INTERNAL_32BIT_PIXEL  1
#endif

#ifndef JD_SWAP_RED_AND_BLUE
#   define JD_SWAP_RED_AND_BLUE     1
#endif

