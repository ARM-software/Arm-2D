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

#endif


