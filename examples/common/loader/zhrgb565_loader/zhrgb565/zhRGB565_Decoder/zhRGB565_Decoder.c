#include "zhRGB565_Decoder.h"

#if defined(_RTE_)
#    include "RTE_Components.h"
#endif
#if defined(RTE_Acceleration_Arm_2D)
#    include "arm_2d_helper.h"
#endif

#if defined(RTE_Acceleration_Arm_2D_Extra_Loader)
#    include "arm_2d_example_loaders.h"
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
 * ref_color：基准色
 * encoded_num：重复数据
 * pixel：解码后的像素保存的地址
 */
__STATIC_INLINE
void zhRGB565_RLE_decoder(uint16_t ref_color, uint16_t encoder_num, uint16_t *pixel)
{
    for(uint32_t i=0;i<encoder_num; i++){
        zhRGB565_save_pixel(pixel++, ref_color);
    }
}


/**
 * ref_color：基准色
 * encoderDATA：编码数据
 * pixel1,pixel2：解码后的像素
 */
__STATIC_FORCEINLINE
void zhRGB565_DIFF_decoder(uint16_t ref_color, uint16_t encoderDATA, uint16_t *pixel1, uint16_t *pixel2)
{
    uint8_t tmpdataH = encoderDATA>>8, tmpdataL = encoderDATA;
    uint16_t HI = GET_RGB332_TO_RGB565(tmpdataH);
    uint16_t LO = GET_RGB332_TO_RGB565(tmpdataL);
    
    uint16_t tmpPixel = ref_color ^ HI;
    zhRGB565_save_pixel(pixel1, tmpPixel);
    zhRGB565_save_pixel(pixel2, tmpPixel ^ LO);
}

#else

/**
 * ref_color：基准色
 * encoded_num：重复数据
 * pixel：解码后的像素保存的地址
 */
static inline void zhRGB565_RLE_decoder(uint16_t ref_color, uint16_t encoder_num, uint16_t *pixel)
{
    for(uint32_t i=0;i<encoder_num; i++){
        *(pixel) = ref_color;
        pixel++;
    }
}


/**
 * ref_color：基准色
 * encoderDATA：编码数据
 * pixel1,pixel2：解码后的像素
 */
static inline void zhRGB565_DIFF_decoder(uint16_t ref_color, uint16_t encoderDATA, uint16_t *pixel1, uint16_t *pixel2)
{
    uint8_t tmpdataH = encoderDATA>>8, tmpdataL = encoderDATA;
    uint16_t HI = GET_RGB332_TO_RGB565(tmpdataH);
    uint16_t LO = GET_RGB332_TO_RGB565(tmpdataL);
    
    *pixel1 = ref_color ^ HI;
    *pixel2 = *pixel1 ^ LO;
}
#endif

