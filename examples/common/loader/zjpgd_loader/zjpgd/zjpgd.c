/*
 * Copyright (c) 2025 Jiapeng Li <mail@jiapeng.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "zjpgd.h"
#include "zjpgdalg.h"

/*-------------------------------------------------------------------------*/
// Utility functions
static void *zjd_malloc(zjd_t *zjd, int32_t len)
{
    uint8_t *p = 0;

    /* Align block size to the word boundary */
    len = (len + 3) & ~3;

    if (zjd->buflen >= len) {
        zjd->buflen -= len;
        p = (uint8_t *)zjd->buf;
        zjd->buf = (void *)(p + len);
    }

    return (void *)p;
}

#define CVACC_COEF   1024

static inline uint8_t ycbcr2r(int Y, int Cb, int Cr)
{
    return BYTECLIP(Y + ((int)(1.402 * CVACC_COEF) * Cr) / CVACC_COEF);
}

static inline uint8_t ycbcr2g(int Y, int Cb, int Cr)
{
    return BYTECLIP(Y - (((int)(0.344 * CVACC_COEF) * Cb + (int)(0.714 * CVACC_COEF) * Cr) / CVACC_COEF));
}

static inline uint8_t ycbcr2b(int Y, int Cb, int Cr)
{
    return BYTECLIP(Y + ((int)(1.772 * CVACC_COEF) * Cb) / CVACC_COEF);
}

static inline bool is_rect_intersect(const zjd_rect_t *r0, const zjd_rect_t *r1)
{
    return !(
               r0->x + r0->w <= r1->x ||   // r0 is completely to the left of r1
               r1->x + r1->w <= r0->x ||   // r1 is completely to the left of r0
               r0->y + r0->h <= r1->y ||   // r0 is completely above r1
               r1->y + r1->h <= r0->y      // r1 is completely above r0
           );
}

static inline bool is_r0_beyond_r1(const zjd_rect_t *r0, const zjd_rect_t *r1)
{
    return (r0->x + r0->w >= r1->x + r1->w) &&
           (r0->y + r0->h >= r1->y + r1->h);
}

/*-------------------------------------------------------------------------*/
// Log
#if ZJD_DEBUG
const char *zjd_code2bin(char *buf, int code, int bits)
{
    int i;

    for (i = 0; i < bits; i++) {
        buf[bits - 1 - i] = (code & (1 << i)) ? '1' : '0';
    }
    buf[bits] = '\0';

    return buf;
}

void zjd_log_huffman_table(int num, int cls, uint8_t *hb, uint16_t *hc, uint8_t *hd)
{
    int i, j, total_codes;
    char buf[32];
    if (hb == NULL || hc == NULL || hd == NULL) {
        return;
    }

    ZJD_LOG("\nTable Number: %d, Class: %s, Huffman Table [%d][%d]:", num, cls ? "AC" : "DC", num, cls);
    ZJD_LOG("index, bits, code(hex), code(bin), data");
    j = 0;
    for (i = 0; i < 16; i++) {
        total_codes = hb[i];
        while (total_codes--)  {
            ZJD_LOG("%3d, %2d, %04X, %17s, %02X", j, i + 1, hc[j], zjd_code2bin(buf, hc[j], i + 1), hd[j]);
            j++;
        }
    }
}

void zjd_log_qt_table(int i, int32_t *p)
{
    ZJD_LOG("\nQuantization Table ID: %d", i);
    ZJD_INTDUMP(p, 64);
}

void zjd_log(zjd_t *zjd)
{
    ZJD_LOG("\nIpsf:");
    ZJD_INTDUMP(Ipsf, 64);

    ZJD_LOG("\nZigZag:");
    ZJD_INTDUMP(ZIGZAG, 64);
}
#endif

/*-------------------------------------------------------------------------*/
// Decompress Helpers

static inline int zjd_get_hc(zjd_huff_t *huff, uint32_t dreg, uint8_t dbit, uint8_t *val)
{
    uint8_t i, n_codes;
    uint16_t n_bits;

    const uint8_t *hb = huff->bits;
    const uint16_t *hc = huff->code;
    const uint8_t *hd = huff->data;

    /* Incremental search for all codes */
    for (i = 1; i <= 16; i++) {
        n_codes = *hb++;
        n_bits = dreg >> (32 - i);
        while (n_codes--)  {
            if (n_bits == *hc) {  /* Compare code */
                if (i > dbit) {
                    return 0;
                }
                *val = *hd;
                return i;
            }
            hc++;
            hd++;
        }
    }

    return 0;
}

/*-------------------------------------------------------------------------*/
// YUV to Pixel Format

// YUV → Grayscale (1 byte)
static void yuv_to_grayscale(uint8_t **pix, int yy, int cb, int cr)
{
    *(*pix)++ = (uint8_t)yy;  // Just use Y channel
}

// YUV → RGB888 (R, G, B)
static void yuv_to_rgb888(uint8_t **pix, int yy, int cb, int cr)
{
    *(*pix)++ = ycbcr2r(yy, cb, cr);
    *(*pix)++ = ycbcr2g(yy, cb, cr);
    *(*pix)++ = ycbcr2b(yy, cb, cr);
}

// YUV → BGR888 (B, G, R)
static void yuv_to_bgr888(uint8_t **pix, int yy, int cb, int cr)
{
    *(*pix)++ = ycbcr2b(yy, cb, cr);
    *(*pix)++ = ycbcr2g(yy, cb, cr);
    *(*pix)++ = ycbcr2r(yy, cb, cr);
}

// YUV → RGB565 (2 bytes)
static void yuv_to_rgb565(uint8_t **pix, int yy, int cb, int cr)
{
    uint8_t r = ycbcr2r(yy, cb, cr);
    uint8_t g = ycbcr2g(yy, cb, cr);
    uint8_t b = ycbcr2b(yy, cb, cr);

    uint16_t rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

    *(*pix)++ = rgb & 0xFF;
    *(*pix)++ = (rgb >> 8) & 0xFF;
}

