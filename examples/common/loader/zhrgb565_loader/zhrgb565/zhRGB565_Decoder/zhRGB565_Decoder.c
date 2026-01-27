#include "zhRGB565_Decoder.h"

#if defined(RTE_Acceleration_Arm_2D_Extra_Loader)
#   define __ZHRGB565_LOADER_IMPLEMENT__
#   include "arm_2d_example_loaders.h"
#endif

#if defined(RTE_Acceleration_Arm_2D)
__STATIC_FORCEINLINE
void zhRGB565_save_pixel(COLOUR_INT *pPixel, uint16_t hwRGB565Pixel)
{
#if __GLCD_CFG_COLOUR_DEPTH__ == 16
    *pPixel = hwRGB565Pixel;
#else
    __arm_2d_color_fast_rgb_t tPixel;
    __arm_2d_rgb565_unpack(hwRGB565Pixel, &tPixel);

#    if __GLCD_CFG_COLOUR_DEPTH__ == 8
    *pPixel = __arm_2d_gray8_pack(&tPixel);
#    elif __GLCD_CFG_COLOUR_DEPTH__ == 32
    *pPixel = __arm_2d_ccca8888_pack(&tPixel);
#endif
#endif
}

/**
 * ref_color：reference color
 * encoded_num：repeat count
 * pixel：pointer to store the decoded pixel
 */
__STATIC_INLINE
void zhRGB565_RLE_decoder(uint16_t ref_color, uint16_t encoder_num, COLOUR_INT *pixel)
{
    for(uint32_t i=0;i<encoder_num; i++){
        zhRGB565_save_pixel(pixel++, ref_color);
    }
}

#else

/**
 * ref_color：reference color
 * encoded_num：repeat count
 * pixel：pointer to store the decoded pixel
 */
static inline void zhRGB565_RLE_decoder(uint16_t ref_color, uint16_t encoder_num, uint16_t *pixel)
{
    for(uint32_t i=0;i<encoder_num; i++){
        *(pixel) = ref_color;
        pixel++;
    }
}

#endif

/**
 * ref_color：reference color
 * encoderDATA： encoded data
 * pixel1,pixel2：decoded pixels
 */
static inline void zhRGB565_DIFF_decoder(uint16_t ref_color, uint16_t encoderDATA, uint16_t *pixel1, uint16_t *pixel2)
{
    uint8_t tmpdataH = encoderDATA>>8, tmpdataL = encoderDATA;
    uint16_t HI = GET_RGB332_TO_RGB565(tmpdataH);
    uint16_t LO = GET_RGB332_TO_RGB565(tmpdataL);
    
    *pixel1 = ref_color ^ HI;
    *pixel2 = *pixel1 ^ LO;
}

