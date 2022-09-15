# Image to Tile Convertor (img2c.py)

## 1. Overview

The `arm_2d_tile_t` descriptors are used as the standard input and output in most of the arm-2d APIs. This tool helps you to convert a given image into 

- A C array for pixels and (for most of time) a dedicated alpha-mask if the format contains an alpha-channel.
- `arm_2d_tile_t` descriptors for the image and the alpha-mask(s).



### FEATURE

- Support popular image formats, e.g. PNG, JPG, JPEG, BMP etc.
- Output formats:
  - RGB565 with an optional alpha-mask (A2, A4 and A8)
  - RGBA8888 with alpha-masks, one in a separate alpha-mask and the other in `ARM_2D_CHANNEL_8in32` format. 
- Support resize before conversion 
- Support rotation before conversion



## 2. How to Use

### Usage :

img2c.py [***-h***] [***--format*** \<***FORMAT***\>] [***--name \<NAME\>***] [***--dim DIM DIM***] [***--rot ANGLE***] \<***-i \<Input File Path***\>\> \[***-o \<Output file Path***\>\]



| Arguments                 | Description                                                  | Memo     |
| ------------------------- | ------------------------------------------------------------ | -------- |
| ***-h, --help***          | show help information                                        |          |
| -i ***Input File Path***  | Input file (png, bmp, etc..)                                 |          |
| -o ***Output File Path*** | output C file containing RGB56/RGB888 and alpha values arrays | Optional |
| --name ***Name***         | A specified array name.                                      | Optional |
| --format ***Format***     | RGB Format, i.e. rgb565, rgb32, all (***default***)          | Optional |
| --dim ***Width Height***  | Resize the image with the given width and height             | Optional |
| --rot ***angle***         | Rotate the image with the given angle in degrees             | Optional |
| --a2, --a4                | introduce extra A2 / A4 masks when it is possible.           | Optional |


## Example:

```
python img2c.py -i ..\examples\benchmark\asset\HeliumRes.jpg --name Helium
```



## Installation:

Needs ***python3***, ***pillow*** (https://python-pillow.org/) & ***numpy***

```
pip install Pillow
pip install numpy
```

