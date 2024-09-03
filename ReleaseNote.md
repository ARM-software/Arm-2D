# Release Note



## Release v1.2.0 

### Core and Library

- Adds Helium implementations for IIR-Blur operations

- Adds Canary to the scratch memory management

- Improves the porting user experience

- Adds a new API for colour-filling-with-A1-mask with/without opacity

- Fixes a potential issue found in the pixel-pipeline 

  - In some rare case when the target region pointer is NULL, the operation behaviour is UNDEFINED

- Adds new series APIs for colour-filling-with-alpha-gradient

  - Adds variants for defining alpha gradient with 4-sample-points, 3-sample-points, vertial mode and horizontal mode
  - Adds varients for specifying a source mask
  - Adds varients for specifying an optional opacity

- Significantly improves the performance of transform operations

  - ~80% performance uplift for non-Helium powered Cortex-M processors
  - ~20% performance uplift for the helium version of colour-filling-with-mask-opacity-and-transform
  - Introduces an assumption that **ALL masks and pictures should contains a 1-pixel width boarder when applied to the transform operations.** The colour of the boarder should be same with the background of the picture or the corresponding alpha is always zero.

- Adds an experimental 2x Super-Sampling-Anti-Alias (2xSSAA) transform implementation

- Introduces new OOPC decorations `ARM_PRIVATE_METHOD()`, `ARM_PROTECTED_METHOD()` and `ARM_PUBLIC_METHOD()`

  

### Helper Services

- Improves the Layout Assistant
  - Updates the layout assistant to improve the performance of user applications
  - Allows users to specify the layout alignment
- Adds a helper service for fonts and lcd display
  - Simplifies the support for user defined fonts.
  - Adds the scaling support to A1, A2 and A4 fonts
  - Improves the font display performance
  - Fixes some char size calculation issues
  - Adds a new API `arm_lcd_puts_label` to display a label with an user specified aligment requirement. 
- Updates the scene player service
  - Adds a manual switching mode and allows user to cancel an on-going switching
  - Adds new switching mode, Fly-In, and adds an optional background blur effects to Fly-In mode
- Adds a helper services for user defined controls (user defined element tree)
  - Adds traversal algorithms: pre-order traversal, post-order traversal and bottom-up traversal
  - Adds a dedicated API for finding the top-most control with the coordinate of an user touch event
- Improves the helper service for list
  - Fixes some list-item-selection related issues
  - Adds the Non-Ring mode

### Examples and Templates

- Adds a new scene template `animate background`for the new virtual resource background-image-loading mode
- Update the Benchmarks
  - Adds a 4-sample-point alpha gradient to the Generic Benchmark
  - Replaces the clock-pointer rotation transform operation from tile-transform-with-source-mask to colour-filling-with-mask-opacity-and-transform.
  - Updates the benchmark table
- Improves the overall performance and reduces the memory footprint
- Adds an background-only mode to the bubble charging scene template
- Updates the demo scene `arm_2d_scene_basics` to demonstarte the colour-filling-with-alpha-gradient operations.
- Improves the `progress_bar_flowing`, `progress_bar_drill` and `progress_bar_simple`
- Update list related controls
  - Adds a base class `__simple_list_t`  
  - Re-factories the `number_list_t` to inherit from the `__simple_list_t`
  - Adds a new control `text_list_t` which is derived from `__simple_list_t`
  - Allows users to customize a `number_list_t` instance.
  - Adds new demos for list: Ruler and Hallow-Out List
  - Updates the scene template Fan to demonstrate the use of the `text_list_t`.
- Adds a new demo scene for testing tranform operations
- Updates the example control: nebula
  - Improves the performance
  - Adds support for the inside-out movement
- Improves the performance of the example Helium-ACI library
- Adds built-in fonts for a subset of the ASCII: `ARM_2D_FONT_Arial14_A8`, `ARM_2D_FONT_Arial14_A4` ,`ARM_2D_FONT_Arial14_A2` and  `ARM_2D_FONT_Arial14_A1`
- Updates the asset `c_tileSpinWheelMask` to fulfill the requirement of the updated transform operations.

### Documentation and Misc

- Improves python scripts in the `tools` folder.
  - Adds support for `FT_PIXEL_MODE_MONO`, `FT_PIXEL_MODE_GRAY2` and `FT_PIXEL_MODE_GRAY4` font in ttf2c.py
  - Allows adding an user defined header file in `arm_2d_cfg.h` by defining the macro `__ARM_2D_USER_APP_CFG_H__`.
  - Adds support for generating A1 mask to img2c.py
- Updates README.md and the doxygen script
- Updates the copy-right



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