#if !defined(RTE_Acceleration_Arm_2D_Extra_Loader)
// x0,y0:(x0,y0) top-left coordinates of the selected area in the image
// width，height：dimensions of the region to extract from the image
// src：compressed data
// buf：decompression buffer
void zhRGB565_decompress_baseversion(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height,const uint16_t *src, uint16_t *buf)
{
    uint16_t pic_width = GET_RGB565_ENCODER_WIDTH(src);
    uint16_t pic_height = GET_RGB565_ENCODER_HEIGHT(src);
    uint16_t ecoder_rle_Flag = GET_RGB565_ENCODER_FLAG(src);
	// The high-byte flag is identical for both delta and RLE encoding; however, 
	// delta encoding produces far fewer data, so it never uses the “long-code” variant.
    uint16_t ecoder_diff_Flag = GET_RGB565_ENCODER_FLAG(src)|0x0080;
    uint32_t sjb_length = GET_RGB565_ENCODER_SJB_LENGHT(src);               // upgrade table length
    uint32_t encoder_addr = GET_RGB565_ENCODER_DATA_ADDR(src);              // Starting coordinates of encoded data
    
    uint16_t pic_line, pic_col, i;
    uint32_t line_pos_base = 0;
    uint32_t real_width;
    uint32_t x00, line_addr;
    
    uint16_t ref_color, len, pixl_len;
    
    // Boundary check
    if (x0 >= pic_width || y0 >= pic_height) return;
    if (x0 + width > pic_width) width = pic_width - x0;
    if (y0 + height > pic_height) height = pic_height - y0;

    for(pic_line=y0; pic_line<(y0+height); pic_line++)
    {
        /************************* Process upgrade table *************************/
        if(sjb_length != 0){        // upgrade table present
            for(i=0; i<sjb_length; i++){
                if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i))
                {
                    line_pos_base = 0;
                }
                else
                {
                    line_pos_base = 65536*(i+1);
                    if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i+1))
                        break;
                }
            }
        }
        /********************** upgrade table processing complete **********************/
        
        // Pre-fetch the start offset of the specified row in the encoded data
        line_addr = encoder_addr + GET_RGB565_ENCODER_LINE_DATA_ADDR(src, pic_line) + line_pos_base;
        
        x00 = 0;
        real_width = width;        // Actual image width
        for(pic_col = 0; pic_col<pic_width; pic_col++)
        {
            uint16_t EncodeData = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
            uint16_t tmpdata = EncodeData & 0xff80;
            
            if(tmpdata == ecoder_rle_Flag)
            {
                pic_col++;        // Point to ref_color
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if (EncodeData == ecoder_rle_Flag)        // Long-code encoding
                {
                    pic_col++;        // Point to length data
                    pixl_len = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);        // Read RLE-encoded length
                }
                else     // short-code encoding
                {
                    pixl_len = EncodeData & 0x007f;        // Fetch encoded length
                }

                if(x00 < x0){        // x00 Cursor is to the left of the target start
                    if (x00 + pixl_len < x0)    // x00 Cursor still left of target after adding decode length, skip
                    {
                        x00 += pixl_len;
                        continue;               // Continue to fetch next data segment
                    }
                    else
                    {
                        pixl_len = x00 + pixl_len - x0;     // Valid length starting from x0 coordinate
                        x00 = x0 + pixl_len;
                    }
                }
                
                if (real_width > pixl_len)    // Required pixel length exceeds encoded length, decode entire run
                {
                    zhRGB565_RLE_decoder(ref_color, pixl_len, buf);
                    buf += pixl_len;
                    real_width -= pixl_len;
                }
                else
                {// Remaining pixels < encoded run length; decode only the leftover width
                    zhRGB565_RLE_decoder(ref_color, real_width, buf);
                    buf += real_width;
                    break;
                }
            }
            else if(tmpdata == ecoder_diff_Flag)    // Delta encoding flag
            {
                len = EncodeData & 0x007f;         // Fetch encoded length; delta encoding uses short-length format
                pixl_len = len*2 + 1;              // Total decompressed pixel length
                
                uint16_t real_pixl_len = pixl_len;
                uint16_t skip_cnt = 0;
                
                if(x00 < x0)        // x00 Cursor is to the left of the target start
                {    
                    if (x00 + pixl_len < x0)    // x00 Cursor is to the left of the target start
                    {
                        x00 += pixl_len;
                        pic_col += len + 1;
                        continue;    
                    }
                    else    // If fully decoded, length can reach the start coordinate
                    {
						// Calculate new coordinate and decode length
                        real_pixl_len = x00 + pixl_len - x0;
                        x00 = x0 + real_pixl_len;
                        skip_cnt  = pixl_len - real_pixl_len;    // Number of leading pixels to skip
                    }
                }

                uint16_t pix1, pix2, encdata;
                pic_col++;        // Point to ref_color
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if(skip_cnt == 0){        // No skip needed, decode directly
                    *buf++ = ref_color;        // Reference color stored as-is, retrieved as-is
                    real_width--;
                    // Decoding of current line finished, switch to next line
                    if(real_width == 0){ break; }
                }
                else
                    skip_cnt--;
                
                for(uint16_t i=0; i<len; i++)
                {
                    pic_col++;        // Point to encoded data
                    encdata = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                    zhRGB565_DIFF_decoder(ref_color, encdata, &pix1, &pix2);
                    
                    if(skip_cnt == 0){        // No skip needed, decode directly
                        *buf++ = pix1;        // First pixel
                        real_width--;
                        // Decoding of current line finished, switch to next line
                        if(real_width == 0){ break; }
                    }
                    else
                        skip_cnt--;
                    
                    if(skip_cnt == 0){        // No skip needed, decode directly
                        *buf++ = pix2;        // Second pixel
                        real_width--;
                        // Decoding of current line finished, switch to next line
                        if(real_width == 0){ break; }
                    }
                    else
                        skip_cnt--;
                    // The last decoded pixel serves as the reference color for the next encoded data
                    ref_color = pix2;
                }
                            
                // Buffer line break
                if(real_width == 0){ break; }
            }
            else
            {
                if (x00 < x0)
                {
                    x00++;
                    continue;
                }
                *buf++ = EncodeData;
                real_width--;
                // Buffer line break
                if(real_width == 0){ break; }
            }
        }
    }
}
#endif

