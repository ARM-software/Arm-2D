/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "arm-2d", "index.html", [
    [ "Header Files Summary", "md_E__git_EndpointAI_Kernels_Research_Arm_2D_Library_Include_README.html", [
      [ "1 Overview", "md_E__git_EndpointAI_Kernels_Research_Arm_2D_Library_Include_README.html#autotoc_md1", null ],
      [ "2 Public Header Files", "md_E__git_EndpointAI_Kernels_Research_Arm_2D_Library_Include_README.html#autotoc_md2", null ],
      [ "3 Private Header Files", "md_E__git_EndpointAI_Kernels_Research_Arm_2D_Library_Include_README.html#autotoc_md3", null ],
      [ "4 Templates", "md_E__git_EndpointAI_Kernels_Research_Arm_2D_Library_Include_README.html#autotoc_md4", null ]
    ] ],
    [ "How to Use Tile Operations", "md_how_to_use_tile_operations.html", [
      [ "1 Overview", "md_how_to_use_tile_operations.html#autotoc_md6", null ],
      [ "2 Key Scenarios, Patterns and Use Cases", "md_how_to_use_tile_operations.html#autotoc_md7", [
        [ "2.1 Normal Usage", "md_how_to_use_tile_operations.html#autotoc_md8", [
          [ "2.1.1 Window/Panel Clipping", "md_how_to_use_tile_operations.html#autotoc_md9", null ],
          [ "2.1.2 Texture Paving", "md_how_to_use_tile_operations.html#autotoc_md10", null ]
        ] ],
        [ "2.2 Advanced Usage", "md_how_to_use_tile_operations.html#autotoc_md11", [
          [ "2.2.1 Partial Frame Buffer (PFB)", "md_how_to_use_tile_operations.html#autotoc_md12", null ]
        ] ]
      ] ],
      [ "3 Design Considerations", "md_how_to_use_tile_operations.html#autotoc_md13", null ],
      [ "4 API Table", "md_how_to_use_tile_operations.html#autotoc_md14", [
        [ "4.1 Basic", "md_how_to_use_tile_operations.html#autotoc_md15", [
          [ "4.1.1 <strong>arm_2d_tile_generate_child</strong>", "md_how_to_use_tile_operations.html#autotoc_md16", null ],
          [ "4.1.2 <strong>arm_2d_tile_get_root</strong>", "md_how_to_use_tile_operations.html#autotoc_md17", null ],
          [ "4.1.3 <strong>arm_2d_is_root_tile</strong>", "md_how_to_use_tile_operations.html#autotoc_md18", null ],
          [ "4.1.4 <strong>arm_2d_region_intersect</strong>", "md_how_to_use_tile_operations.html#autotoc_md19", null ],
          [ "4.1.5 <strong>arm_2d_get_address_and_region_from_tile</strong>", "md_how_to_use_tile_operations.html#autotoc_md20", null ]
        ] ],
        [ "4.2 Copy, Fill and X/Y Mirroring", "md_how_to_use_tile_operations.html#autotoc_md21", [
          [ "4.2.1 <strong>arm_2d_rgb16_tile_copy</strong>", "md_how_to_use_tile_operations.html#autotoc_md22", null ],
          [ "4.2.2 <strong>arm_2d_rgb32_tile_copy</strong>", "md_how_to_use_tile_operations.html#autotoc_md23", null ]
        ] ],
        [ "4.3 Colour Keying", "md_how_to_use_tile_operations.html#autotoc_md24", [
          [ "4.3.1 arm_2d_c8bit_tile_copy_with_colour_keying", "md_how_to_use_tile_operations.html#autotoc_md25", null ],
          [ "4.3.2 arm_2d_rbg16_tile_copy_with_colour_keying", "md_how_to_use_tile_operations.html#autotoc_md26", null ],
          [ "4.3.3 arm_2d_rbg32_tile_copy_with_colour_keying", "md_how_to_use_tile_operations.html#autotoc_md27", null ]
        ] ]
      ] ],
      [ "5 Reference", "md_how_to_use_tile_operations.html#autotoc_md28", null ]
    ] ],
    [ "Introduction for Arm-2D", "md_Introduction.html", [
      [ "1 Overview", "md_Introduction.html#autotoc_md30", [
        [ "1.1 What is Arm-2D", "md_Introduction.html#autotoc_md31", null ],
        [ "1.2 Target Audiences", "md_Introduction.html#autotoc_md32", [
          [ "1.2.1 GUI Service Provider", "md_Introduction.html#autotoc_md33", null ],
          [ "1.2.2 Silicon Vendor", "md_Introduction.html#autotoc_md34", null ],
          [ "1.2.3 Bare-metal System Developers", "md_Introduction.html#autotoc_md35", null ]
        ] ],
        [ "1.3 Arm-2D Libraries", "md_Introduction.html#autotoc_md36", [
          [ "1.3.1 Standard Software Implementation", "md_Introduction.html#autotoc_md37", null ],
          [ "1.3.2 Helium Acceleration", "md_Introduction.html#autotoc_md38", null ],
          [ "1.3.4 Third-party Implementation", "md_Introduction.html#autotoc_md39", null ],
          [ "1.3.5 Arm Custom Instruction Support", "md_Introduction.html#autotoc_md40", null ]
        ] ],
        [ "1.4 Scope and Limitations", "md_Introduction.html#autotoc_md41", [
          [ "1.4.1 Scope", "md_Introduction.html#autotoc_md42", null ],
          [ "1.4.2 Limitations", "md_Introduction.html#autotoc_md43", null ]
        ] ],
        [ "1.5 Operation Categories", "md_Introduction.html#autotoc_md44", null ],
        [ "1.6 Folder Structures", "md_Introduction.html#autotoc_md45", null ]
      ] ],
      [ "2 Basics", "md_Introduction.html#autotoc_md46", [
        [ "2.1 Region", "md_Introduction.html#autotoc_md47", [
          [ "2.1.1 Location", "md_Introduction.html#autotoc_md48", null ],
          [ "2.1.2 Size", "md_Introduction.html#autotoc_md49", null ]
        ] ],
        [ "2.2 Boxing Model", "md_Introduction.html#autotoc_md50", [
          [ "2.2.1 Absolute Location and Relative Location", "md_Introduction.html#autotoc_md51", null ],
          [ "2.2.2 Absolute Region and Relative Region", "md_Introduction.html#autotoc_md52", null ]
        ] ],
        [ "2.3 Tile", "md_Introduction.html#autotoc_md53", [
          [ "2.3.1 Root Tile", "md_Introduction.html#autotoc_md54", null ],
          [ "2.3.2 Child Tile", "md_Introduction.html#autotoc_md55", null ],
          [ "2.3.3 Partial Frame Buffer", "md_Introduction.html#autotoc_md56", null ]
        ] ],
        [ "2.4 Colour", "md_Introduction.html#autotoc_md57", null ],
        [ "2.4 API Usage Modes", "md_Introduction.html#autotoc_md58", [
          [ "2.4.1 Synchronous Mode", "md_Introduction.html#autotoc_md59", null ],
          [ "2.4.2 Asynchronous Mode", "md_Introduction.html#autotoc_md60", null ]
        ] ]
      ] ],
      [ "3 API Summary", "md_Introduction.html#autotoc_md61", [
        [ "3.1 Tile Operations", "md_Introduction.html#autotoc_md62", null ],
        [ "3.2 Colour Space Conversion", "md_Introduction.html#autotoc_md63", null ],
        [ "3.3 Alpha Blending", "md_Introduction.html#autotoc_md64", null ],
        [ "3.4 Mirroring/Rotation", "md_Introduction.html#autotoc_md65", null ],
        [ "3.5 Zooming/Stretching", "md_Introduction.html#autotoc_md66", null ],
        [ "3.6 Drawing", "md_Introduction.html#autotoc_md67", null ],
        [ "3.7 Filters", "md_Introduction.html#autotoc_md68", null ]
      ] ]
    ] ],
    [ "How to Read Those Documents", "md_README.html", [
      [ "@ref E:/git/EndpointAI/Kernels/Research/Arm-2D/documents/Introduction.md \"Introduction for Arm-2D\"", "md_README.html#autotoc_md70", null ],
      [ "@ref E:/git/EndpointAI/Kernels/Research/Arm-2D/documents/how_to_use_tile_operations.md \"How to Use Tile Operations\"", "md_README.html#autotoc_md71", null ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", "globals_defs" ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"____arm__2d__alpha__blending_8inc.html",
"arm__2d__async_8c_source.html",
"arm__2d__transform_8c.html#abfe25527c148bf09da32f1a3136dd135",
"arm__2d__types_8h.html#adf6d1383c1b6c20dff66fbe15effdb84",
"structarm__2d__op__alpha__cl__key__t.html#a13d5bfa95c504d60a781f857e70d3e83",
"unionarm__2d__color__rgba8888__t.html#abf716ea760145003e1846f85e1bfd55f"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';