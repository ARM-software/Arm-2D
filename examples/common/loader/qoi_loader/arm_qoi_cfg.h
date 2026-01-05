/*----------------------------------------------*/
/* Qoi Decoder System Configurations 1.0.0      */
/*----------------------------------------------*/

#ifndef __ARM_2D_QOI_CFG_H__
#define __ARM_2D_QOI_CFG_H__

#include "arm_2d_cfg.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Qoi System Configurations
// =======================

// <o>Width of the screen <0-65535>
// <i> The size of the IO buffer
// <i> Default: 64
#ifndef ARM_QOI_IO_BUFF_SIZE
#   define ARM_QOI_IO_BUFF_SIZE         64
#endif

//</h>

// <<< end of configuration section >>>


#if     __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_GRAY8
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_RGB565
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_CCCN888
#endif

#endif