#if defined(RTE_Acceleration_Arm_2D)

#if defined(RTE_Acceleration_Arm_2D_Extra_Loader) && __ARM_2D_ZHRGB565_USE_LOADER_IO__
#undef GET_RGB565_ENCODER_WIDTH
#undef GET_RGB565_ENCODER_HEIGHT
#undef GET_RGB565_ENCODER_FLAG
#undef GET_RGB565_ENCODER_SJB_LENGHT
#undef GET_RGB565_ENCODER_LINE_POS
#undef GET_RGB565_ENCODER_DATA_ADDR
#undef GET_RGB565_ENCODER_SJB_DATA
#undef GET_RGB565_ENCODER_LINE_DATA_ADDR
#undef GET_RGB565_ENCODER_LINE_DATA
#undef GET_RGB565_ENCODER_LINE_DATA2

__STATIC_FORCEINLINE
uint16_t zhRGB565_read_u16_from_loader(uintptr_t ptLoader)
{
    uint16_t hwData;
    arm_generic_loader_io_read( (arm_generic_loader_t *)ptLoader, 
                                (uint8_t *)&hwData, 2);

    return hwData;
}

__STATIC_FORCEINLINE
uint16_t zhRGB565_read_u16_from_loader_with_offset(uintptr_t ptLoader, uintptr_t nOffset)
{
    arm_generic_loader_io_seek( (arm_generic_loader_t *)ptLoader, 
                                nOffset * sizeof(uint16_t), 
                                SEEK_SET);
    return zhRGB565_read_u16_from_loader(ptLoader);
}

__STATIC_FORCEINLINE
void zhRGB565_set_loader_offset(uintptr_t ptLoader, uintptr_t nOffset)
{
    arm_generic_loader_io_seek( (arm_generic_loader_t *)ptLoader, 
                                nOffset * sizeof(uint16_t), 
                                SEEK_SET);
}

/* Get image width */
//#define     GET_RGB565_ENCODER_WIDTH(BUF)                ((BUF)[0])
#define GET_RGB565_ENCODER_WIDTH(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 0)

/* Get image height */
//#define     GET_RGB565_ENCODER_HEIGHT(BUF)                ((BUF)[1])
#define GET_RGB565_ENCODER_HEIGHT(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 1)

/* Get image encoding flag */
//#define     GET_RGB565_ENCODER_FLAG(BUF)                ((BUF)[2])
#define GET_RGB565_ENCODER_FLAG(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 2)

/* Get image upgrade table length */
//#define     GET_RGB565_ENCODER_SJB_LENGHT(BUF)          ((BUF)[3])
#define GET_RGB565_ENCODER_SJB_LENGHT(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 3)

/* Get image row-offset table start address */
//#define     GET_RGB565_ENCODER_LINE_POS(BUF)            ((BUF)[4])
#define GET_RGB565_ENCODER_LINE_POS(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 4)

/* Get image encoded data start address */
//#define     GET_RGB565_ENCODER_DATA_ADDR(BUF)            ((BUF)[5])
#define GET_RGB565_ENCODER_DATA_ADDR(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 5)

/* Get image row index of the N-th entry in upgrade table, N = 0,1,2... */
//#define     GET_RGB565_ENCODER_SJB_DATA(BUF, N)            ((BUF)[6+N])
#define GET_RGB565_ENCODER_SJB_DATA(__addr, __index)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 6 + (__index))

/* Get image address of N-th row's encoded data in row-offset table, N = 0,1,2... */
//#define     GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N)    ((BUF)[GET_RGB565_ENCODER_LINE_POS(BUF) + (N)])
#define GET_RGB565_ENCODER_LINE_DATA_ADDR(__addr, __index)                      \
        zhRGB565_read_u16_from_loader_with_offset(                              \
            (uintptr_t)(__addr),                                                \
            GET_RGB565_ENCODER_LINE_POS(__addr) + (__index))


/* Get image M-th data of N-th row's encoded data, N = 0,1,2... */
//#define     GET_RGB565_ENCODER_LINE_DATA(BUF,N,M)        ((BUF)[GET_RGB565_ENCODER_DATA_ADDR(BUF) + GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N) + (M)])
#define GET_RGB565_ENCODER_LINE_DATA(__addr, N, M)                              \
        zhRGB565_read_u16_from_loader_with_offset(                              \
            (uintptr_t)(__addr),                                                \
			GET_RGB565_ENCODER_DATA_ADDR(__addr) +                              \
            GET_RGB565_ENCODER_LINE_DATA_ADDR(__addr, (N)) + (M))

