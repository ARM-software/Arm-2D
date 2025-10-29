/*
 * Copyright (c) 2025 Jiapeng Li <mail@jiapeng.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ZJPGDCFG_H__
#define __ZJPGDCFG_H__

#ifdef ZJD_USR_CFG
#   include ZJD_USR_CFG
#endif

#ifndef ZJD_MEM_POOL_SZ
#define ZJD_MEM_POOL_SZ                 (1024 * 3)
#endif

#ifndef ZJD_DEBUG
#   define ZJD_DEBUG                    0
#endif

#if ZJD_DEBUG
#   include <stdio.h>

#   ifndef ZJD_LOG
#       define ZJD_LOG(x...)                 do { \
            printf(x); printf("\n"); \
        } while(0)
#   endif

#   ifndef ZJD_HEXDUMP
#       define ZJD_HEXDUMP(x, y)             do { \
            for (int i = 0; i < y; i++) { \
                if (i && i % 16 == 0) printf("\n"); \
                printf("%02X ", ((uint8_t*)x)[i]); \
            } \
            printf("\n"); \
        } while(0)
#   endif

#   ifndef ZJD_INTDUMP
#       define ZJD_INTDUMP(x, y)             do { \
            for (int i = 0; i < y; i++) { \
                if (i && i % 8 == 0) printf("\n"); \
                printf("%5d ", x[i]); \
            } \
            printf("\n"); \
        } while(0)
#endif

#   ifndef ZJD_RGBDUMP
#       define ZJD_RGBDUMP(x, y)             do { \
            for (int i = 0; i < y / 3; i++) { \
                if (i && i % 8 == 0) printf("\n"); \
                printf("(%3d,%3d,%3d) ", x[i * 3], x[i * 3 + 1], x[i * 3 + 2]); \
            } \
            printf("\n"); \
        } while(0)
#   endif
#else
#   define ZJD_LOG(x...)                do {} while(0)
#   define ZJD_HEXDUMP(x, y)            do {} while(0)
#   define ZJD_INTDUMP(x, y)            do {} while(0)
#   define ZJD_RGBDUMP(x, y)            do {} while(0)
#endif

#endif /* __ZJPGDCFG_H__ */
