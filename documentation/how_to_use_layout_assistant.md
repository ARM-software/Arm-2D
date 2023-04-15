# How to Use Layout Assistant

When we use arm-2d for 2D image processing, almost all APIs need to specify the target Region. Especially for those users who need to use Arm-2D for simple UI design, how to place graphic elements and how to deal with the layout of the screen is essentially a problem of Region calculation. In order to free users from manual Region calculations, Arm-2D refers to the popular GUI layout methodology and provides a series of simple and easy-to-use Helpers in the form of macro templates, called Layout Assistant.

This article will introduce the use of Arm-2D Layout Assistant in detail through examples.



## 1 Basic Concepts

We assume that you are getting into Arm-2D for the first time, so in order for you to understand the following content, we need to introduce some basic concepts first, such as Region, canvas, container and so on.

### 1.1 Region

Region is a rectangular area described by the Location (the coordinates of the upper left corner) and the Size information.

```c
typedef struct arm_2d_region_t {
    implement_ex(arm_2d_location_t, tLocation);
    implement_ex(arm_2d_size_t, tSize);
} arm_2d_region_t;
```

**Figure 1-1 Region with Location and Size ** 

![Region with Location and Size](../documentation/pictures/Introduction2_1a.png) 

#### 1.1.1 Location

The coordinate of the Region is defined by the vertices at the upper left corner of the bit rectangle. Its data structure is as follows:

```c
typedef struct arm_2d_location_t {
    int16_t iX;
    int16_t iY;
} arm_2d_location_t;
```

Different from the general Cartesian coordinate system, in graphics, the Y-axis is usually mirrored in the opposite direction, which means that the lower the Y coordinate is, the larger the Y coordinate is. In the Boxing model that will be introduced later, we will understand that the **coordinates of a Region can be negative**, representing the position of the current Region relative to the starting point of its parent Region.

**Figure 1-2 When Location has a negative coordinates.** 

![When Location has a negative coordinates](../documentation/pictures/Introduction2_1b.png) 

As shown in **Figure 1-2**, when the ***x*** and ***y*** coordinates of a Region are both negative, it actually has a considerable area outside (upper left corner) of its parent Region. When we try to find the intersection of the current Region and its parent Region, we will find that only part of the region is valid.

#### 1.1.2 Size

The size information of the Region is described by the Height and Width together. The data structure is defined as follows:

```c
typedef struct arm_2d_size_t {
    int16_t iWidth;
    int16_t iHeight;
} arm_2d_size_t;
```

Although a signed type ***int16_t*** is used to describe the width and height, negative numbers are meaningless and should be avoided.

### 1.2 Boxing Model

The so-called Boxing Model describes the affiliation between Regions, which is often used to describe the relationship between containers and visual elements. 

In a GUI stack, the Boxing Model usually talks about more complex stuff, such as the border's width, the margin inside a container border, the padding of / distance between the elements inside a container etc. Arm-2D does ***NOT*** cares about these details but only describes the simple relationship between a container and the elements inside.

#### 1.2.1 Absolute Location and Relative Location

In Arm-2d, we consider panels or windows as containers, and the Locations of the panels and the windows are their coordinates in the display buffer. **We call this kind of location information that directly describes the coordinates in a display buffer as an Absolute Location**. In **Figure 1-3**, the panel (top container) coordinates are absolute coordinates.

The coordinates of the elements inside a container are described as **coordinates relative to the upper left corner of the parent container**. We call this kind of Locations the **Relative Locations**. In addition to that, since the container is only a special element, container nesting becomes possible.  In **Figure 1-3**, the two innermost Regions have Relative Locations.

**Figure 1-3 A Typical Example of Absolute Locations and Relative Locations**

![A Typical Example of Absolute Locations and Relative Locations](../documentation/pictures/Introduction2_2a.png) 

#### 1.2.2 Absolute Region and Relative Region

If a Region has absolute Location, it is an **Absolute Region**; similarly, if a  Region has relative Location, it is a **Relative Region**.

**Figure 1-4 A Typical Example of Absolute Regions and Relative Regions**

![A Typical Example of Absolute Regions and Relative Regions](../documentation/pictures/Introduction2_2b.png) 

When we use these relative and absolute information to perform visual area calculations, it is easy to exclude those areas that are actually invisible to the user from various graphic operations, thereby improving the overall 2D processing performance (as shown in **Figure 1-4** ).

### 1.2 Canvas

