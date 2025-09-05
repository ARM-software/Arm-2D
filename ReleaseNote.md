# Release Note



## Release v1.2.4 (September 2025)

### Core and Library

* Adds new APIs: **tile-fill-with-source-mask-and-opacity** with the Helium acceleration and the implicit CCCA8888 conversions.
* Adds new APIs: **tile-fill-with-opacit**y with Helium acceleration and the implicity CCCA8888 conversions. 
* Improves the support for CCCA8888
* Renames **tile-copy--with-opacity** to **tile-copy-with-opacity-only. **The old APIs are still available and marked deprecated. 
* Allows using negative scaling ratios in transform operations. Negative ratios lead to X and/or Y mirroring in transformation. 
* Slightly improves the performance of the tile-copy-with-colour-keying operations.
* Enhances the code quality
  * Fixes potential tail-prediction related issues in the Helium implementation.
  * Improves the inline assembly compatibility for different compilers. 
  * Fixes a potential shadow-variables issue in the blending intrinsics (macros).
* Other minor fixes



### Helper Services

* Enhances the PFB and the Display Adapter services.
  * Prevents PFB from overflowing when dealing with 4K or 8L display system.
  * Improves the 3FB mode 
  * Updates the Display Adapter Template
  * Improves the support for e-ink devices
    * Introduces a smart Full-Frame-Refresh mode for e-ink devices.
    * Introduces a macro option allowing the `disp_adapterN_task()` to return `ARM_2D_RT_FRAME_SKIPPED` when a frame is skipped. 
    * Improves the Low-Level-Sync-Up scheme to allow drawing the next frame while e-ink devices is flushing (>300ms).
  * Adds an anti-noise-scanning mode as a countermeasure for the electromagnetic interference
* Enhances the Font service.
  * Introduces a new API to calculate the drawing boxes for multi-line strings.
  * Update the default fonts.
  * Fixes the support for kerning in the font scaling mode.
  * Improves the support for Virtual Resources.
* Adds a new time related helper function: `arm_2d_helper_time_liner_slider_i64()`.
* Allows GUI developers to check user-defined attributes for target node candidates when calling `arm_2d_helper_control_find_node_with_location()`
* Reduce the memory footprint
  * Removes unnecessary dependencies between the Helper services and the example controls.
  * Adds a macro switch `__ARM_2D_CFG_CALL_NON_OPACITY_VERSION_IMPLICITILY_FOR_255__` allowing the linker to remove unused functions further.
  * Adds a macro swith `__ARM_2D_CFG_PFB_DISABLE_DIRTY_REGION_OPTIMIZATION__` for removing the Dirty Region Optimization service throughtly while keeping the APIs for compatiblity. 
* Enhance the Layout Assistant service
  * Add a new keyword: `arm_2d_layout_with_align_mode()` allowing users to update the layout alignment at runtime. 

### Examples and Templates

* Updates example projects.
  * Fixes RW initialisation issues caused by applying the (implicit) ABSOLUTE attribute in RAM-related execution regions.
* Adds new demos: 
  * Music Player
  * Font
  * Pave
  * QRCode
* Adds new example controls:
  * The QRCode Box (`qrcode_box_t`): generate QRcode at runtime.
  * The Image Box (`image_box_t`): loads images and adjusts the size of the image to fit the size of the given target area.
  * The CRT Screen Simulation (`crt_screen_t`): inherits from the Image Box and shows white noise and the CRT scanning effect.
  * The Foldable Panel (`foldable_panel_t`): provides a container (`arm_2d_tile_t *`) as the target panel for further drawing, and allows folding and unfolding it with user-specified configurations. 

* Improves the performance of the demo: balls
* Enhances example controls.
  * Reduces memory footprints
  * Improves the performance of the historgram.
  * Fixes and improves the text box
    * Fixes a divided-by-zero issue.
    * Fixes the clipping issues found in the alignment calculation.
    * Fixes the tail-seperator-treated-as-return issue.
    * Adds a new API to get line height.
    * Adds a new API to get the text line number.
    * Adds new APIs to set and get the font scaling ratio. 
    * Adds the centre alignment mode.
    * Allows pix level scrolling.
    * Adds line caches and a context scheme
    * Introduces the inner margin.
    * Improves performance