// YUV → BGR565 (2 bytes)
static void yuv_to_bgr565(uint8_t **pix, int yy, int cb, int cr)
{
    uint8_t r = ycbcr2r(yy, cb, cr);
    uint8_t g = ycbcr2g(yy, cb, cr);
    uint8_t b = ycbcr2b(yy, cb, cr);

    uint16_t bgr = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);

    *(*pix)++ = bgr & 0xFF;
    *(*pix)++ = (bgr >> 8) & 0xFF;
}

// YUV → RGBA8888 (R, G, B, A)
static void yuv_to_rgba8888(uint8_t **pix, int yy, int cb, int cr)
{
    *(*pix)++ = ycbcr2r(yy, cb, cr);
    *(*pix)++ = ycbcr2g(yy, cb, cr);
    *(*pix)++ = ycbcr2b(yy, cb, cr);
    *(*pix)++ = 0xFF;  // Alpha channel
}

// YUV → BGRA888 (B, G, R, A)
static void yuv_to_bgra8888(uint8_t **pix, int yy, int cb, int cr)
{
    *(*pix)++ = ycbcr2b(yy, cb, cr);
    *(*pix)++ = ycbcr2g(yy, cb, cr);
    *(*pix)++ = ycbcr2r(yy, cb, cr);
    *(*pix)++ = 0xFF;  // Alpha channel
}

static const zjd_yuv2pix_t zjd_yuv2pix_tab[] = {
    yuv_to_grayscale,
    yuv_to_rgb565,
    yuv_to_bgr565,
    yuv_to_rgb888,
    yuv_to_bgr888,
    yuv_to_rgba8888,
    yuv_to_bgra8888,
};

/*-------------------------------------------------------------------------*/
// YUV Scan
static int yuv400_scan(zjd_t *zjd, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect)
{
    uint8_t *pix;
    zjd_yuv_t *py;
    int yy, cb = 0, cr = 0;

    /* Build a RGB MCU from discrete components */
    pix = (uint8_t *)zjd->workbuf;
    py = zjd->mcubuf;
    for (unsigned int iy = 0; iy < 8; iy++) {
        for (unsigned int ix = 0; ix < 8; ix++) {
            yy = *py++;   /* Get Y component */
            zjd->yuv2pix(&pix, yy, cb, cr);
        }
    }

    return zjd->ofunc(zjd, mcu_rect, zjd->workbuf);
}

static int yuv444_scan(zjd_t *zjd, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect)
{
    uint8_t *pix = (uint8_t *)zjd->workbuf;
    zjd_yuv_t *py, *pcb, *pcr;
    int yy, cb, cr;

    /* In YUV444, each pixel has its own Y, Cb, Cr values */
    py  = zjd->mcubuf;        // Y block start
    pcb = py + 64;           // Cb block start
    pcr = pcb + 64;          // Cr block start

    for (unsigned int iy = 0; iy < 8; iy++) {
        for (unsigned int ix = 0; ix < 8; ix++) {
            yy = *py++;
            cb = *pcb++ - 128;
            cr = *pcr++ - 128;

            zjd->yuv2pix(&pix, yy, cb, cr);
        }
    }

    ZJD_LOG("RGB888 (YUV444): (x:%u,y:%u)-(%u,%u)", mcu_rect->x, mcu_rect->y, mcu_rect->w, mcu_rect->h);
    pix = (uint8_t *)zjd->workbuf;
    ZJD_RGBDUMP(pix, 3 * 64);

    return zjd->ofunc(zjd, mcu_rect, zjd->workbuf);
}

static int yuv422_scan(zjd_t *zjd, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect)
{
    int iy, ix, icmp;
    zjd_yuv_t *py, *pcb, *pcr;
    uint8_t *pix = (uint8_t *)zjd->workbuf;

    int y_block_col, y_block_row;
    int yy, cb, cr;

    zjd_rect_t rect;

    // 4 blocks: Y1, Y2, Cb, Cr → 16x8 Y region
    pcb = zjd->component[2].mcubuf;     // Cb block starts after Y1 + Y2
    pcr = zjd->component[3].mcubuf;     // Cr block starts after Cb

    // Loop through the two Y blocks (icmp = 0: left, 1: right)
    for (icmp = 0; icmp < 2; icmp++) {
        py = zjd->component[icmp].mcubuf;
        pix = (uint8_t *)zjd->workbuf;

        // Block positions: 0=(0,0), 1=(8,0)
        y_block_col = (icmp & 1) << 3;  // 0 or 8
        y_block_row = 0;                // Only one row of Y blocks

        rect.x = mcu_rect->x + y_block_col;
        rect.y = mcu_rect->y + y_block_row;
        rect.w = 8;
        rect.h = 8;

        if ((tgt_rect != NULL) && !is_rect_intersect(&rect, tgt_rect)) {
            continue;   // Skip this block if it does not intersect with the target rectangle
        }

        for (iy = 0; iy < 8; iy++) {
            for (ix = 0; ix < 8; ix++) {
                int x_abs = y_block_col + ix;
                int y_abs = y_block_row + iy;

                // Cb/Cr sampled at 2:1 horizontally, 1:1 vertically → 8x8 UV blocks
                int uv_idx = (y_abs << 3) + (x_abs >> 1);

                yy = *py++;
                cb = pcb[uv_idx] - 128;
                cr = pcr[uv_idx] - 128;

                zjd->yuv2pix(&pix, yy, cb, cr);
            }
        }

        if (!zjd->ofunc(zjd, &rect, zjd->workbuf)) {
            return 0;
        }
    }

    return 1;
}

