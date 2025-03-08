/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>TJpgDec System Configurations R0.03
// =======================
//


// <o>Input Buffer Size
//      <512=>          0.5K
//      <1024=>         1.0K
//      <2048=>         2.0K
//      <4096=>         4.0K
//      <8192=>         8.0K
// <i> Specifies size of stream input buffer.
#ifndef JD_SZBUF
#   define JD_SZBUF         512
#endif


// <o>Pixel Format
//      <0=>    RGB888(24-bit/pix)
//      <1=>    RGB565(16-bit/pix)
//      <2=>    Gray8(8-bit/pix)
// <i> Specifies output pixel format.
#ifndef JD_FORMAT
#   define JD_FORMAT   1
#endif

// <q> Enable output descaling
// <i> Switches output descaling feature.
#ifndef JD_USE_SCALE
#   define  JD_USE_SCALE    1
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