// x0,y0:(x0,y0)图片上选择区域的左上角坐标
// width，height：取图片的宽高区域
// src：压缩数据
// buf：解压缓存
void zhRGB565_decompress_baseversion(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height,const uint16_t *src, uint16_t *buf)
{
    uint16_t pic_width = GET_RGB565_ENCODER_WIDTH(src);
    uint16_t pic_height = GET_RGB565_ENCODER_HEIGHT(src);
    uint16_t ecoder_rle_Flag = GET_RGB565_ENCODER_FLAG(src);
    uint16_t ecoder_diff_Flag = GET_RGB565_ENCODER_FLAG(src)|0x0080;        // 差值编码和RLE编码标志高字节是一样的，但是差值编码数据较少，所以差值编码没有长编码
    uint32_t sjb_length = GET_RGB565_ENCODER_SJB_LENGHT(src);                // 记录升阶表长度
    uint32_t encoder_addr = GET_RGB565_ENCODER_DATA_ADDR(src);                // 编码数据起点坐标
    
    uint16_t pic_line, pic_col, i;
    uint32_t line_pos_base = 0;
    uint32_t real_width;
    uint32_t x00, line_addr;        // 实际解码编码
    
    uint16_t ref_color, len, pixl_len;
    
    // 边界检查
    if (x0 >= pic_width || y0 >= pic_height) return;
    if (x0 + width > pic_width) width = pic_width - x0;
    if (y0 + height > pic_height) height = pic_height - y0;

    // 打印图片数据
    for(pic_line=y0; pic_line<(y0+height); pic_line++)        // 遍历每行
    {
        /************************* 处理升阶表 *************************/
        if(sjb_length != 0){        // 有升阶表
            for(i=0; i<sjb_length; i++){
                if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i))        // 解码行数小于
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
        /********************** 处理升阶表结束 **********************/
        
        // 预先读取编码数据中指定行的起点坐标
        line_addr = encoder_addr + GET_RGB565_ENCODER_LINE_DATA_ADDR(src, pic_line) + line_pos_base;
        
        x00 = 0;
        real_width = width;        // 实际图片宽度
        for(pic_col = 0; pic_col<pic_width; pic_col++)
        {
            uint16_t EncodeData = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
            uint16_t tmpdata = EncodeData & 0xff80;
            
            if(tmpdata == ecoder_rle_Flag)                // RLE编码标志
            {
                pic_col++;        // 指向参考色
                // 取参考色
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if (EncodeData == ecoder_rle_Flag)        // 长编码
                {
                    pic_col++;        // 指向长度数据
                    pixl_len = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);        // 取像素长度，即RLE编码长度
                }
                else     // 短编码
                {
                    pixl_len = EncodeData & 0x007f;        // 取低字节，取编码长度
                }

                if(x00 < x0){        // x00游标在目标起点左侧
                    if (x00 + pixl_len < x0)    // x00游标+解码长度后仍位于左侧，跳过
                    {
                        x00 += pixl_len;        // x00游标+解码长度
                        continue;                // 继续检索新数据
                    }
                    else
                    {
                        pixl_len = x00 + pixl_len - x0;        // 从x0坐标开始的有效长度
                        x00 = x0 + pixl_len;                // x00新坐标
                    }
                }
                
                if (real_width > pixl_len)    // 需要的像素长度大于编码长度，全部解码
                {
                    zhRGB565_RLE_decoder(ref_color, pixl_len, buf);
                    buf += pixl_len;
                    real_width -= pixl_len;
                }
                else
                {// 剩余要解码的长度小于编码长度，直接解码剩余宽度的数据
                    zhRGB565_RLE_decoder(ref_color, real_width, buf);
                    buf += real_width;
                    break;
                }
            }
            else if(tmpdata == ecoder_diff_Flag)    // 差值编码标志
            {
                len = EncodeData & 0x007f;            // 取低字节，取编码长度，差值编码采用短长度编码
                pixl_len = len*2 + 1;                // 全部解压后的像素长度
                
                uint16_t real_pixl_len = pixl_len;
                uint16_t skip_cnt = 0;
                
                if(x00 < x0)        // x00游标在目标起点左侧
                {    
                    if (x00 + pixl_len < x0)    // x00游标+全部解压后的像素长度仍位于左侧，跳过
                    {
                        x00 += pixl_len;        // x00游标+解码长度
                        pic_col += len + 1;                // 指向参考色
                        continue;    
                    }
                    else    // 全部解码的话，长度可以够到起点坐标
                    {
                        real_pixl_len = x00 + pixl_len - x0;    // 从x0坐标开始的有效长度，这个要从解码后最后的像素向前取
                        x00 = x0 + real_pixl_len;                // x00新坐标
                        skip_cnt  = pixl_len - real_pixl_len;    // 需要跳过前面的像素数
                    }
                }

                uint16_t pix1, pix2, encdata;
                pic_col++;        // 指向参考色
                // 取参考色
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if(skip_cnt == 0){        // 无需跳过，直接解码
                    *buf++ = ref_color;        // 参考色是原值存储，原值取出
                    real_width--;
                    // 缓冲区换行
                    if(real_width == 0){         // 本行解码完毕,切换到新的一行解码
                        break;
                    }
                }
                else
                    skip_cnt--;
                
                for(uint16_t i=0; i<len; i++)
                {
                    pic_col++;        // 指向编码数据
                    encdata = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                    zhRGB565_DIFF_decoder(ref_color, encdata, &pix1, &pix2);    // 解码一个数据，可以解出2个像素
                    
                    if(skip_cnt == 0){        // 无需跳过，直接解码
                        *buf++ = pix1;        // 第一个像素
                        real_width--;
                        // 缓冲区换行
                        if(real_width == 0){ break; }    // 本行解码完毕,切换到新的一行解码
                    }
                    else
                        skip_cnt--;
                    
                    if(skip_cnt == 0){        // 无需跳过，直接解码
                        *buf++ = pix2;        // 第二个像素
                        real_width--;
                        // 缓冲区换行
                        if(real_width == 0){ break; }    // 本行解码完毕,切换到新的一行解码
                    }
                    else
                        skip_cnt--;
                    
                    ref_color = pix2;        // 解码出的最后一个像素是下一个编码数据的参考色
                }
                            
                // 缓冲区换行
                if(real_width == 0){ break; }
            }
            else
            {
                if (x00 < x0)
                {
                    x00++;        // x00游标前进一个像素
                    continue;
                }
                *buf++ = EncodeData;
                real_width--;
                // 缓冲区换行
                if(real_width == 0){ break; }
            }
        }
    }
}

