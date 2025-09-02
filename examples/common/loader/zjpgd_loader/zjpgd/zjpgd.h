/*
 * Copyright (c) 2025 Jiapeng Li <mail@jiapeng.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ZJPGD_H__
#define __ZJPGD_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

//#include "zjpgdcfg.h"
#include "arm_zjpgd_cfg.h"

#define ZJD_VER_MAJOR           0
#define ZJD_VER_MINOR           1
#define ZJD_VER_PATCH           2
#define ZJD_VERSION             (ZJD_VER_MAJOR << 16 | ZJD_VER_MINOR << 8 | ZJD_VER_PATCH)

typedef enum {
    ZJD_GRAYSCALE    = 0,    // 1B
    ZJD_RGB565       = 1,    // 2B
    ZJD_BGR565       = 2,    // 2B
    ZJD_RGB888       = 3,    // 3B
    ZJD_BGR888       = 4,    // 3B
    ZJD_RGBA8888     = 5,    // 4B
    ZJD_BGRA8888     = 6,    // 4B
} zjd_outfmt_t;

typedef enum {
    ZJD_OK = 0,
    ZJD_ERR_LEN = -1,
    ZJD_ERR_LEN0 = -2,
    ZJD_ERR_LEN1 = -3,

    ZJD_ERR_FMT0 = -4,
    ZJD_ERR_FMT1 = -5,
    ZJD_ERR_FMT2 = -6,

    ZJD_ERR_OOM_QTTBL = -7,
    ZJD_ERR_LEN_QTTBL = -8,
    ZJD_ERR_FMT_QTTBL = -9,

    ZJD_ERR_LEN_SOS = -10,

    ZJD_ERR_OOM_HFMTBL1 = -11,
    ZJD_ERR_LEN_HFMTBL1 = -12,
    ZJD_ERR_FMT_HFMTBL1 = -13,

    ZJD_ERR_OOM_HFMTBL2 = -14,
    ZJD_ERR_LEN_HFMTBL2 = -15,
    ZJD_ERR_FMT_HFMTBL2 = -16,

    ZJD_ERR_OOM_HFMTBL3 = -17,
    ZJD_ERR_LEN_HFMTBL3 = -18,
    ZJD_ERR_FMT_HFMTBL3 = -19,

    ZJD_ERR_OOM0 = -20,
    ZJD_ERR_OOM1 = -21,

    ZJD_ERR_OOM_SOF = -22,
    ZJD_ERR_LEN_SOF = -23,
    ZJD_ERR_FMT_SOF = -24,
    ZJD_ERR_FMT3 = -25,

    ZJD_ERR_YUV = -26,

    ZJD_ERR_MARKER_UNKNOWN = -27,

    ZJD_ERR_MARKER_NOT_SUPPORTED = -28,
    ZJD_ERR_SOI = -29,

    ZJD_ERR_SCAN_SLOW = -30,
    ZJD_ERR_PARA = -31,

    ZJD_ERR_OOM2 = -32,
    ZJD_ERR_FMT4 = -33,

} zjd_res_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} zjd_rect_t;

typedef struct {
    uint8_t *huffbits[2][2];    /* Huffman bit distribution tables [id][dcac] */
    uint16_t *huffcode[2][2];   /* Huffman code word tables [id][dcac] */
    uint8_t *huffdata[2][2];    /* Huffman decoded data tables [id][dcac] */
    int32_t *qttbl[4];          /* Dequantizer tables [id] */
    uint8_t qtid[3];            /* Quantization table ID of each component, Y, Cb, Cr */
} zjd_tbl_t;

typedef struct {
    uint8_t *bits;
    uint16_t *code;
    uint8_t *data;
} zjd_huff_t;

typedef int16_t zjd_yuv_t;

typedef struct {
    zjd_huff_t huff[2];
    int32_t *qttbl;
    int16_t *dcv;
    zjd_yuv_t *mcubuf;
} zjd_comp_t;

typedef struct zjd zjd_t;

typedef int (*zjd_ifunc_t)(zjd_t *, uint8_t *buf, uint32_t addr, int len);
typedef int (*zjd_ofunc_t)(zjd_t *, zjd_rect_t *rect, void *pixels);
typedef int (*zjd_yuv_scan_t)(zjd_t *, zjd_rect_t *mcu_rect, const zjd_rect_t *tgt_rect);
typedef void (*zjd_yuv2pix_t)(uint8_t **pix, int yy, int cb, int cr);

typedef struct {
    uint32_t offset;
    uint32_t dreg;
    int8_t dbit;
    uint16_t mcu_x;
    uint16_t mcu_y;
    int16_t dcv[3];
} zjd_ctx_t;

typedef struct {
    zjd_outfmt_t outfmt;
    zjd_ifunc_t ifunc;
    zjd_ofunc_t ofunc;
    void *buf;
    int32_t buflen;
    void *arg;
} zjd_cfg_t;

struct zjd {
    zjd_ctx_t ctx;
    zjd_comp_t component[6];
    int16_t dcv[3];

    uint8_t msx;
    uint8_t msy;
    uint8_t ncomp;
    uint16_t width;
    uint16_t height;
    uint32_t imgoft;

    uint32_t oft;
    uint32_t pos;
    int16_t buflen;
    void *buf;

    zjd_yuv_scan_t yuv_scan;
    zjd_yuv2pix_t yuv2pix;

    zjd_ifunc_t ifunc;
    zjd_ofunc_t ofunc;

    void *workbuf;
    zjd_yuv_t *mcubuf;
    void *arg;
};

zjd_res_t zjd_init(zjd_t *zjd, const zjd_cfg_t *cfg);
zjd_res_t zjd_scan(zjd_t *zjd, const zjd_ctx_t *snapshot, const zjd_rect_t *tgt_rect);
void zjd_save(const zjd_t *zjd, zjd_ctx_t *snapshot);

#endif /* __ZJPGD_H__ */
