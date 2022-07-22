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
  [ "Arm-2D", "index.html", [
    [ "README", "index.html", [
      [ "Branch Summary", "index.html#autotoc_md0", null ],
      [ "Features", "index.html#autotoc_md1", [
        [ "Planned in the Future", "index.html#autotoc_md3", null ]
      ] ],
      [ "1 Introduction", "index.html#autotoc_md4", [
        [ "1.1 The Background", "index.html#autotoc_md5", null ],
        [ "1.2 The Problems in current solutions", "index.html#autotoc_md6", null ],
        [ "1.3 The Platform", "index.html#autotoc_md7", null ],
        [ "1.4 Dependency", "index.html#autotoc_md8", null ],
        [ "1.5 Examples, Benchmark and Templates", "index.html#autotoc_md9", [
          [ "1.5.1 Summary", "index.html#autotoc_md10", null ],
          [ "1.5.2 Benchmark", "index.html#autotoc_md11", null ]
        ] ]
      ] ],
      [ "2 Folder Hierarchy", "index.html#autotoc_md12", null ],
      [ "3 Tips For Exploring the Library", "index.html#autotoc_md13", [
        [ "3.1 \"I am a library user, I only care about how to use the library\"", "index.html#autotoc_md14", null ],
        [ "3.2 \"I am interested in the implementation\"", "index.html#autotoc_md15", null ]
      ] ],
      [ "4 Documentation", "index.html#autotoc_md16", null ],
      [ "5 Limitations", "index.html#autotoc_md17", [
        [ "5.1 The Generic Limitations", "index.html#autotoc_md18", null ],
        [ "5.2 The Temporary Limitations", "index.html#autotoc_md19", null ]
      ] ],
      [ "6 Feedback", "index.html#autotoc_md20", null ]
    ] ],
    [ "How to Deploy the Arm-2D Library", "deploy.html", [
      [ "1 How to Get the Latest Arm-2D Library", "deploy.html#autotoc_md21", null ],
      [ "2 How to Deploy the Library", "deploy.html#autotoc_md22", [
        [ "2.1 Deploy to an MDK project", "deploy.html#autotoc_md23", null ]
      ] ],
      [ "3 Other Useful information", "deploy.html#autotoc_md24", [
        [ "3.1 Examples", "deploy.html#autotoc_md25", null ],
        [ "3.2 How To Read the Library In Users' View", "deploy.html#autotoc_md26", null ]
      ] ]
    ] ],
    [ "Introduction for Arm-2D", "intro.html", [
      [ "1 Overview", "intro.html#autotoc_md27", [
        [ "1.1 What is Arm-2D", "intro.html#autotoc_md28", null ],
        [ "1.2 Target Audiences", "intro.html#autotoc_md29", [
          [ "1.2.1 GUI Service Provider", "intro.html#autotoc_md30", null ],
          [ "1.2.2 Silicon Vendor", "intro.html#autotoc_md31", null ],
          [ "1.2.3 Embedded Software Developers", "intro.html#autotoc_md32", null ]
        ] ],
        [ "1.3 Arm-2D Libraries", "intro.html#autotoc_md33", [
          [ "1.3.1 Standard Software Implementation", "intro.html#autotoc_md34", null ],
          [ "1.3.2 Helium Acceleration", "intro.html#autotoc_md35", null ],
          [ "1.3.4 Third-party Implementation", "intro.html#autotoc_md36", null ],
          [ "1.3.5 Arm Custom Instruction Support", "intro.html#autotoc_md37", null ]
        ] ],
        [ "1.4 Scope and Limitations", "intro.html#autotoc_md38", [
          [ "1.4.1 Scope", "intro.html#autotoc_md39", null ],
          [ "1.4.2 Limitations", "intro.html#autotoc_md40", null ]
        ] ],
        [ "1.5 Operation Categories", "intro.html#autotoc_md41", null ],
        [ "1.6 Folder Structures", "intro.html#autotoc_md42", null ]
      ] ],
      [ "2 Basics", "intro.html#autotoc_md43", [
        [ "2.1 Region", "intro.html#autotoc_md44", [
          [ "2.1.1 Location", "intro.html#autotoc_md45", null ],
          [ "2.1.2 Size", "intro.html#autotoc_md46", null ]
        ] ],
        [ "2.2 Boxing Model", "intro.html#autotoc_md47", [
          [ "2.2.1 Absolute Location and Relative Location", "intro.html#autotoc_md48", null ],
          [ "2.2.2 Absolute Region and Relative Region", "intro.html#autotoc_md49", null ]
        ] ],
        [ "2.3 Tile", "intro.html#autotoc_md50", [
          [ "2.3.1 Root Tile", "intro.html#autotoc_md51", null ],
          [ "2.3.2 Child Tile", "intro.html#autotoc_md52", null ],
          [ "2.3.3 Partial Frame Buffer", "intro.html#autotoc_md53", null ]
        ] ],
        [ "2.4 Colour", "intro.html#autotoc_md54", null ],
        [ "2.4 API Usage Modes", "intro.html#autotoc_md55", [
          [ "2.4.1 Synchronous Mode", "intro.html#autotoc_md56", null ],
          [ "2.4.2 Asynchronous Mode", "intro.html#autotoc_md57", null ]
        ] ]
      ] ],
      [ "3 API Summary", "intro.html#autotoc_md58", [
        [ "3.1 Tile Operations", "intro.html#autotoc_md59", null ],
        [ "3.2 Colour Space Conversion", "intro.html#autotoc_md60", null ],
        [ "3.3 Alpha Blending", "intro.html#autotoc_md61", null ],
        [ "3.4 Transform (Rotation/Scaling)", "intro.html#autotoc_md62", null ],
        [ "3.5 Drawing", "intro.html#autotoc_md63", null ],
        [ "3.6 Filters", "intro.html#autotoc_md64", null ]
      ] ]
    ] ],
    [ "How to Use Tile Operations", "use_tile.html", [
      [ "1 Overview", "use_tile.html#autotoc_md65", null ],
      [ "2 Key Scenarios, Patterns and Use Cases", "use_tile.html#autotoc_md66", [
        [ "2.1 Normal Usage", "use_tile.html#autotoc_md67", [
          [ "2.1.1 Window/Panel Clipping", "use_tile.html#autotoc_md68", null ],
          [ "2.1.2 Texture Paving", "use_tile.html#autotoc_md69", null ]
        ] ],
        [ "2.2 Advanced Usage", "use_tile.html#autotoc_md70", [
          [ "2.2.1 Partial Frame Buffer (PFB)", "use_tile.html#autotoc_md71", null ]
        ] ]
      ] ],
      [ "3 Design Considerations", "use_tile.html#autotoc_md72", null ],
      [ "4 API Table", "use_tile.html#autotoc_md73", [
        [ "4.1 Basic", "use_tile.html#autotoc_md74", [
          [ "4.1.1 <strong>arm_2d_tile_generate_child</strong>", "use_tile.html#autotoc_md75", null ],
          [ "4.1.2 <strong>arm_2d_tile_get_root</strong>", "use_tile.html#autotoc_md76", null ],
          [ "4.1.3 <strong>arm_2d_is_root_tile</strong>", "use_tile.html#autotoc_md77", null ],
          [ "4.1.4 <strong>arm_2d_region_intersect</strong>", "use_tile.html#autotoc_md78", null ],
          [ "4.1.5 <strong>arm_2d_get_address_and_region_from_tile</strong>", "use_tile.html#autotoc_md79", null ]
        ] ],
        [ "4.2 Copy, Fill and X/Y Mirroring", "use_tile.html#autotoc_md80", [
          [ "4.2.1 <strong>arm_2d_rgb16_tile_copy</strong>", "use_tile.html#autotoc_md81", null ],
          [ "4.2.2 <strong>arm_2d_rgb32_tile_copy</strong>", "use_tile.html#autotoc_md82", null ]
        ] ],
        [ "4.3 Colour Keying", "use_tile.html#autotoc_md83", [
          [ "4.3.1 arm_2d_c8bit_tile_copy_with_colour_keying", "use_tile.html#autotoc_md84", null ],
          [ "4.3.2 arm_2d_rbg16_tile_copy_with_colour_keying", "use_tile.html#autotoc_md85", null ],
          [ "4.3.3 arm_2d_rbg32_tile_copy_with_colour_keying", "use_tile.html#autotoc_md86", null ]
        ] ]
      ] ],
      [ "5 Reference", "use_tile.html#autotoc_md87", null ]
    ] ],
    [ "Header Files Summary", "headers.html", [
      [ "1 Overview", "headers.html#autotoc_md88", null ],
      [ "2 Public Header Files", "headers.html#autotoc_md89", null ],
      [ "3 Private Header Files", "headers.html#autotoc_md90", null ],
      [ "4 Templates", "headers.html#autotoc_md91", null ]
    ] ],
    [ "Reference", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"deploy.html",
"group__gHelper.html#ga10a29fc878fb8adb0c542589d1b602cf",
"group__gKernel.html#structarm__2d__op__status__t_8____unnamed44____"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';