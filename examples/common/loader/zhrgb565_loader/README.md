# About zhRGB565

## Overview

The zhRGB565 is a lossless RGB565 data compression format, suitable for embedded MCU applications. 

You can find the License and documents in the `zhrgb565` folder.

The original repo: https://gitee.com/haku-mcuplayer/zhRGB565



## Changes for redistribution

- Removed the `example` folder
- Removed the `zhRGB565_encoder_IDE` folder. You can download the encoder from the original repo.
- Customised the algorithm to convert decoded RGB565 pixels into GRAY8 or CCCN888 (RGBA8888) pixels.