//#define     GET_RGB565_ENCODER_LINE_DATA2(BUF,N,M)        ((BUF)[(N) + (M)])
#define GET_RGB565_ENCODER_LINE_DATA2(__addr, N, M)                             \
        zhRGB565_read_u16_from_loader((uintptr_t)(__addr))

#define SET_RGB565_ENCODER_READ_HINT(__addr, __index)                           \
        zhRGB565_set_loader_offset((uintptr_t)(__addr), __index)
#else

#   define SET_RGB565_ENCODER_READ_HINT(...)

#endif

ARM_NONNULL(1)
arm_2d_size_t zhRGB565_get_image_size(arm_generic_loader_t *ptLoader)
{

    assert(NULL != ptLoader);
#if __ARM_2D_ZHRGB565_USE_LOADER_IO__
    arm_2d_size_t tSize = {
        .iWidth = GET_RGB565_ENCODER_WIDTH((const uint16_t *)ptLoader),
        .iHeight = GET_RGB565_ENCODER_HEIGHT((const uint16_t *)ptLoader),
    };
#else
    arm_generic_loader_io_seek(	ptLoader, 
                                0, 
                                SEEK_SET);

    arm_zhrgb565_loader_t *ptThis = (arm_zhrgb565_loader_t *)ptLoader;
    const uint16_t *phwSrc = this.phwLocalSource;

    arm_2d_size_t tSize = {
        .iWidth = GET_RGB565_ENCODER_WIDTH(phwSrc),
        .iHeight = GET_RGB565_ENCODER_HEIGHT(phwSrc),
    };

#endif
    return tSize;
}

