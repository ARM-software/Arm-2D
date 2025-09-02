/*----------------------------------------------*/
/* ZJpgD System Configurations 0.1.2            */
/*----------------------------------------------*/

#ifndef __ARM_2D_ZJPGD_CFG_H__
#define __ARM_2D_ZJPGD_CFG_H__

#include "arm_2d_cfg.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>ZJpgD System Configurations
// =======================


//</h>

// <<< end of configuration section >>>


#ifndef ZJD_MEM_POOL_SZ
#define ZJD_MEM_POOL_SZ                 (1024 * 3)
#endif

#if     __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define ZJD_FORMAT   ZJD_GRAYSCALE
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define ZJD_FORMAT   ZJD_RGB565
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define ZJD_FORMAT   ZJD_BGRA8888
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




#endif


