/*----------------------------------------------*/
/* Qoi Decoder System Configurations 1.0.0      */
/*----------------------------------------------*/

#ifndef __ARM_2D_QOI_CFG_H__
#define __ARM_2D_QOI_CFG_H__

#include "arm_2d.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>QOI System Configurations
// =======================

// <o> The QOI IO Buffer Size <0-65535>
// <i> The size of the IO buffer
// <i> Default: 64
#ifndef ARM_QOI_IO_BUFF_SIZE
#   define ARM_QOI_IO_BUFF_SIZE         64
#endif

// <q>Use Loader IO
// <i> This feature is enabled by default. If the compressed image is accessible inside the 4G memory space, please disable this option.
// <i> When this feature is disabled, the ARM_QOI_IO_BUFFER_SIZE is ignored. 
#ifndef __ARM_QOI_USE_LOADER_IO__
#   define __ARM_QOI_USE_LOADER_IO__    1
#endif

//</h>

// <<< end of configuration section >>> 

#if !__ARM_QOI_USE_LOADER_IO__
#   undef ARM_QOI_IO_BUFF_SIZE
#   define ARM_QOI_IO_BUFF_SIZE     0
#endif

#if     __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_GRAY8
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_RGB565
#elif   __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define ARM_QOI_FORMAT   ARM_QOI_DEC_FORMAT_CCCN888
#endif

#endif