static int yuv420_scan(zjd_t *zjd, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect)
{
    int iy, ix, icmp;
    zjd_yuv_t *py, *pcb, *pcr;
    uint8_t *pix = (uint8_t *)zjd->workbuf;

    int y_block_col, y_block_row;
    int yy, cb, cr;

    zjd_rect_t rect;

    // 6 blocks: Y1,Y2,Y3,Y4,Cb,Cr
    pcb = zjd->component[4].mcubuf; // Cb block start
    pcr = zjd->component[5].mcubuf; // Cr block start

    for (icmp = 0; icmp < 4; icmp++) {
        py = zjd->component[icmp].mcubuf;
        pix = (uint8_t *)zjd->workbuf;

        // 0: (0, 0), 1: (8, 0), 2: (0, 8), 3: (8, 8)
        y_block_col = (icmp & 1) << 3;
        y_block_row = (icmp >> 1) << 3;

        rect.x = mcu_rect->x + y_block_col;
        rect.y = mcu_rect->y + y_block_row;
        rect.w = 8;
        rect.h = 8;

        if ((tgt_rect != NULL) && !is_rect_intersect(&rect, tgt_rect)) {
            continue;   // Skip this block if it does not intersect with the target rectangle
        }

        for (iy = 0; iy < 8; iy++) {
            for (ix = 0; ix < 8; ix++) {
                int x_abs = y_block_col + ix;
                int y_abs = y_block_row + iy;
                int uv_idx = ((y_abs >> 1) << 3) + (x_abs >> 1);    // y/2 * 8 + x/2

                yy = *py++;
                cb = pcb[uv_idx] - 128;
                cr = pcr[uv_idx] - 128;

                zjd->yuv2pix(&pix, yy, cb, cr);
            }
        }
        if (!zjd->ofunc(zjd, &rect, zjd->workbuf)) {
            return 0;
        }
    }

    return 1;
}

static int zjd_mcu_scan(zjd_t *zjd, uint8_t n_cmp, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect)
{
    int cmp, i;
    zjd_comp_t *component;
    zjd_yuv_t *p;
    int32_t *tmp = (int32_t *)zjd->workbuf;

    /* dequantize && idct */
    for (cmp = 0; cmp < n_cmp; cmp++) {
        component = &zjd->component[cmp];
        p = component->mcubuf;
        ZJD_LOG("Component %d:", cmp);
        for (i = 0; i < 64; i++) {
            if (p[i]) {
                tmp[i] = p[i] * component->qttbl[i] >> 8;
            } else {
                tmp[i] = 0;
            }
        }
        ZJD_LOG("TMP:");
        ZJD_INTDUMP(tmp, 64);
        block_idct(tmp, p);
        ZJD_LOG("  P:");
        ZJD_INTDUMP(p, 64);
    }

    /* scan & output */
    return zjd->yuv_scan(zjd, mcu_rect, tgt_rect);
}

/*-------------------------------------------------------------------------*/
// JPEG Segment Handlers

/* Create de-quantization and prescaling tables with a DQT segment       */
static zjd_res_t zjd_dqt_handler(zjd_t *zjd, zjd_tbl_t *tbl, const uint8_t *buf, int32_t len)
{
    unsigned int i, zi;
    uint8_t d;
    int32_t *pb;

    while (len) { /* Process all tables in the segment */
        if (len < 65) {
            return ZJD_ERR_LEN_QTTBL;
        }
        len -= 65;

        /* Get table property */
        d = *buf++;
        if (d & 0xF0) {
            return ZJD_ERR_FMT_QTTBL;
        }

        /* Register the table */
        i = d & 3;
        pb = zjd_malloc(zjd, 64 * sizeof(int32_t)); /* Allocate a memory block for the table */
        if (!pb) {
            return ZJD_ERR_OOM_QTTBL;
        }
        tbl->qttbl[i] = pb;

        /* Load the table */
        for (i = 0; i < 64; i++) {
            zi = ZIGZAG[i];                        /* Zigzag-order to raster-order conversion */
            pb[zi] = (int32_t)((uint32_t) * buf++ * Ipsf[zi]); /* Apply scale factor of Arai algorithm to the de-quantizers */
        }

#if ZJD_DEBUG
        zjd_log_qt_table(d & 3, pb);
#endif
    }

    return ZJD_OK;
}

/* Create huffman code tables with a DHT segment                         */
static zjd_res_t zjd_dht_handler(zjd_t *zjd, zjd_tbl_t *tbl, const uint8_t *buf, int32_t len)
{
    unsigned int i, j, b, cls, num;
    int32_t np;
    uint8_t d, *pb, *pd;
    uint16_t hc, *ph;

    /* header(1) | bits counter map (16) |  */
    while (len) {
        if (len < 17) {
            return ZJD_ERR_LEN_HFMTBL1;    /* Err: wrong buf size */
        }
        len -= 17;

        /* Get table number and class: only 0x00 0x01 0x10 0x11 are valid */
        d = *buf++;
        if (d & 0xEE) {
            return ZJD_ERR_FMT_HFMTBL1;
        }
        cls = d >> 4;
        num = d & 0x0F;

        /* Load Huffman BITS table */
        pb = zjd_malloc(zjd, 16);
        if (!pb) {
            return ZJD_ERR_OOM_HFMTBL1;
        }
        for (np = i = 0; i < 16; i++) {
            np += (pb[i] = *buf++);
        }
        tbl->huffbits[num][cls] = pb;

        /* Convert BITS table to CODE table*/
        ph = zjd_malloc(zjd, np * sizeof(uint16_t));
        if (!ph) {
            return ZJD_ERR_OOM_HFMTBL2;
        }
        hc = 0;
        for (j = i = 0; i < 16; i++) {
            b = pb[i];
            while (b--) {
                ph[j++] = hc++;
            }
            hc <<= 1;
        }
        tbl->huffcode[num][cls] = ph;

        /* Load Huffman DATA table */
        if (len < np) {
            return ZJD_ERR_LEN_HFMTBL2;
        }
        len -= np;
        pd = zjd_malloc(zjd, np);
        if (!pd) {
            return ZJD_ERR_OOM_HFMTBL3;
        }
        for (i = 0; i < np; i++) {
            pd[i] = *buf++;
        }
        tbl->huffdata[num][cls] = pd;

        /* DC Table value check */
        if (cls == 0) {
            for (i = 0; i < np; i++) {
                if (pd[i] > 11) {
                    return ZJD_ERR_FMT_HFMTBL3;
                }
            }
        }

#if ZJD_DEBUG
        zjd_log_huffman_table(num, cls, pb, ph, pd);
#endif

    }

    return ZJD_OK;
}

