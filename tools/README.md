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

```sh
img2c.py [-h] [--format <FORMAT>] [--name <NAME\>] [--dim <Width> <Height>] [--rot <ANGLE>] <-i <Input File Path>> [-o <Output file Path>]
```



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





# TrueType Font Convertor (ttf2c.py)

## 1. Overview

This tool will extract the glyph bitmap from the specified TrueType font file according to the text (dictionary) provided by the user and generate a custom font that arm-2d can use directly.



## 2. How to Use

### Usage:

```sh
ttf2c.py [-h] <-i <Input File Path>> <-t <Text File Path>>[-o <Output File Path>] [-n <Font Name>] [-s <Font Bit Width>]
```



| Arguments                 | Description                                                  | Memo     |
| ------------------------- | ------------------------------------------------------------ | -------- |
| ***-h, --help***          | show help information                                        |          |
| -i ***Input File Path***  | Input TTF file (*.ttf)                                       |          |
| -t **Test File Path**     | A input text file used as reference. ttf2c.py uses the chars in the text file to create a customized font. |          |
| -o ***Output File Path*** | the path for the output C file containing the customized font using UTF8 coding | Optional |
| -n ***Font Name***        | the font name: **ARM_2D_FONT_A8_\<Font Name\>**              | Optional |
| -p Pixel Size             | the desired font size in pixel. The pixel size is only a reference and the actual size will be slightly larger due to the TrueType Font feature. | Optional |
| -s ***Font Bit Width***   | the font bit width, must be 1,2,4 or 8 bit                   | Optional |


## 3. Installation:

Needs ***python3***, ***numpy*** and **freetype**:

```
pip install freetype-py
pip install numpy
```



