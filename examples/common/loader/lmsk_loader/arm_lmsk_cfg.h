/*----------------------------------------------*/
/* LMSK Decoder System Configurations 1.0.0      */
/*----------------------------------------------*/

#ifndef __ARM_2D_LMSK_CFG_H__
#define __ARM_2D_LMSK_CFG_H__

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>LMSK System Configurations
// =======================

// <q>Use Loader IO
// <i> This feature is enabled by default. If the compressed image is accessible inside the 4G memory space, please disable this option.
#ifndef __ARM_LMSK_USE_LOADER_IO__
#   define __ARM_LMSK_USE_LOADER_IO__    1
#endif

// <o>The policy for compenstate the LSB
//     <0=>  No Compenstation
//     <1=>  Only compensate the maximum alpha
//     <2=>  Linear Interpolate
// <i> The policy for inserting LSB bits when the Alpha MSB count is less than 8
#ifndef __ARM_LMSK_LSB_COMPENSATION_POLICY__
#   define __ARM_LMSK_LSB_COMPENSATION_POLICY__     2
#endif

//</h>

// <<< end of configuration section >>> 

#endif