static zjd_res_t zjd_sof0_handler(zjd_t *zjd, zjd_tbl_t *tbl, const uint8_t *buf, int32_t len)
{
    int i;
    uint8_t b;

    zjd->width = buf[3] << 8 | buf[4];      /* Image width in unit of pixel */
    zjd->height = buf[1] << 8 | buf[2];     /* Image height in unit of pixel */
    zjd->ncomp = buf[5];                 /* Number of color components */
    if (zjd->ncomp != 3 && zjd->ncomp != 1) {
        return ZJD_ERR_FMT_SOF;    /* Err: Supports only Grayscale and Y/Cb/Cr */
    }

    /* Check each image component */
    for (i = 0; i < zjd->ncomp; i++) {
        b = buf[7 + 3 * i];                         /* Get sampling factor */
        if (i == 0) {   /* Y component */
            /**/
            if (b != 0x11 && b != 0x22 && b != 0x21) {  /* Check sampling factor */
                return ZJD_ERR_FMT_SOF;                    /* Err: Supports only 4:4:4, 4:2:0 or 4:2:2 */
            }
            zjd->msx = b >> 4;
            zjd->msy = b & 15;     /* Size of MCU [blocks] */
        } else {        /* Cb/Cr component */
            if (b != 0x11) {
                return ZJD_ERR_FMT_SOF;    /* Err: Sampling factor of Cb/Cr must be 1 */
            }
        }
        tbl->qtid[i] = buf[8 + 3 * i];               /* Get dequantizer table ID for this component */
        if (tbl->qtid[i] > 3) {
            return ZJD_ERR_FMT_SOF;    /* Err: Invalid ID */
        }
    }

    ZJD_LOG("SOF0 start of frame, w: %u, h: %u, ncomp: %u, msx: %u, msy: %u, qtid:",
            zjd->width, zjd->height, zjd->ncomp, zjd->msx, zjd->msy);
    ZJD_HEXDUMP(tbl->qtid, zjd->ncomp);

    return ZJD_OK;
}

static zjd_res_t zjd_sos_handler(zjd_t *zjd, zjd_tbl_t *tbl, const uint8_t *buf, int32_t len, zjd_outfmt_t outfmt)
{
    int i, n;
    uint8_t b;

    ZJD_LOG("SOS start of scan:");
    if (!zjd->width || !zjd->height) {
        return ZJD_ERR_FMT1;    /* Err: Invalid image size */
    }

    if (buf[0] != zjd->ncomp) {
        return ZJD_ERR_FMT3;    /* Err: Wrong color components */
    }

    /* Check if all tables corresponding to each components have been loaded */
    for (i = 0; i < zjd->ncomp; i++) {
        b = buf[2 + 2 * i]; /* Get huffman table ID */
        if (b != 0x00 && b != 0x11) {
            return ZJD_ERR_FMT3;    /* Err: Different table number for DC/AC element */
        }
        n = i ? 1 : 0;                          /* Component class */
        if (!tbl->huffbits[n][0] || !tbl->huffbits[n][1]) {   /* Check huffman table for this component */
            return ZJD_ERR_FMT1;                    /* Err: Not loaded */
        }
        if (!tbl->qttbl[tbl->qtid[i]]) {          /* Check dequantizer table for this component */
            return ZJD_ERR_FMT1;                    /* Err: Not loaded */
        }
    }

    /* Allocate working buffer for MCU and pixel output */
    n = zjd->msy * zjd->msx;                      /* Number of Y blocks in the MCU */
    if (!n) {
        return ZJD_ERR_FMT1;    /* Err: SOF0 has not been loaded */
    }

    /* Shared buffer
        1. IDCT (64 * sizeof(int32_t))
        2. RGB ((64 * sizeof(ARGB8888))) output
    */
    zjd->workbuf = zjd_malloc(zjd, 64 * 4);
    if (!zjd->workbuf) {
        return ZJD_ERR_OOM0;
    }

    /* Allocate MCU working buffer */
    zjd->mcubuf = zjd_malloc(zjd, (n + 2) * 64 * sizeof(zjd_yuv_t));
    if (!zjd->mcubuf) {
        return ZJD_ERR_OOM1;
    }

    /* Y */
    for (i = 0; i < n; i++) {
        zjd->component[i].huff[0].bits = tbl->huffbits[0][0];
        zjd->component[i].huff[0].code = tbl->huffcode[0][0];
        zjd->component[i].huff[0].data = tbl->huffdata[0][0];
        zjd->component[i].huff[1].bits = tbl->huffbits[0][1];
        zjd->component[i].huff[1].code = tbl->huffcode[0][1];
        zjd->component[i].huff[1].data = tbl->huffdata[0][1];
        zjd->component[i].qttbl = tbl->qttbl[tbl->qtid[0]];
        zjd->component[i].dcv = &zjd->dcv[0];
        zjd->component[i].mcubuf = &zjd->mcubuf[i * 64];
        ZJD_LOG("huff[%d]", i);
    }

    /* CrCb */
    if (zjd->ncomp == 3) {
        for (i = 0; i < 2; i++) {
            zjd->component[n + i].huff[0].bits = tbl->huffbits[1][0];
            zjd->component[n + i].huff[0].code = tbl->huffcode[1][0];
            zjd->component[n + i].huff[0].data = tbl->huffdata[1][0];
            zjd->component[n + i].huff[1].bits = tbl->huffbits[1][1];
            zjd->component[n + i].huff[1].code = tbl->huffcode[1][1];
            zjd->component[n + i].huff[1].data = tbl->huffdata[1][1];
            zjd->component[n + i].qttbl = tbl->qttbl[tbl->qtid[i + 1]];
            zjd->component[n + i].dcv = &zjd->dcv[i + 1];
            zjd->component[n + i].mcubuf = &zjd->mcubuf[(n + i) * 64];
            ZJD_LOG("huff[%d]", n + i);
        }
    }

    /* Reuse remained buf and buflen as cache */
    ZJD_LOG("Cache: %p, %d", zjd->buf, zjd->buflen);

    /* Convert to RGB */
    if ((zjd->ncomp == 1) && (zjd->msx == 1) && (zjd->msy == 1)) {
        ZJD_LOG("YUV400");
        zjd->yuv_scan = yuv400_scan;
    } else if ((zjd->ncomp == 3) && (zjd->msx == 1) && (zjd->msy == 1)) {
        ZJD_LOG("YUV444");
        zjd->yuv_scan = yuv444_scan;
    } else if ((zjd->ncomp == 3) && (zjd->msx == 2) && (zjd->msy == 2)) {
        ZJD_LOG("YUV420");
        zjd->yuv_scan = yuv420_scan;
    } else if ((zjd->ncomp == 3) && (zjd->msx == 2) && (zjd->msy == 1)) {
        ZJD_LOG("YUV422");
        zjd->yuv_scan = yuv422_scan;
    } else {
        return ZJD_ERR_YUV;
    }

    /* default Color format */
    zjd->yuv2pix = zjd_yuv2pix_tab[outfmt];
    ZJD_LOG("Output format: %d", outfmt);

    /* Save compressed data address offset */
    zjd->imgoft = zjd->oft;

    return ZJD_OK;
}