#if defined(RTE_Acceleration_Arm_2D)

#if defined(RTE_Acceleration_Arm_2D_Extra_Loader)
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
    arm_generic_loader_io_read(ptLoader, (uint8_t *)&hwData, 2);
    return hwData;
}

__STATIC_FORCEINLINE
uint16_t zhRGB565_read_u16_from_loader_with_offset(uintptr_t ptLoader, uintptr_t nOffset)
{
    arm_generic_loader_io_seek(	ptLoader, 
                                nOffset * sizeof(uint16_t), 
                                SEEK_SET);
    return zhRGB565_read_u16_from_loader(ptLoader);
}

/* 获取图片 - 宽度 */
//#define     GET_RGB565_ENCODER_WIDTH(BUF)                ((BUF)[0])
#define GET_RGB565_ENCODER_WIDTH(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 0)

/* 获取图片 - 高度 */
//#define     GET_RGB565_ENCODER_HEIGHT(BUF)                ((BUF)[1])
#define GET_RGB565_ENCODER_HEIGHT(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 1)

/* 获取图片 - 编码标志 */
//#define     GET_RGB565_ENCODER_FLAG(BUF)                ((BUF)[2])
#define GET_RGB565_ENCODER_FLAG(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 2)

/* 获取图片 - 升阶表长度 */
//#define     GET_RGB565_ENCODER_SJB_LENGHT(BUF)          ((BUF)[3])
#define GET_RGB565_ENCODER_SJB_LENGHT(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 3)

/* 获取图片 - 行偏移表起点地址 */
//#define     GET_RGB565_ENCODER_LINE_POS(BUF)            ((BUF)[4])
#define GET_RGB565_ENCODER_LINE_POS(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 4)

/* 获取图片 - 编码数据起点地址 */
//#define     GET_RGB565_ENCODER_DATA_ADDR(BUF)            ((BUF)[5])
#define GET_RGB565_ENCODER_DATA_ADDR(__addr)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 5)

/* 获取图片 - 获取升阶表第N个数据的行号,N=0,1,2...*/
//#define     GET_RGB565_ENCODER_SJB_DATA(BUF, N)            ((BUF)[6+N])
#define GET_RGB565_ENCODER_SJB_DATA(__addr, __index)    \
        zhRGB565_read_u16_from_loader_with_offset((uintptr_t)(__addr), 6 + (__index))

/* 获取图片 - 第N行编码数据在行偏移表的地址 ,N=0,1,2...*/
//#define     GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N)    ((BUF)[GET_RGB565_ENCODER_LINE_POS(BUF) + (N)])
#define GET_RGB565_ENCODER_LINE_DATA_ADDR(__addr, __index)                      \
        zhRGB565_read_u16_from_loader_with_offset(                              \
            (uintptr_t)(__addr),                                                \
            GET_RGB565_ENCODER_LINE_POS(__addr) + (__index))


/* 获取图片 - 图片第N行编码数据的第M个数据 ,N=0,1,2...*/
//#define     GET_RGB565_ENCODER_LINE_DATA(BUF,N,M)        ((BUF)[GET_RGB565_ENCODER_DATA_ADDR(BUF) + GET_RGB565_ENCODER_LINE_DATA_ADDR(BUF,N) + (M)])
#define GET_RGB565_ENCODER_LINE_DATA(__addr, N, M)                              \
        zhRGB565_read_u16_from_loader_with_offset(                              \
            (uintptr_t)(__addr),                                                \
			GET_RGB565_ENCODER_DATA_ADDR(__addr) +                              \
            GET_RGB565_ENCODER_LINE_DATA_ADDR(__addr, (N)) + (M))