The essence of the canvas is Region. To draw on a Tile, we need to use `arm_2d_canvas()` to create a canvas first. The syntax is as follows:

```c
arm_2d_canvas(<The address of the target tile>, <the canvas name>) {
    /* the scope of the canvas */
}
```

Here `arm_2d_canvas()` takes two parameters, i.e. the **address of the target Tile** and the **name of the canvas**. `arm_2d_canvas()` will create a canvas with the specified name for the specified Tile. 

**NOTE**: Canvas cannot be used outside of curly braces.

For example:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    ARM_2D_UNUSED(ptTile); /* the target screen */
    ...
    arm_2d_canvas(ptTile, __top_canvas) {
        /* place your drawing code here */
    }

    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}
```

This is a typical scene drawing function, where `ptTile` points to the target Tile representing the screen. In order to draw on the screen, we need to create a canvas for it, namely `__top_canvas` in the example.

### 1.3 Container

Region is just a descriptive information. How to interpret the meaning of Region depends entirely on which API we use. Similarly, when we use the canvas for layout, it often happens that the actual graphic elements exceed the scope of the canvas. At this point, whether to clip the excess part depends on the specific situation. But if we clearly want the part beyond the canvas to be clipped, then we need to introduce the concept of Container.

The essence of the container is a Child-Tile, and any content that exceeds the rectangular area of the child tile will be clipped. Via the macro `arm_2d_container()` we can easily create a container. The syntax is as follows:

```c
arm_2d_container(<the address of the target tile>, <the name of the new child tile>, <the address of the target region>)
```

Here `arm_2d_container()` takes three parameters, i.e. the **address of the target Tile**, the **name of the new Child Tile**, and **the address of the target region**. `arm_2d_container()` will generate a new child tile with the name specified by the user according to the given target Region, and generate the corresponding canvas with `__canvas` as the suffix. For example, if we specify the name of the child tile as `my_container`, then the corresponding canvas is called `my_container_canvas`. 

**NOTE**:  

1. The address of the target Region can be `NULL`. In this case, the child tile essentially coincides with the target Tile in size.
2. The child tile and the canvas cannot be used outside of curly braces.

```c
RM_NONNULL(1)
void control_template_show( user_control_template_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion, 
                            bool bIsNewFrame)
{
    ...

    arm_2d_container(ptTile, __control, ptRegion) {
        /* put your drawing code inside here
         *    - &__control is the target tile (please do not use ptTile anymore)
         *    - __control_canvas is the canvas
         */

    }

    arm_2d_op_wait_async(NULL);
}