* Update Loaders
  * Adds a new JPEG decoder: [**zjpgd**](https://github.com/JiapengLi/zjpgd). It has better ROI support and runs faster than TJpgDec for multiple small ROI blocks.
    * The scene template, including meter and histogram, and the demo rickrolling, were updated for zjpgd. 
    * It allows users to specify the output colour format at runtime.
  * Fixes the cross-platform compilation issue found in the source code of the tjpgd library. 
* Adds the missing libm in makefile of the PC template.
* Adds prefix `arm_2d_asset_` to all assets. 
* Updates documents



## Release v1.2.3 (16th May 2025)

### Core and Library

* Fixes transform tail-prediction issue in helium backend
* Fixes a typo in the implicit colour conversion from ccca8888 to cccn888 transform-only operation
* Adds a new API: `arm_2d_create_region_from_corner_points()`

### Helper Services

* Moves `circle_small.c` from `examples\common\assets` folder to `Helper\source folder`.
* Improves the scene templates
* Improves the compatibility of the control helper service with 3rd party GUIs

### Examples and Templates

* Adds a new Demo: balls
* Updates all scene templates and demos to improve the reliability



## Release v1.2.2 (1st May 2025)

### Core and Library

* Enhances the transform operations
  * Significantly improves the helium transform operations (~60%)
  * Adds implicit colour format conversion support for CCCA8888
  * Adds a new set of APIs that
    * Supports applying different scaling ratios on the X-axis and Y-axis, respectively
    * Supports using floating point numbers (`arm_2d_point_float_t`) to specify pivots
    * Implement the old APIs with the new APIs. The old APIs are marked as deprecated. Please use the new APIs in new designs. 
  * Removes the floating-point based implementation and uses the fixed-point based acceleration only
  * Fixes the black-boarder artifact issue in the Helium transform-only operations
* Adds a new API `arm_2d_sw_normal_root_tile_copy()` to facilitate some low-level tile operations. 
* Improves IIR Blur
  * Adds helium acceleration for the reverse paths
  * Fixes RGB565 ghost-shadow issue
  * Adds Blur mode control.


### Helper Services

* Improve the Font Helper service
  * Fixes issues found in Font scaling
  * Adds a text-buffer centric APIs to improve the printf user experience
    * `arm_lcd_print_to_buffer()`, `arm_lcd_print_buffer()`, etc.
  * Adds support for specifying line-spacing and char-spacing
  * Treats `arm_2d_helper_get_utf8_byte_valid_length()` as a static inline function
  * Updates built-in fonts 
* Improves the support for 3FP helper service
  * Fixes a frame-synchronization issue in the 3FP helper service
  * Implements 3FB mode in the PC project template
* Fixes a selection region calculation issue in the List helper service
* Improves the Display Adatper service
  * Updates the display adapter template for fixing navigation layer display issues
  * Updates the display adapter tempalte for fixing floating-console display issues
* Enhances the Layout Assistant
  * Improves the user application performance with a PFB optimization mode (Enabled by set macro `__ARM_2D_CFG_OPTIMIZE_FOR_PFB_IN_LAYOUT_ASSISTANT__` to `1` in `arm_2d_cfg.h`).
    * Introduces a new hint `__arm_2d_hint_optimize_for_pfb__()`
    * Introduces a new set of keywords with postfix `_open`, which provide the same functionality as the one without the postfix but without PFB optimization.
* Improves the PFB Helper Service
  * Adds new prototypes to the existing API `arm_2d_helper_pfb_is_region_active()`
  * Fixes infinite-loop issue when invalid dirty region is added to the working list.
  * Adds veritical-first PFB scan policy for some special application scenarios.
* Updates the Dirty Region Transform helper serivce to support the new transform APIs
* Fixes stop-updating issues in dirty region helper service when any dynamic dirty region is out of visual area. 

### Examples and Templates

* Updates `spin_zoom_widget_t` to use the new transform APIs.
* Improves example controls
  * Progressbar-flowing
  * Busy-wheel
* Improves the SDL2 driver in the PC project template
  * Adds different outlook styles for monochrome displaying, i.e. nixie-tube, untra-low-power LCD (gray), green (backlight) LCD and blue (backlight) LCD. 
  * Applys the Blur filter to the monochrome display
* Improves the scene templates
  * Adds missing event handlers
* Adds a new example control: Text-Box (`text_box_t`)
  * Supports popular alignment modes: Left-Align, Right-Align and **Justified-Align**
  * Supports Extra-Large texts, Font-Scaling, Char-Spacing, Line-Spacing and **Paragraph-Spacing**.
  * Adds a built-in dirty region service.
  * Adds a new Demo, `arm_2d_scene_text_reader`, for Text-Box.
* Improves the console box (`console_box_t`)
  * Replaces the `arm_lcd_putchar()` with the newly introduces text-buffer-centric APIs, i.e. `arm_lcd_printf_buffer()` and `arm_lcd_putchar_to_buffer()`.
* Use the Time-of-Compilation as the start time of the system timestamp
* Adds an new category of examples, **Asset Loader**, for loading GUI resources
  * Asset Loaders are derived from the virtual resource
  * Adds a dedicated asset loader for JPEG images based on TJpgDec
    * Implements **Region of Interests** mode that only decodes the areas addressed by dirty regions
    * Ultra-small memory footprint
      * 4K Heap for allocating working memory 
      * **No dedicated Framebuffer is required** to store the decoded pixels when used together with tile-copy-only operations. 
    * Adds a demo for showing how to decode JPEG images, i.e. `arm_2d_scene_tjpgd`.
    * Adds a demo for showing a GIF-like animation using JPEG image, i.e. `arm_2d_scene_rickrolling`.
    * Updates scene templates (i.e. meter and histogram) to use JPEG images as the background
* Updates benchmarks
* Adds a demo for the Matrix code-fall effect. 
* Adds a demo for the IIR Blur.
* Adds an example control, progress-bar-round, with two different styles.

### Documentation and Misc

* Adds a section in `how_to_deploy_the_arm_2d_library.md` to introduce how to synchronize with the low-level display driver
* Adds a section in `how_to_use_layout_assistant.md` for docking related keywords and syntaxs
* Improves the support for LVGL
* Improves the support for GCC
* Improves the support for Linux
* Updates `ttf2c.py` 
  * Fixes the support for the Font scaling.
  * Adds an ASCII mode to ease the font generation of the ASCII character set.
* Update README.md and ReleaseNote.md



## Release v1.2.1 (20 December 2024)

### Core and Library

* Introduces a dedicated data type  `q16_t` and corresponding operations for Q15.16 fix point number.

* Fixes a memory alignment issue in the scratch memory management. 

* Improves the protoThread macro templates.

* Improves the log system

* Improves the GCC support for Armv8.1-M processors

* Adds a new set of filter operations that reverse the pixel colours of a user-specified region for the target tile

* Fixes a display issue in IIR-Helium implementation when the blur degree is zero. 

  

### Helper Services

* Adds a new User-Defined-OPCODE template for drawing on the target tile
* Fixes a dirty-region debug issue that keeps the old indication in skipped frames. 
* Fixes a memory leaking issue in Dirty Region Helper Services. 
* Improves dirty region helper services
  * During the dirty region item update, users can specify an extra region for minimal enclosure region calculation in addition to the standard Old-plus-New region calculation.
  * Improves the region clipping schema in the dirty-region-item-update.
* Improves layout assistant
  * Adds new prototypes for existing macro APIs
  * Some minor fixes
* Improves the support for monochrome devices in the display adapter service.
* Improves the PIHelper performance (using fixed point to replace floating point)
* Improves the Scene Player
  * Adds a new Before-Deposing event to each scene
* Enhance the List helper services.
  * Adds tracking mode for selected item indication
  * Adds PIHelper for selected item indication
  * Adds new region calculator for water-fall style of lists.
  * Fixes issues in region calculation 
* Fixes some issues in the font helper service
* Improves the list helper service
  * Improves the support for non-ring mode
  * Adds new region calculators for water-fall style
  * Adds new APIs for selection indication, list status indication, redraw, etc.
  * Adds PIHelper for selection indication
  * Remove `__arm_2d_list_core_move_offset()`
  * Improves the list performance
* Improve `__simple_list_t`
  * Updates the built-in dirty region schema
  * Adds a scrolling bar for selection indication, which supports auto-fading
* Improves the shape helper services
  * Adds an API for drawing the glass-bar effect

### Examples and Templates

* Removes a deprecated header file: `arm_2d_extra_controls.h`. Please use `arm_2d_example_controls.h` instead.
* Introduces a new scene-loading schema for demonstration. 
* Improves the ACI example and allows MDK to use the ACI plugin during Fast Model Debugging.
  * Accelerate rgb565 tile copy with source mask
* Implement `arm_irq_safe` for the PC platform using the SDL2 mutex.
* Adds PC project template to cmsis-pack.
* Allows users to resize the SDL2 window in the PC project.
* Adds a dedicated project template for OLED monochrome devices using Raspberry Pi Pico as the hardware platform.
* Adds new demos
  * A demo for compass
  * A set of demos for monochrome display devices showing the typical application scenarios.
    * Line progress bar
    * histogram
    * Text List
    * Tracking List
    * Icon List
    * Clock
  * A demo for User-Defined-OPCODEs (i.e. drawing lines and circles)
  * A demo for filters
* Improves virtual resource-related demos.
* Adds new scene templates
  * A scene template for knobs. 
* Fixes a typo in `spin_zoom_widgets`
* Improves the example control Progress-Wheel and adds new features
  * Improves the built-in dirty region calculation algorithm
  * Allows users to specify the Progress-Wheel orientation (the starting and ending point), i.e. Top, Bottom, Left or Right.
* Adds new example controls
  * Icon List (`icon_list_t`)
* Fixes some minor issues found in the font helper service.
* Adds demos for User-Defined-OPCODE
  * A demo OPCODE for drawing lines.
  * A demo OPCODE for drawing circles.
  * A demo for how to use those User-Defined-OPCODE. 

### Documentation and Misc

* Fixes the `__REV16` in `arm_2d_user_arch_port.h` 
* Improves Python scripts in the `tools` folder.
* rename `getting_started_as_a_professional_user.md` to `getting_started_as_a_gui_stack_developer.md`



## Release v1.2.0 (9 September 2024)

### Core and Library

- Adds Helium implementations for IIR-Blur operations

- Adds Canary to the scratch memory management

- Improves the porting user experience

- Adds a new API for colour-filling-with-A1-mask with/without opacity

- Fixes a potential issue found in the pixel-pipeline 

  - In some rare cases when the target region pointer is NULL, the operation behaviour is UNDEFINED

- Adds new series APIs for colour-filling-with-alpha-gradient

  - Adds variants for defining alpha gradient with 4-sample-points, 3-sample-points, vertical mode and horizontal mode
  - Adds variants for specifying a source mask
  - Adds variants for specifying an optional opacity

- Significantly improves the performance of transform operations.

  - ~80% performance uplift for non-Helium powered Cortex-M processors
  - ~20% performance uplift for the helium version of colour-filling-with-mask-opacity-and-transform
  - Introduces an assumption that **ALL masks and pictures should contain a 1-pixel width border when applied to the transform operations.** The border's colour should be the same as the picture's background, or the corresponding alpha should always be zero.

- Adds an experimental 2x Super-Sampling-Anti-Alias (2xSSAA) transform implementation

- Introduces new OOPC decorations `ARM_PRIVATE_METHOD()`, `ARM_PROTECTED_METHOD()` and `ARM_PUBLIC_METHOD()`

  

### Helper Services

- Improves the Layout Assistant
  - Updates the layout assistant to improve the performance of user applications
  - Allows users to specify the layout alignment
- Adds a helper service for fonts and LCD display
  - Simplifies the support for user-defined fonts.
  - Adds the scaling support to A1, A2 and A4 fonts
  - Improves the font display performance
  - Fixes some char size calculation issues
  - Adds a new API `arm_lcd_puts_label` to display a label with a user-specified alignment requirement. 
- Updates the scene player service
  - Adds a manual switching mode and allows users to cancel an on-going switching
  - Adds new switching mode, Fly-In, and adds an optional background blur effects to Fly-In mode
- Adds a helper service for user-defined controls (user-defined element tree)
  - Adds traversal algorithms: pre-order traversal, post-order traversal and bottom-up traversal
  - Adds a dedicated API for finding the top-most control with the coordinate of an user touch event
- Improves the list helper service
  - Fixes some list-item-selection-related issues
  - Adds the Non-Ring mode
- Improves the accuracy and behaviour of the PIHelper

### Examples and Templates

- Adds a new scene template `animate background` for the new virtual resource background-image-loading mode
- Update the Benchmarks
  - Adds a 4-sample-point alpha gradient to the Generic Benchmark
  - Replaces the clock-pointer rotation transform operation from tile-transform-with-source-mask to colour-filling-with-mask-opacity-and-transform.
  - Updates the benchmark table
- Improves the overall performance and reduces the memory footprint
- Adds an background-only mode to the bubble charging scene template
- Updates the demo scene `arm_2d_scene_basics` to demonstrate the colour-filling-with-alpha-gradient operations.
- Improves the `progress_bar_flowing`, `progress_bar_drill` and `progress_bar_simple`
- Update list-related controls
  - Adds a base class `__simple_list_t`  
  - Re-factories the `number_list_t` to inherit from the `__simple_list_t`
  - Adds a new control `text_list_t` which is derived from `__simple_list_t`
  - Allows users to customise a `number_list_t` instance.
  - Adds new demos for list: Ruler and Hallow-Out List
  - Updates the scene template Fan to demonstrate the use of the `text_list_t`.
- Adds a new demo scene for testing transform operations
- Updates the example control: nebula
  - Improves the performance
  - Adds support for the inside-out movement
- Improves the performance of the example Helium-ACI library
- Adds built-in fonts for a subset of the ASCII: `ARM_2D_FONT_LiberationSansRegular14_A8`, `ARM_2D_FONT_LiberationSansRegular14_A4`, `ARM_2D_FONT_Arial14_A2` and  `ARM_2D_FONT_LiberationSansRegular14_A1`
- Updates the asset `c_tileSpinWheelMask` to fulfil the requirement of the updated transform operations.
- Adds a new example control: `spin_zoom_widget_t`
  - Updates `arm_2d_scene_fan` and `arm_2d_scene_watch` to demonstrate the usage of `spin_zoom_widget_t`
- Adds a new example control: `meter_pointer_t`
  - Updates `arm_2d_scene_meter` to demonstrate the usage of `meter_pointer_t`

- Adds a new example control: `cloudy_glass_t`
- Adds a new demo: watch face 01


### Documentation and Misc

- Improves Python scripts in the `tools` folder.
  - Adds support for `FT_PIXEL_MODE_MONO`, `FT_PIXEL_MODE_GRAY2` and `FT_PIXEL_MODE_GRAY4` font in ttf2c.py
  - Allows adding a user-defined header file in `arm_2d_cfg.h` by defining the macro `__ARM_2D_USER_APP_CFG_H__`.
  - Adds support for generating A1 mask to img2c.py
- Updates README.md and the Doxygen script
- Updates the copy-right
- Improves the support for IAR



## Release v1.1.6 (23 May 2024)

### Core and Library

- Improves the compatibility with IAR and GCC
- Various fixes in the Helium library
- Adds new APIs for fill-colour-with-mirroring (with/without opacity)
- Adds new APIs for fill-colour-with-horizontal/vertical-line-masks (with/without opacity)
- Adds new APIs for tile-copy-with-source-mask-and-opacity
- Adds implicit conversion for ARGB8888 (ccca8888) in tile-copy APIs
  - Copy with Opacity
  - Copy with Source Masks (with/without Opacity)
- Adds experimental APIs for IIR blur
- Adds a Helium-ACI example library.
- Uses jinja scripts for generating source files
- Improves pixel-pipeline for processing masks


### Helper Services

- Improves performance statistics
  - Allows frame-skipping when no valid dirty region
  
- Improves log services
  - Adds a dedicated channel for statistics
  - Adds a dedicated channel for example controls
  - Adds line number
  
- Improves scratch memory management

- Improves the layout assistant
  - Adds macro helpers for docking
  - Allows `arm_2d_canvas` to skip running when it is out of the partial-framebuffer valid region.
  - Improves the capability of the layout nesting. 
  - Adds a layout assistant debug mode
  
- Improves the lcd printf service
  - Improves the scaling service
  
- Improves the Display Adapter service
  - Adds the support for screen rotation (90, 180 and 270 degrees) in the Display Adapter service
  - Adds colour information
  - Allows retargeting printf to LCD
  - Improves user application performance
  - Improves the support for UTF8
  - Adds `arm_2d_helper_pfb_is_region_being_drawing`
  - Adds a Tiny Mode for navigation layer
  
- Improves the Scene Player service
  - Adds the canvas colour to arm_2d_scene_t.
  - Adds an onLoad event to arm_2d_scene_t.
  - Adds a built-in dirty region helper service
  - Improves the scene switching effects
  
- Adds a helper service for byte FIFO: `arm_2d_byte_fifo_t`

- Adds `__arm_2d_helper_time_elapsed`

- Adds helper services for dirty regions

- Adds a code template for adding user defined OPCODE

  


### Examples and Templates

- Improves the compatibility with CMSIS6
- Improves the complexity of the generic benchmark
- Adds an example control for drawing rounded corner images
- Allows changing the masks when 
  - drawing rounded corner boxes
  - drawing rounded corner border
  - drawing progress wheel
- Adds built-in dirty region support to example controls
- Adds example project for csolution: `[template][csolution]`
- Improves code templates of the scene, the display adapter and the example control
- Adds demos
- Marks `arm_extra_controls.h` as deprecated. Please use `arm_2d_example_controls.h` instead.
- Adds new example controls:
  - console box
  - histogram
- Adds new scene templates:
  - alarm clock
  - bubble charging
  - fan
- Adds new assets 
- Adds an `onDrawParticles` event handler to the dynamic nebular 
- Fixes small issues in example controls
- Improve the PC template
- Reduces the memory footprint of example controls.
- Improves the performance of example controls



### Documentation and Misc

- Improves python scripts in the `tools` folder.
- Adds a document for how-to-use-jinja-script-for-code-generation





## Release v1.1.5 (01 Dec 2023)

### Core and Library

- Improves the support for masks in the pixel-pipeline.
- Updates transform operations
  - Always generating a child tile for the target tile in transform operations.
  - Adds optimization for hollow out masks in C implementations
- Adds the support for adding **Asynchronous acceleration**, **Synchronous acceleration** and **ACI acceleration**.
- Adds the support for trace log
- Adds a new API: `arm_2d_is_region_inside_target()`

### Helper Services

- Updates `ttf2c.py` for generating **A1**, **A2** and **A4** fonts.
- Updates the Display Adapter Service (the PFB Helper Service)
  - Introduces a **Dry-Run** mode in the PFB helper service.
  - Introduces the **Dynamic Dirty Region**, a new type of dirty region items that allows users to update its area during drawing and interact with the PFB. helper service. 
  - Introduces the concept of `virtual screen` which is used to anchor the virtual screen in the PFB helper service.
  - Introduces the Dirty Region Optimization in the PFB Helper service.
  - Updates the performance statistic service in the Display Adapter service.
    - Introduces the CPU Usage info calculation.
    - Places the performance statistic to the Display Adapter class.
  - Simplifies the way of navigation layer customization.
  - Improves the robustness of the Asynchronous Flushing helper.
  - Improves the dirty region debug mode.
  - Updates the `disp_adapter_task()` to support **Framerate-Locking** in the bare-metal environment.
- Introduces helper macros for docking.
- Adds the char scaling support to A8 fonts.
- Updates the transform helper service.
  - Fixes an issue in region calculation.
  - Improves the performance.

### Examples and Templates

- Adds a new example scene to demonstrate a curve-list-view.
- Adds a new example scene (arm_2d_scene_audiomark) to demonstrate the dirty region optimization
- Adds a new service, **3FB helper service** for supporting the so-called Direct Mode.
- Updates example controls and scene templates.
  - Adds dirty regions to the Progress Wheel.
  - Adds a new API for drawing round corner image.
  - Updates the APIs for drawing round corner boxes and allows specifying user defined resources.
  - Updates the progress bars.
  - Adds a new scene template for fitness trackers.
  - 

- Updates benchmarks
  - Reduces the SRAM footprint of the tiny mode for the generic benchmark.

- Improve PC template

### Documentation and Misc

- Adds an initial support for CMake.
- Introduces a header template `arm_2d_user_arch_port.h` and makes it easier to port arm-2d to a different architecture other than Cortex-M.
- Improves the support for C++ compilation.
- Fixes the support for the Arm Compiler 5.
- Improve the support for Arm Compiler 6.21.
- Adds a new document **How To Accelerate Arm-2D**
- Updates the document **How To Deploy Arm-2D** to introduce the supports for Asynchronous Flushing Mode and Direct Mode.
- Updates the benchmark generic result.
- Updates README.md.
- Adds **ReleaseNote.md**