/*-------------------------------------------------------------------------*/
// API
zjd_res_t zjd_init(zjd_t *zjd, const zjd_cfg_t *cfg)
{
    uint8_t segs[4];
    uint8_t *buf;
    uint16_t marker;
    int32_t ret, len;
    zjd_res_t rc;
    zjd_tbl_t _tbl, *tbl = &_tbl;

    memset(zjd, 0, sizeof(zjd_t));
    zjd->buf = cfg->buf;
    zjd->buflen = cfg->buflen;
    zjd->ifunc = cfg->ifunc;
    zjd->ofunc = cfg->ofunc;
    zjd->arg = cfg->arg;

    memset(tbl, 0, sizeof(zjd_tbl_t));
    while (1) {
        ret = zjd->ifunc(zjd, segs, zjd->oft, 2);
        if (ret != 2) {
            ZJD_LOG("ifunc error %d", ret);
            return ZJD_ERR_LEN0;
        }
        zjd->oft += ret;

        if (segs[0] != 0xFF) {
            return ZJD_ERR_FMT0;
        }
        marker = segs[1];
        if (zjd->oft == 2) {
            if (marker != 0xD8) {
                return ZJD_ERR_SOI;
            }
        }

        if ((marker < 0xD0) || (marker > 0xD9)) {
            ret = zjd->ifunc(zjd, segs, zjd->oft, 2);
            if (ret != 2) {
                ZJD_LOG("ifunc error %d", ret);
                return ZJD_ERR_LEN1;
            }
            zjd->oft += ret;

            len = segs[0] << 8 | segs[1];
            if (len <= 2) {
                return ZJD_ERR_FMT1;
            }
            len -= 2;
        } else {
            len = 0;
        }

        switch (marker) {
        case 0xD8:  /* SOI */
            ZJD_LOG("SOI");
            break;
        case 0xC0:  /* SOF0 */
        case 0xC4:  /* DHT */
        case 0xDA:  /* SOS */
        case 0xDB:  /* DQT */
        case 0xDD:  /* DRI */
            if (len > zjd->buflen) {
                ZJD_LOG("Too large segment %2X %d > %d", marker, len, zjd->buflen);
                return ZJD_ERR_OOM1;
            }
            buf = (uint8_t *)zjd->buf + (zjd->buflen - len);
            ret = zjd->ifunc(zjd, buf, zjd->oft, len);
            if (ret != len) {
                ZJD_LOG("ifunc error %d", ret);
                return ZJD_ERR_LEN1;
            }
            zjd->oft += ret;

            ZJD_HEXDUMP(buf, len);

            switch (marker) {
            case 0xC4: /* DHT */
                rc = zjd_dht_handler(zjd, tbl, buf, len);
                if (rc != ZJD_OK) {
                    return rc;
                }
                break;
            case 0xDB: /* DQT */
                rc = zjd_dqt_handler(zjd, tbl, buf, len);
                if (rc != ZJD_OK) {
                    return rc;
                }
                break;
            case 0xDD: /* DRI */
                ZJD_LOG("DRI");
                break;
            case 0xC0: /* SOF0 */
                rc = zjd_sof0_handler(zjd, tbl, buf, len);
                if (rc != ZJD_OK) {
                    return rc;
                }
                break;
            case 0xDA: /* SOS */
                ZJD_LOG("SOS");
                rc = zjd_sos_handler(zjd, tbl, buf, len, cfg->outfmt);
                if (rc != ZJD_OK) {
                    return rc;
                }
#if ZJD_DEBUG
                zjd_log(zjd);
#endif
                return ZJD_OK;
            }
            if (zjd->buflen <= len) {
                ZJD_LOG("No more buffer");
                return ZJD_ERR_OOM1;
            }
            break;
        case 0xC1:  /* SOF1 */
        case 0xC2:  /* SOF2 */
        case 0xC3:  /* SOF3 */
        case 0xC5:  /* SOF5 */
        case 0xC6:  /* SOF6 */
        case 0xC7:  /* SOF7 */
        case 0xC9:  /* SOF9 */
        case 0xCA:  /* SOF10 */
        case 0xCB:  /* SOF11 */
        case 0xCD:  /* SOF13 */
        case 0xCE:  /* SOF14 */
        case 0xCF:  /* SOF15 */
        case 0xD9:  /* EOI */
            ZJD_LOG("Unsupported marker %02X", marker);
            return ZJD_ERR_MARKER_NOT_SUPPORTED;
        default:
            ZJD_LOG("Skip segment marker %02X,%d", marker, len);
            if (len) {
                ret = zjd->ifunc(zjd, NULL, zjd->oft, len);
                if (ret != len) {
                    ZJD_LOG("ifunc error %d", ret);
                    return ZJD_ERR_LEN1;
                }
                zjd->oft += ret;
            }
            break;
        }
    }
    return ZJD_OK;
}