```

The above example comes from the control template. In general GUI design, the part beyond the rectangular area of the control needs to be clipped, that's why we create a container for a user control. 



## 2 Alignment

In UI design, alignment is the most basic layout method. There are 9 alignments as shown in **Figure 2-1**:

**Figure 2-1 The 9 alignments**

![](../documentation/pictures/HowToUseLayoutAssistant2_1.png) 



Arm-2D provides macros with a unified style for these 9 alignments, as shown in **Table 2-1**:

**Table 2-1 Macros and Regions for the 9 alignments**

|            | Left                                                 | Central                                                  | Right                                                  |
| ---------- | ---------------------------------------------------- | -------------------------------------------------------- | ------------------------------------------------------ |
| **Top**    | `arm_2d_align_top_left()`: `__top_left_region`       | `arm_2d_align_top_centre()`: `__top_centre_region`       | `arm_2d_align_top_right()`: `__top_right_region`       |
| **Middle** | `arm_2d_align_mid_left()`: `__mid_left_region`       | `arm_2d_align_centre()`: `__centre_region`               | `arm_2d_align_mid_right()`: `__mid_right_region`       |
| **Bottom** | `arm_2d_align_bottom_left()`: `__bottom_left_region` | `arm_2d_align_bottom_centre()`: `__bottom_centre_region` | `arm_2d_align_bottom_right()`: `__bottom_right_region` |



The syntax of these macros is as follows:

**Syntax 1:** 

```c
arm_2d_align_<alignment>(<the target region: arm_2d_region_t>, <the width of the target area: int16_t>, <the height of the target area: int16_t>) {
    /* You can use a region called __<alignment>_region in the scope defined by the curly braces*/
    ...
}
```

Here the macro helper `arm_2d_align_<alignment>` takes three arguments, i.e. the **target region object**, the **width** and **height** of the target area. 

**NOTE**: Please pass the **arm_2d_region_t object** to the macro helper as the target region but **NOT** the **address of an arm_2d_region_t object**.

**Syntax 2:**

```c
arm_2d_align_<alignment>(<the target region: arm_2d_region_t>, <the size of the target area: arm_2d_size_t>) {
    /* You can use a region called __<alignment>_region in the scope defined by the curly braces*/
    ...
}
```

Here the macro helper `arm_2d_align_<alignment>` takes two arguments, i.e. the **target region object** and the **size of the target area**. 

**NOTE**: 

1. Please pass an **arm_2d_region_t object** to the macro helper as the target region but **NOT** the **address of an arm_2d_region_t object**. 
2. Please pass an **arm_2d_size_t object** to the macro helper as the size of the target area but **NOT** the **address of an arm_2d_size_t object**. 



Based on the syntax above, the code corresponding to **Figure 2-1** is as follows:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    user_scene_0_t *ptThis = (user_scene_0_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
        
        arm_2d_align_top_left(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__top_left_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
        }
        arm_2d_align_top_centre(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__top_centre_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
        }
        arm_2d_align_top_right(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__top_right_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
        }


        
        arm_2d_align_mid_left(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__mid_left_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }
        arm_2d_align_centre(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__centre_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }
        arm_2d_align_mid_right(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__mid_right_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }
        
        arm_2d_align_bottom_left(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__bottom_left_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }
        arm_2d_align_bottom_centre(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__bottom_centre_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }
        arm_2d_align_bottom_right(__top_canvas, 60, 60 ) {
            
            draw_round_corner_border(   ptTile, 
                                        &__bottom_right_region, 
                                        GLCD_COLOR_BLACK, 
                                        (arm_2d_border_opacity_t)
                                            {32, 32, 255-64, 255-64},
                                        (arm_2d_corner_opacity_t)
                                            {0, 128, 128, 128});
                                    
        }

    }
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



## 3 Layout

When designing a graphical interface, in addition to the **alignment** described above, we usually want to arrage a stream of graphic elements to a specified area in sequence under certain rules. This process is often referred to as **layout**. Commonly used layout rules are:

- **Line Stream Layout** (in vertical manner or horizontal manner)
- **Steam Layout with wrapping**  (in vertical-first manner or horizontal-first manner)



When doing layout, users only need to enumerate graphic elements, specify their sizes, and optionally specify the space (a.ka. **Padding**) among elements. During this process, the user does not need to manually calculate the coordinates of each element, which is very convenient.



### 3.1 Line Stream Layout

The Line Stream Layout is a common layout method, which places elements in a specified area one by one. 

**Note**: If any elements exceed the given area, the Line Stream Layout will not wrap when placing elements. 

#### 3.1.1 Horizontal Line Stream

Horizontal placement is one of the common ways of Line Stream Layout. Its syntax is as follows:

```c
arm_2d_layout(<the target region: arm_2d_region_t>) {
    /* Syntax 1 */
    __item_line_horizontal(<width>, <height>) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    
    /* Syntax 2 */
    __item_line_horizontal(<size of the element: arm_2d_size_t>) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    /* more of the __item_line_horizontal segments */
    ...
    
}
```

Here, `arm_2d_layout()` takes an `arm_2d_region_t` object as the target region and `__item_line_horizontal()` must be used inside the `arm_2d_layout()` structure. You can list as many `__item_line_horizontal()` segments as you want. `__item_line_horizontal()` takes two mandatory parameters, i.e. **width** and **height** or one `arm_2d_size_t` object as the size of the element.

**NOTE**: 

1. Please pass an **arm_2d_region_t object** to the macro helper `arm_2d_layout()` as the target region but **NOT** the **address of an arm_2d_region_t object**. 
2. When using the **Syntax 2**, please pass an **arm_2d_size_t object** to the macro helper `__item_line_horizontal()` as the size of the target element but **NOT** the **address of an arm_2d_size_t object**. 



**Figure 3-1** shows an simple example of the Line Stream Layout, which places four buttons in a line wihout and padding. In the source code, we mark the target area in red, so you can see that the 4th button is actually out of the region. 

**Figure 3-1 A Horizontal Line Stream Layout Example**

![](../documentation/pictures/HowToUseLayoutAssistant3_1_1.png) 

The example code is shown below:

```c
static void draw_buttom(const arm_2d_tile_t *ptTile, 
                        arm_2d_region_t *ptRegion,
                        const char *pchString,
                        COLOUR_INT tColour,
                        uint8_t chOpacity)
{
    

    arm_2d_size_t tTextSize = ARM_2D_FONT_A4_DIGITS_ONLY
                                .use_as__arm_2d_user_font_t
                                    .use_as__arm_2d_font_t
                                        .tCharSize;
    tTextSize.iWidth *= strlen(pchString);

    arm_2d_container(ptTile, __button, ptRegion) {
        
        draw_round_corner_border(   &__button, 
                                    &__button_canvas, 
                                    GLCD_COLOR_BLACK, 
                                    (arm_2d_border_opacity_t)
                                        {32, 32, 32, 32},
                                    (arm_2d_corner_opacity_t)
                                        {32, 32, 32, 32});
        
        arm_2d_align_centre(__button_canvas, tTextSize) {

            arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__button);
            arm_lcd_text_set_font((arm_2d_font_t *)&ARM_2D_FONT_A4_DIGITS_ONLY);
            arm_lcd_text_set_draw_region(&__centre_region);
            arm_lcd_text_set_colour(tColour, GLCD_COLOR_WHITE);
            arm_lcd_text_set_opacity(chOpacity);
            arm_lcd_printf("%s", pchString);
            arm_lcd_text_set_opacity(255);
        }
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 100, 100 ) {


            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            arm_2d_layout(__centre_region) {
                __item_line_horizontal(28,28) {
                    draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                }
                __item_line_horizontal(28,28) {
                    draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                }
                __item_line_horizontal(28,28) {
                    draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                }
                __item_line_horizontal(28,28) {
                    draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



If we want to clip the part out of the target area, as shown in **Figure 3-2**, we should use `arm_2d_container()`.

**Figure 3-2 Using Container for Region Clipping**

![](../documentation/pictures/HowToUseLayoutAssistant3_1_2.png) 

The corresponding code is as follows:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 100, 100 ) {


            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            arm_2d_container(ptTile, __panel, &__centre_region) {
            
                arm_2d_layout(__panel_canvas) {
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                    }
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



In addition to the size information, `__item_line_horizontal()` can optionally accept 4 additional padding parameters for the left, right, top and bottom. The syntax is shown below:

```c
arm_2d_layout(<the target region: arm_2d_region_t>) {
    /* Syntax 1 */
    __item_line_horizontal(<width>, <height> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    
    /* Syntax 2 */
    __item_line_horizontal(<size of the element: arm_2d_size_t> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    /* more of the __item_line_horizontal segments */
    ...
    
}
```



**Figure 3-3** shows the visual effects of four elements with different padding using the Horizontal Line Stream Layout.

**Figure 3-3 Different Paddings among Items**

![](../documentation/pictures/HowToUseLayoutAssistant3_1_3.png) 

The corresonponding code is as follows:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 200, 50 ) {

            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            arm_2d_container(ptTile, __panel, &__centre_region) {
            
                arm_2d_layout(__panel_canvas) {
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28, 2, 2, 10, 10) {
                        draw_buttom(&__panel, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28, 10, 10, 20, 20 ) {
                        draw_buttom(&__panel, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                    }
                    __item_line_horizontal(28,28) {
                        draw_buttom(&__panel, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                    }
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



#### 3.1.2 Vertical Line Stream

The Vertical Line Stream Layout is similar to the Horizontal Line Stream Layout described above. The only difference is the layout direction. The syntax is shown below:

```c
arm_2d_layout(<the target region: arm_2d_region_t>) {
    /* Syntax 1 */
    __item_line_vertical(<width>, <height> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    
    /* Syntax 2 */
    __item_line_vertical(<size of the element: arm_2d_size_t> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    /* more of the __item_line_vertical segments */
    ...
    
}
```

**NOTE**: 

1. Please pass an **arm_2d_region_t object** to the macro helper `arm_2d_layout()` as the target region but **NOT** the **address of an arm_2d_region_t object**. 
2. When using the **Syntax 2**, please pass an **arm_2d_size_t object** to the macro helper `__item_line_vertical()` as the size of the target element but **NOT** the **address of an arm_2d_size_t object**. 



**Figure 3-4** shows the visual effects of four elements with the same padding using the Vertical Line Stream Layout.

**Figure 3-4 A Vertical Line Stream Layout Example with Padding**

![](../documentation/pictures/HowToUseLayoutAssistant3_1_4.png) 

The example code is shown below:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 120, 120 ) {


            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            
            arm_2d_layout(__centre_region) {
                __item_line_vertical(28, 28, 2, 2, 2, 2) {
                    draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                }
                __item_line_vertical(28, 28, 2, 2, 2, 2) {
                    draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                }
                __item_line_vertical(28, 28, 2, 2, 2, 2 ) {
                    draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                }
                __item_line_vertical(28, 28, 2, 2, 2, 2) {
                    draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



### 3.2 Stream Layout with Wrapping

Similar to the Line Stream Layout, the **Stream Layout** is a commonly used layout method, which places elements in a line and wraps before hitting the bundary of the target area. 

#### 3.2.1 Horizontal Stream

Horizontal placement is one of the common ways of the Stream Layout. Its syntax is as follows:

```c
arm_2d_layout(<the target region: arm_2d_region_t>) {
    /* Syntax 1 */
    __item_horizontal(<width>, <height> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    
    /* Syntax 2 */
    __item_horizontal(<size of the element: arm_2d_size_t> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    /* more of the __item_horizontal segments */
    ...
    
}
```

**NOTE**: 

1. Please pass an **arm_2d_region_t object** to the macro helper `arm_2d_layout()` as the target region but **NOT** the **address of an arm_2d_region_t object**. 
2. When using the **Syntax 2**, please pass an **arm_2d_size_t object** to the macro helper `__item_horizontal()` as the size of the target element but **NOT** the **address of an arm_2d_size_t object**. 



**Figure 3-5** shows a digits panel implemented with the Horizontal Stream Layout. Padding (2,2,2,2) is added to each element.

**Figure 3-5 A Horizontal Stream Layout Example: Digits Panel**

![](../documentation/pictures/HowToUseLayoutAssistant3_2_1.png) 

The corresponding source code is as follows:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 96, 128 ) {


            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            arm_2d_layout(__centre_region) {
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "5", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "6", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "7", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "8", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "9", GLCD_COLOR_BLUE, 64);
                }
                __item_horizontal(28,28,34,34,2,2) {
                    draw_buttom(ptTile, &__item_region, "0", GLCD_COLOR_BLUE, 64);
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```



#### 3.2.2 Vertical Stream

The **Vertical Stream Layout** is similar to the **Horizontal Stream Layout** described above. The only difference is the layout direction. The syntax is shown below:

```c
arm_2d_layout(<the target region: arm_2d_region_t>) {
    /* Syntax 1 */
    __item_vertical(<width>, <height> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    
    /* Syntax 2 */
    __item_vertical(<size of the element: arm_2d_size_t> [, <left>, <right>, <top>, <bottom>]) {
        /* you can use __item_region in the scope defined by the curly braces */
        ...
    }
    /* more of the __item_vertical segments */
    ...
    
}
```

**NOTE**: 

1. Please pass an **arm_2d_region_t object** to the macro helper `arm_2d_layout()` as the target region but **NOT** the **address of an arm_2d_region_t object**. 
2. When using the **Syntax 2**, please pass an **arm_2d_size_t object** to the macro helper `__item_vertical()` as the size of the target element but **NOT** the **address of an arm_2d_size_t object**. 

**Figure 3-6** shows a digits panel similar to the one shown in **Figure 3-5**. It is implemented with the Vertical Stream Layout and paddings are added.

**Figure 3-6 A Vertical Stream Layout Example: Digits Panel**

![](../documentation/pictures/HowToUseLayoutAssistant3_2_2.png) 

The example code is shown below:

```c
static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    
    arm_2d_canvas(ptTile, __top_canvas) {
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

        arm_2d_align_centre(__top_canvas, 128, 96 ) {


            arm_2d_helper_draw_box( ptTile, 
                                    &__centre_region, 
                                    1,
                                    GLCD_COLOR_RED, 
                                    128);
                
            arm_2d_op_wait_async(NULL);
            
            arm_2d_layout(__centre_region) {
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "1", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "2", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "3", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "4", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "5", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "6", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "7", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "8", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,2,2) {
                    draw_buttom(ptTile, &__item_region, "9", GLCD_COLOR_BLUE, 64);
                }
                __item_vertical(28,28,2,2,34,34) {
                    draw_buttom(ptTile, &__item_region, "0", GLCD_COLOR_BLUE, 64);
                }
            }
        }
    }

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}
```