// x0,y0:(x0,y0) top-left coordinates of the selected area in the image
// width，height：dimensions of the region to extract from the image
// src：compressed data
// buf：decompression buffer
void zhRGB565_decompress_for_arm2d(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, const uint16_t *src, COLOUR_INT *buf, int16_t iTargetStride)
{
    uint16_t pic_width = GET_RGB565_ENCODER_WIDTH(src);
    uint16_t pic_height = GET_RGB565_ENCODER_HEIGHT(src);
    uint16_t ecoder_rle_Flag = GET_RGB565_ENCODER_FLAG(src);
	// The high-byte flag is identical for both delta and RLE encoding; however, 
	// delta encoding produces far fewer data, so it never uses the “long-code” variant.
    uint16_t ecoder_diff_Flag = ecoder_rle_Flag |0x0080;
    uint32_t sjb_length = GET_RGB565_ENCODER_SJB_LENGHT(src);           // upgrade table length
    uint32_t encoder_addr = GET_RGB565_ENCODER_DATA_ADDR(src);    	    // Starting coordinates of encoded data
    
    uint16_t pic_line, pic_col;
    uint32_t line_pos_base = 0, real_width;
    uint32_t x00, line_addr;
    
    COLOUR_INT *buf_base;
    buf_base = (COLOUR_INT *)buf;
    
    uint16_t g_sjb_cur = 0;
    uint16_t ref_color, len, pixl_len;
    
    // Boundary check
    if (x0 >= pic_width || y0 >= pic_height) return;
    if (x0 + width > pic_width) width = pic_width - x0;
    if (y0 + height > pic_height) height = pic_height - y0;

    for(pic_line=y0; pic_line<(y0+height); pic_line++)
    {
        /************************* Process upgrade table *************************/
        if(sjb_length != 0){  // upgrade table present
            for(uint16_t i=0; i<sjb_length; i++){
                if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i))
                {
                    line_pos_base = 0;
                }
                else
                {
                    line_pos_base = 65536*(i+1);
                    if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i+1))
                        break;
                }
            }
        }
        /********************** upgrade table processing complete **********************/

        // Pre-fetch the start offset of the specified row in the encoded data
        line_addr = encoder_addr + GET_RGB565_ENCODER_LINE_DATA_ADDR(src, pic_line) + line_pos_base;

        x00 = 0;
        real_width = width;        // Actual image width
        for(pic_col = 0; pic_col<pic_width; pic_col++)
        {
            SET_RGB565_ENCODER_READ_HINT(src, line_addr + pic_col);
            uint16_t EncodeData = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
            uint16_t tmpdata = EncodeData & 0xff80;
            
            /********************************* RLE decoder  **********************************/
            if(tmpdata == ecoder_rle_Flag)
            {
                pic_col++;        // Point to ref_color
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if (EncodeData == ecoder_rle_Flag)        // Long-code encoding
                {
                    pic_col++;        // Point to length data
                    pixl_len = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);        // Read RLE-encoded length
                }
                else    // short-code encoding
                {
                    pixl_len = EncodeData & 0x007f;        // Fetch encoded length
                }

                if(x00 < x0){        // x00 Cursor is to the left of the target start
                    if (x00 + pixl_len < x0)    // x00 Cursor still left of target after adding decode length, skip
                    {
                        x00 += pixl_len;
                        continue;                // Continue to fetch next data segment
                    }
                    else
                    {
                        pixl_len = x00 + pixl_len - x0;	// Valid length starting from x0 coordinate
                        x00 = x0 + pixl_len;
                    }
                }
                
                if (real_width > pixl_len)    // Required pixel length exceeds encoded length, decode entire run
                {
                    zhRGB565_RLE_decoder(ref_color, pixl_len, buf);
                    buf += pixl_len;
                    real_width -= pixl_len;
                }
                else
                {// Remaining pixels < encoded run length; decode only the leftover width
                    zhRGB565_RLE_decoder(ref_color, real_width, buf);
                    buf += real_width;
                    // Buffer line break
                    buf_base += iTargetStride;
                    buf = buf_base;
                    break;
                }
            }
            /********************************* Delta decoding  **********************************/
            else if(tmpdata == ecoder_diff_Flag)    // Delta encoding flag
            {
                len = EncodeData & 0x007f;            // Fetch encoded length; delta encoding uses short-length format
                pixl_len = len*2 + 1;                 // Total decompressed pixel length
                
                uint16_t real_pixl_len = pixl_len;
                uint16_t skip_cnt = 0;
                
                if(x00 < x0)        // x00 Cursor is to the left of the target start
                {    
                    if (x00 + pixl_len < x0)    // x00 Cursor still left of target after adding decode length, skip
                    {
                        x00 += pixl_len;
                        pic_col += len + 1;
                        continue;    
                    }
                    else    // If fully decoded, length can reach the start coordinate
                    {
						// Calculate new coordinate and decode length
                        real_pixl_len = x00 + pixl_len - x0;
                        x00 = x0 + real_pixl_len;
                        skip_cnt = pixl_len - real_pixl_len;    // Number of leading pixels to skip
                    }
                }

                uint16_t pix1, pix2, encdata;
                pic_col++;        // Point to encoded data
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if(skip_cnt == 0){            // No skip needed, decode directly
                    // Reference color stored as-is, retrieved as-is
                    zhRGB565_save_pixel(buf++, ref_color);
                    real_width--;
                    // Decoding of current line finished, switch to next line
                    if(real_width == 0){
                        buf_base += iTargetStride;
                        buf = buf_base;
                        break;
                    }
                }
                else
                    skip_cnt--;
                
                for(uint16_t i=0; i<len; i++)
                {
                    pic_col++;        // Point to encoded data
                    encdata = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                    zhRGB565_DIFF_decoder(ref_color, encdata, &pix1, &pix2);    // Decode one data word to yield 2 pixels
                    
                    if(skip_cnt == 0){        // No skip needed, decode directly
                        // First pixel
                        zhRGB565_save_pixel(buf++, pix1);
                        real_width--;
                        // Decoding of current line finished, switch to next line
                        if(real_width == 0){ break; }
                    }
                    else
                        skip_cnt--;
                    
                    if(skip_cnt == 0){        // No skip needed, decode directly
                        // Second pixel
                        zhRGB565_save_pixel(buf++, pix2);
                        real_width--;
                        // Decoding of current line finished, switch to next line
                        if(real_width == 0){ break; }
                    }
                    else
                        skip_cnt--;
                    // The last decoded pixel serves as the reference color for the next encoded data
                    ref_color = pix2;
                }
                            
                // Buffer line break
                if(real_width == 0){ buf_base += iTargetStride; buf = buf_base; break; }
            }
            else
            {
                if (x00 < x0)
                {
                    x00++;
                    continue;
                }
                zhRGB565_save_pixel(buf++, EncodeData);
                real_width--;
                
                // Buffer line break
                if(real_width == 0){ buf_base += iTargetStride; buf = buf_base; break; }
            }
        }
    }
}

#endif