/* TODO: sub function of zjd_scan, speed optimized scan */
zjd_res_t zjd_scan_full(zjd_t *zjd)
{
    int32_t dc = 0;
    uint8_t *dp = zjd->buf;
    uint8_t last_d = 0, d = 0, dbit = 0, cnt = 0, cmp = 0, cls = 0, bl0, bl1, val, zeros, i;
    uint32_t dreg = 0;
    int ebits, dcac = 0;
    uint8_t bits_threshold = 15;
    int n_cmp;
    bool next_huff = true, skip_idct = false, rst = false;
    zjd_rect_t _mcu_rect, *mcu_rect = &_mcu_rect;

    zjd_comp_t *component = &zjd->component[cmp];
    zjd_ctx_t *ctx = &zjd->ctx;
    int32_t *tmp = (int32_t *)zjd->workbuf;

    n_cmp = zjd->msy * zjd->msx;
    if (zjd->ncomp == 3) {
        n_cmp += 2;
    }

    mcu_rect->x = 0;
    mcu_rect->y = 0;
    mcu_rect->w = zjd->msx << 3;
    mcu_rect->h = zjd->msy << 3;
    memset(component->mcubuf, 0, 64 * sizeof(zjd_yuv_t));
    memset(tmp, 0, 64 * 4);

    zjd->oft = zjd->imgoft;
    zjd->pos = zjd->oft;
    zjd->dcv[0] = 0;
    zjd->dcv[1] = 0;
    zjd->dcv[2] = 0;

    /* take snapshot */
    ctx->offset = zjd->oft;
    ctx->dreg = dreg;
    ctx->dbit = dbit;
    ctx->mcu_x = mcu_rect->x;
    ctx->mcu_y = mcu_rect->y;
    ctx->dcv[0] = zjd->dcv[0];
    ctx->dcv[1] = zjd->dcv[1];
    ctx->dcv[2] = zjd->dcv[2];

    ZJD_LOG("full mode");

    while (1) {
        if (zjd->oft >= zjd->pos) {
            dp = zjd->buf; /* Top of input buffer */
            dc = zjd->ifunc(zjd, zjd->buf, zjd->oft, zjd->buflen);
            if (!dc) {
                ZJD_LOG("No more data, %d", dbit);
                return ZJD_ERR_LEN_SOS;
            }
            zjd->pos = zjd->oft + dc;
        }

        // if (dbit > 24) {
        //     ZJD_LOG("No more buffer");
        //     return ZJD_ERR_SCAN_SLOW;
        // }

        d = *dp++;
        zjd->oft++;

        if (d == 0xFF) {
            last_d = d;
            continue;
        } else if (last_d == 0xFF) {
            last_d = 0;
            ZJD_LOG("Found marker %02X", d);
            if (d == 0x00) {
                ZJD_LOG("Padding byte");
                dreg = (dreg & ~(0xFFFFFFFF >> dbit)) | ((uint32_t)0xFF << (32 - 8 - dbit));
                dbit += 8;
            } else {
                if (d == 0xD9) {
                    ZJD_LOG("EOI marker");
                    bits_threshold = 0;
                } else if ((d >= 0xD0) && (d <= 0xD7)) {
                    ZJD_LOG("RST marker %02X", d);
                    bits_threshold = 0;
                    rst = true;
                } else {
                    ZJD_LOG("###Unknown marker %02X", d);
                    continue;
                }
            }
        } else {
            dreg = (dreg & ~(0xFFFFFFFF >> dbit)) | ((uint32_t)d << (32 - 8 - dbit));
            dbit += 8;
        }

        //ZJD_LOG("Buffer: %08X %u %02X", dreg, dbit, d);

        while (dbit > bits_threshold) {
            if (next_huff) {
                next_huff = !next_huff;

                /* 0: DC, others: AC */
                cls = !!cnt;

                // ZJD_LOG("(x: %d, y: %d), cmp %u, %s table, cls %d, cnt %d, dreg %08X, dbit %u, dcv %d,%d,%d",
                //        mcu_rect->x, mcu_rect->y, cmp, cls == 0 ? "DC" : "AC", cls, cnt, dreg, dbit, zjd->dcv[0], zjd->dcv[1], zjd->dcv[2]);

                bl0 = zjd_get_hc(&component->huff[cls], dreg, dbit, &val);
                if (!bl0) {
                    dp = zjd->buf;
                    ZJD_LOG("bl0 Huffman code too short: %08X %u", dreg, dbit);
                    return ZJD_ERR_FMT1;
                }

                dbit -= bl0;
                dreg <<= bl0;

                ZJD_LOG("processing huff, cmp %u, cnt %d, cls %u, bl0 %u, val %02X", cmp, cnt, cls, bl0, val);

                /* continue to check if need to load dreg again */
                if (val != 0) {
                    continue;
                }
            }

            if (!next_huff) {
                next_huff = !next_huff;

                // ZJD_LOG("processing bits, cnt %d val %02X", cnt, val);

                if ((val != 0) || (cnt == 0)) {
                    zeros = val >> 4;
                    bl1 = val & 0x0F;

                    if (dbit < bl1) {
                        ZJD_LOG("bl1 Huffman code too short: %08X %u < %u", dreg, dbit, bl1);
                        return ZJD_ERR_FMT4;
                    }

                    cnt += zeros;
                    if (bl1) {
                        ebits = (int)(dreg >> (32 - bl1));
                        if (!(dreg & 0x80000000)) {
                            ebits -= (1 << bl1) - 1;    /* Restore negative value if needed */
                        }
                    } else {
                        ebits = 0;
                    }
                    if ((cnt == 0) || bl1) {
                        if (cnt == 0) {
                            /* DC component */
                            dcac = *component->dcv + ebits;
                            *component->dcv = dcac;
                        } else {
                            /* AC component */
                            dcac = ebits;
                        }

                        /* reverse zigzag */
                        // component->mcubuf[ZIGZAG[cnt]] = dcac;

                        i = ZIGZAG[cnt];
                        tmp[i] = dcac * component->qttbl[i] >> 8;

                        dbit -= bl1;
                        dreg <<= bl1;
                    }
                    cnt += 1;
                } else {
                    ZJD_LOG("EOB detected");
                    if (cnt == 1) {
                        /* all ac value are zero */
                        skip_idct = true;
                    }
                    zeros = 0;
                    cnt = 64;
                    ebits = 0;
                }

                // ZJD_LOG("Found Huffman code: %08X %u | %u %02X %d %d %d", dreg, dbit, bl0, val, *component->dcv, ebits, dcac);
                if (cnt == 64) {
                    cnt = 0;

                    if (skip_idct) {
                        skip_idct = false;
                        component->mcubuf[0] = (zjd_yuv_t)((tmp[0] >> 8) + 128);
                        for (i = 1; i < 64; i++) {
                            component->mcubuf[i] = component->mcubuf[0];
                        }
                    } else {
                        block_idct(tmp, component->mcubuf);
                    }

                    ZJD_LOG("MCU %d,%d, component %d:", mcu_rect->x, mcu_rect->y, cmp);
                    ZJD_INTDUMP(component->mcubuf, 64);
                    ZJD_LOG("");

                    cmp++;
                    if (cmp >= n_cmp) {
                        cmp = 0;

                        if (!zjd->yuv_scan(zjd, mcu_rect, NULL)) {
                            ZJD_LOG("Scan stopped by user");
                            return ZJD_OK;
                        }

                        mcu_rect->x += mcu_rect->w;
                        if (mcu_rect->x >= zjd->width) {
                            mcu_rect->x = 0;
                            mcu_rect->y += mcu_rect->h;
                            if (mcu_rect->y >= zjd->height) {
                                ZJD_LOG("All MCUs processed (%u padding bits: %X)", dbit, dreg >> (32 - dbit));
                                return ZJD_OK;
                            }
                        }

                        if (rst) {
                            rst = false;
                            dreg = 0;
                            dbit = 0;
                            bits_threshold = 15;
                            zjd->dcv[0] = 0;
                            zjd->dcv[1] = 0;
                            zjd->dcv[2] = 0;
                        }

                        /* save context */
                        ctx->offset = zjd->oft;
                        ctx->dreg = dreg;
                        ctx->dbit = dbit;
                        ctx->mcu_x = mcu_rect->x;
                        ctx->mcu_y = mcu_rect->y;
                        ctx->dcv[0] = zjd->dcv[0];
                        ctx->dcv[1] = zjd->dcv[1];
                        ctx->dcv[2] = zjd->dcv[2];
                        ZJD_LOG("MCU context updated: oft %u, dreg %08X, dbit %u, x %u, y %u, dcv %d %d %d",
                                ctx->offset, ctx->dreg, ctx->dbit,
                                ctx->mcu_x, ctx->mcu_y,
                                ctx->dcv[0], ctx->dcv[1], ctx->dcv[2]
                               );
                    }

                    component = &zjd->component[cmp];
                    memset(component->mcubuf, 0, 64 * sizeof(zjd_yuv_t));
                    memset(tmp, 0, 64 * 4);
                }
            }
        }
    }

    return ZJD_OK;
}