//#define     GET_RGB565_ENCODER_LINE_DATA2(BUF,N,M)        ((BUF)[(N) + (M)])
#define GET_RGB565_ENCODER_LINE_DATA2(__addr, N, M)                             \
        zhRGB565_read_u16_from_loader_with_offset(                              \
            (uintptr_t)(__addr),                                                \
			(N) + (M))

ARM_NONNULL(1)
arm_2d_size_t zhRGB565_get_image_size(arm_generic_loader_t *ptLoader)
{
    assert(NULL != ptLoader);
    arm_2d_size_t tSize = {
        .iWidth = GET_RGB565_ENCODER_WIDTH((const uint16_t *)ptLoader),
        .iHeight = GET_RGB565_ENCODER_HEIGHT((const uint16_t *)ptLoader),
    };
    return tSize;
}
#endif

// x0,y0:(x0,y0)图片上选择区域的左上角坐标
// width，height：取图片的宽高区域
// src：压缩数据
// buf：解压缓存
void zhRGB565_decompress_for_arm2d(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, const uint16_t *src, uint16_t *buf, int16_t iTargetStride)
{
    uint16_t pic_width = GET_RGB565_ENCODER_WIDTH(src);
    uint16_t pic_height = GET_RGB565_ENCODER_HEIGHT(src);
    uint16_t ecoder_rle_Flag = GET_RGB565_ENCODER_FLAG(src);
    uint16_t ecoder_diff_Flag = GET_RGB565_ENCODER_FLAG(src)|0x0080;    // 差值编码和RLE编码标志高字节是一样的，但是差值编码数据较少，所以差值编码没有长编码
    uint32_t sjb_length = GET_RGB565_ENCODER_SJB_LENGHT(src);            // 记录升阶表长度
    uint32_t encoder_addr = GET_RGB565_ENCODER_DATA_ADDR(src);            // 编码数据起点坐标
    
    uint16_t pic_line, pic_col;
    uint32_t line_pos_base = 0, real_width;
    uint32_t x00, line_addr;
    
    COLOUR_INT *buf_base;
    buf_base = (COLOUR_INT *)buf;
    
    uint16_t g_sjb_cur = 0;
    uint16_t ref_color, len, pixl_len;
    
    // 边界检查
    if (x0 >= pic_width || y0 >= pic_height) return;
    if (x0 + width > pic_width) width = pic_width - x0;
    if (y0 + height > pic_height) height = pic_height - y0;

    // 打印图片数据
    for(pic_line=y0; pic_line<(y0+height); pic_line++)        // 遍历每行
    {
        /************************* 处理升阶表 *************************/
        if(sjb_length != 0){        // 有升阶表
            for(uint16_t i=0; i<sjb_length; i++){
                if(pic_line < GET_RGB565_ENCODER_SJB_DATA(src, i))        // 解码行数小于升阶表行数
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
        
        /********************** 处理升阶表结束 **********************/

        // 预先读取编码数据中指定行的起点坐标
        line_addr = encoder_addr + GET_RGB565_ENCODER_LINE_DATA_ADDR(src, pic_line) + line_pos_base;

        x00 = 0;
        real_width = width;        // 实际图片宽度
        for(pic_col = 0; pic_col<pic_width; pic_col++)
        {
            uint16_t EncodeData = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
            uint16_t tmpdata = EncodeData & 0xff80;
            
            /********************************* RLE解码  **********************************/
            if(tmpdata == ecoder_rle_Flag)                // RLE编码标志
            {
                pic_col++;        // 指向参考色
                // 取参考色
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if (EncodeData == ecoder_rle_Flag)        // 长编码
                {
                    pic_col++;        // 指向长度数据
                    pixl_len = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);        // 取像素长度，即RLE编码长度
                }
                else    // 短编码
                {
                    pixl_len = EncodeData & 0x007f;        // 取低字节，取编码长度
                }

                if(x00 < x0){        // x00游标在目标起点左侧
                    if (x00 + pixl_len < x0)    // x00游标+解码长度后仍位于左侧，跳过
                    {
                        x00 += pixl_len;        // x00游标+解码长度
                        continue;                // 继续检索新数据
                    }
                    else
                    {
                        pixl_len = x00 + pixl_len - x0;        // 从x0坐标开始的有效长度
                        x00 = x0 + pixl_len;                // x00新坐标
                    }
                }
                
                if (real_width > pixl_len)    // 需要的像素长度大于编码长度，全部解码
                {
                    zhRGB565_RLE_decoder(ref_color, pixl_len, buf);
                    buf += pixl_len;
                    real_width -= pixl_len;
                }
                else
                {// 剩余要解码的长度小于编码长度，直接解码剩余宽度的数据
                    zhRGB565_RLE_decoder(ref_color, real_width, buf);
                    buf += real_width;
                    // 缓冲区换行
                    buf_base += iTargetStride;
                    buf = buf_base;
                    break;
                }
            }
            /********************************* 差值解码  **********************************/
            else if(tmpdata == ecoder_diff_Flag)    // 差值编码标志
            {
                len = EncodeData & 0x007f;            // 取低字节，取编码长度，差值编码采用短长度编码
                pixl_len = len*2 + 1;                // 全部解压后的像素长度
                
                uint16_t real_pixl_len = pixl_len;
                uint16_t skip_cnt = 0;
                
                if(x00 < x0)        // x00游标在目标起点左侧
                {    
                    if (x00 + pixl_len < x0)    // x00游标+全部解压后的像素长度仍位于左侧，跳过
                    {
                        x00 += pixl_len;        // x00游标+解码长度
                        pic_col += len + 1;        // 指向参考色
                        continue;    
                    }
                    else    // 全部解码的话，长度可以够到起点坐标
                    {
                        real_pixl_len = x00 + pixl_len - x0;    // 从x0坐标开始的有效长度，这个要从解码后最后的像素向前取
                        x00 = x0 + real_pixl_len;                // x00新坐标
                        skip_cnt = pixl_len - real_pixl_len;    // 需要跳过前面的像素数
                    }
                }

                uint16_t pix1, pix2, encdata;
                pic_col++;        // 指向参考色
                // 取参考色
                ref_color = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                
                if(skip_cnt == 0){            // 无需跳过，直接解码
                    //*buf++ = ref_color;        // 参考色是原值存储，原值取出
                    zhRGB565_save_pixel(buf++, ref_color);
                    real_width--;
                    // 缓冲区换行
                    if(real_width == 0){     // 本行解码完毕,切换到新的一行解码
                        buf_base += iTargetStride;
                        buf = buf_base;
                        break;
                    }
                }
                else
                    skip_cnt--;
                
                for(uint16_t i=0; i<len; i++)
                {
                    pic_col++;        // 指向编码数据
                    encdata = GET_RGB565_ENCODER_LINE_DATA2(src, line_addr, pic_col);
                    zhRGB565_DIFF_decoder(ref_color, encdata, &pix1, &pix2);    // 解码一个数据，可以解出2个像素
                    
                    if(skip_cnt == 0){        // 无需跳过，直接解码
                        //*buf++ = pix1;        // 第一个像素
                        zhRGB565_save_pixel(buf++, pix1);
                        real_width--;
                        // 缓冲区换行
                        if(real_width == 0){ break; }        // 本行解码完毕,切换到新的一行解码
                    }
                    else
                        skip_cnt--;
                    
                    if(skip_cnt == 0){        // 无需跳过，直接解码
                        //*buf++ = pix2;        // 第二个像素
                        zhRGB565_save_pixel(buf++, pix2);
                        real_width--;
                        // 缓冲区换行
                        if(real_width == 0){ break; }        // 本行解码完毕,切换到新的一行解码
                    }
                    else
                        skip_cnt--;
                    
                    ref_color = pix2;        // 解码出的最后一个像素是下一个编码数据的参考色
                }
                            
                // 缓冲区换行
                if(real_width == 0){ buf_base += iTargetStride; buf = buf_base; break; }
            }
            else
            {
                if (x00 < x0)
                {
                    x00++;
                    continue;
                }
                //*buf++ = EncodeData;
                zhRGB565_save_pixel(buf++, EncodeData);
                real_width--;
                
                // 缓冲区换行
                if(real_width == 0){ buf_base += iTargetStride; buf = buf_base; break; }
            }
        }
    }
}

#endif


