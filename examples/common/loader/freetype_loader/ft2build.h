/****************************************************************************
 *
 * ft2build.h
 *
 *   FreeType 2 build and setup macros.
 *
 * Copyright (C) 1996-2025 by
 * David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */


  /**************************************************************************
   *
   * This is the 'entry point' for FreeType header file inclusions, to be
   * loaded before all other header files.
   *
   * A typical example is
   *
   * ```
   *   #include <ft2build.h>
   *   #include <freetype/freetype.h>
   * ```
   *
   */

#ifndef __ARM_2D_FT2BUILD_H__
#define __ARM_2D_FT2BUILD_H__

//#warning using customized freetype 

#define FT_CONFIG_OPTIONS_H  <custom/ftoption.h>
#define FT_CONFIG_MODULES_H  <custom/ftmodule.h>

#include <freetype/config/ftheader.h>

#endif /* __ARM_2D_FT2BUILD_H__ */


/* END */