/* TODO: sub function of zjd_scan, skip as much decode process as possible to speed up  */
zjd_res_t zjd_scan_rect(zjd_t *zjd, const zjd_ctx_t *snapshot, const zjd_rect_t *tgt_rect)
{
    int32_t dc = 0;
    uint8_t *dp = zjd->buf;
    uint8_t last_d = 0, d = 0, dbit = 0, cnt = 0, cmp = 0, cls = 0, bl0, bl1, val, zeros;
    uint32_t dreg = 0;
    int ebits, dcac = 0;
    uint8_t bits_threshold = 15;
    int n_cmp;
    bool next_huff = true, rst = false;
    zjd_rect_t _mcu_rect, *mcu_rect = &_mcu_rect;

    zjd_comp_t *component = &zjd->component[cmp];
    zjd_ctx_t *ctx = &zjd->ctx;

    n_cmp = zjd->msy * zjd->msx;
    if (zjd->ncomp == 3) {
        n_cmp += 2;
    }

    mcu_rect->x = 0;
    mcu_rect->y = 0;
    mcu_rect->w = zjd->msx << 3;
    mcu_rect->h = zjd->msy << 3;
    memset(component->mcubuf, 0, 64 * sizeof(zjd_yuv_t));

    if (snapshot) {
        mcu_rect->x = snapshot->mcu_x;
        mcu_rect->y = snapshot->mcu_y;
        dbit = snapshot->dbit;
        dreg = snapshot->dreg;
        zjd->dcv[0] = snapshot->dcv[0];
        zjd->dcv[1] = snapshot->dcv[1];
        zjd->dcv[2] = snapshot->dcv[2];
        zjd->oft = snapshot->offset;
        zjd->pos = zjd->oft;
    } else {
        /* reset to the beginning of scan data */
        zjd->oft = zjd->imgoft;
        zjd->pos = zjd->oft;
        zjd->dcv[0] = 0;
        zjd->dcv[1] = 0;
        zjd->dcv[2] = 0;
    }

    /* take snapshot */
    ctx->offset = zjd->oft;
    ctx->dreg = dreg;
    ctx->dbit = dbit;
    ctx->mcu_x = mcu_rect->x;
    ctx->mcu_y = mcu_rect->y;
    ctx->dcv[0] = zjd->dcv[0];
    ctx->dcv[1] = zjd->dcv[1];
    ctx->dcv[2] = zjd->dcv[2];

    ZJD_LOG("roi mode");

    while (1) {
        if (zjd->oft >= zjd->pos) {
            dp = zjd->buf; /* Top of input buffer */
            dc = zjd->ifunc(zjd, zjd->buf, zjd->oft, zjd->buflen);
            if (!dc) {
                ZJD_LOG("No more data, %d", dbit);
                return ZJD_ERR_LEN_SOS;
            }
            zjd->pos = zjd->oft + dc;
        }

        // if (dbit > 24) {
        //     ZJD_LOG("No more buffer");
        //     return ZJD_ERR_SCAN_SLOW;
        // }

        d = *dp++;
        zjd->oft++;

        if (d == 0xFF) {
            last_d = d;
            continue;
        } else if (last_d == 0xFF) {
            last_d = 0;
            ZJD_LOG("Found marker %02X", d);
            if (d == 0x00) {
                ZJD_LOG("Padding byte");
                dreg = (dreg & ~(0xFFFFFFFF >> dbit)) | ((uint32_t)0xFF << (32 - 8 - dbit));
                dbit += 8;
            } else {
                if (d == 0xD9) {
                    ZJD_LOG("EOI marker");
                    bits_threshold = 0;
                } else if ((d >= 0xD0) && (d <= 0xD7)) {
                    ZJD_LOG("RST marker %02X", d);
                    bits_threshold = 0;
                    rst = true;
                } else {
                    continue;
                }
            }
        } else {
            dreg = (dreg & ~(0xFFFFFFFF >> dbit)) | ((uint32_t)d << (32 - 8 - dbit));
            dbit += 8;
        }

        ZJD_LOG("Buffer: %08X %u %02X", dreg, dbit, d);

        while (dbit > bits_threshold) {
            if (next_huff) {
                next_huff = !next_huff;

                /* 0: DC, others: AC */
                cls = !!cnt;

                ZJD_LOG("(x: %d, y: %d), cmp %u, %s table, cls %d, cnt %d, dreg %08X, dbit %u",
                        mcu_rect->x, mcu_rect->y, cmp, cls == 0 ? "DC" : "AC", cls, cnt, dreg, dbit);

                bl0 = zjd_get_hc(&component->huff[cls], dreg, dbit, &val);
                if (!bl0) {
                    ZJD_LOG("bl0 Huffman code too short: %08X %u", dreg, dbit);
                    return ZJD_ERR_FMT1;
                }

                dbit -= bl0;
                dreg <<= bl0;

                ZJD_LOG("processing huff, bl0 %d, val %02X", bl0, val);

                /* continue to check if need to load dreg again */
                if (val != 0) {
                    continue;
                }
            }

            if (!next_huff) {
                next_huff = !next_huff;

                ZJD_LOG("processing bits, cnt %d val %02X", cnt, val);

                if ((val != 0) || (cnt == 0)) {
                    zeros = val >> 4;
                    bl1 = val & 0x0F;

                    if (dbit < bl1) {
                        ZJD_LOG("bl1 Huffman code too short: %08X %u < %u", dreg, dbit, bl1);
                        return ZJD_ERR_FMT1;
                    }

                    cnt += zeros;

                    if (bl1) {
                        ebits = (int)(dreg >> (32 - bl1));
                        if (!(dreg & 0x80000000)) {
                            ebits -= (1 << bl1) - 1;    /* Restore negative value if needed */
                        }
                    } else {
                        ebits = 0;
                    }
                    if ((cnt == 0) || bl1) {
                        if (cnt == 0) {
                            /* DC component */
                            dcac = *component->dcv + ebits;
                            *component->dcv = dcac;
                        } else {
                            /* AC component */
                            dcac = ebits;
                        }

                        /* reverse zigzag */
                        component->mcubuf[ZIGZAG[cnt]] = dcac;

                        dbit -= bl1;
                        dreg <<= bl1;
                    }
                    cnt += 1;
                } else {
                    /* EOB detected */
                    zeros = 0;
                    cnt = 64;
                    ebits = 0;
                }

                ZJD_LOG("Found Huffman code: %08X %u | %u %02X %d %d %d", dreg, dbit, bl0, val, *component->dcv, ebits, dcac);
                if (cnt == 64) {
                    cnt = 0;

                    ZJD_INTDUMP(component->mcubuf, 64);
                    ZJD_LOG("");

                    cmp++;
                    if (cmp >= n_cmp) {
                        cmp = 0;

                        if (tgt_rect == NULL) {
                            zjd_mcu_scan(zjd, n_cmp, mcu_rect, tgt_rect);
                        } else {
                            if (is_rect_intersect(mcu_rect, tgt_rect)) {
                                ZJD_LOG("MCU intersects with output rectangle (%u,%u,%u,%u) (%u,%u,%u,%u)\n",
                                        mcu_rect->x, mcu_rect->y, mcu_rect->w, mcu_rect->h,
                                        tgt_rect->x, tgt_rect->y, tgt_rect->w, tgt_rect->h);
                                zjd_mcu_scan(zjd, n_cmp, mcu_rect, tgt_rect);

                                if (is_r0_beyond_r1(mcu_rect, tgt_rect)) {
                                    ZJD_LOG("ROI MCUs processed\n");
                                    return ZJD_OK;
                                }
                            }
                        }

                        mcu_rect->x += mcu_rect->w;
                        if (mcu_rect->x >= zjd->width) {
                            mcu_rect->x = 0;
                            mcu_rect->y += mcu_rect->h;
                            if (mcu_rect->y >= zjd->height) {
                                ZJD_LOG("All MCUs processed (%u padding bits: %X)", dbit, dreg >> (32 - dbit));
                                return ZJD_OK;
                            }
                        }

                        if (rst) {
                            rst = false;
                            dreg = 0;
                            dbit = 0;
                            bits_threshold = 15;
                            zjd->dcv[0] = 0;
                            zjd->dcv[1] = 0;
                            zjd->dcv[2] = 0;
                        }

                        /* save context */
                        ctx->offset = zjd->oft;
                        ctx->dreg = dreg;
                        ctx->dbit = dbit;
                        ctx->mcu_x = mcu_rect->x;
                        ctx->mcu_y = mcu_rect->y;
                        ctx->dcv[0] = zjd->dcv[0];
                        ctx->dcv[1] = zjd->dcv[1];
                        ctx->dcv[2] = zjd->dcv[2];
                        ZJD_LOG("MCU context updated: oft %u, dreg %08X, dbit %u, x %u, y %u, dcv %d %d %d",
                                ctx->offset, ctx->dreg, ctx->dbit,
                                ctx->mcu_x, ctx->mcu_y,
                                ctx->dcv[0], ctx->dcv[1], ctx->dcv[2]
                               );
                    }

                    component = &zjd->component[cmp];
                    memset(component->mcubuf, 0, 64 * sizeof(zjd_yuv_t));
                }
            }
        }
    }

    return ZJD_OK;
}

/* snapshot: used to control where to start, and where to resume
   tgt_rect: target rectangle is used for  */
zjd_res_t zjd_scan(zjd_t *zjd, const zjd_ctx_t *snapshot, const zjd_rect_t *tgt_rect)
{
    if (snapshot == NULL && tgt_rect == NULL) {
        return zjd_scan_full(zjd);
    } else {
        return zjd_scan_rect(zjd, snapshot, tgt_rect);
    }
}

void zjd_save(const zjd_t *zjd, zjd_ctx_t *snapshot)
{
    memcpy(snapshot, &zjd->ctx, sizeof(zjd_ctx_t));
}
