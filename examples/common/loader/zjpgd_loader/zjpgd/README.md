# Z JPEG Decoder

[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-32--bit%20MCU-green.svg)]()

> The "Z" in the name is a tribute to the zstd project.

## Key Features

- **Targeted Rectangular Decoding** - Decode only the region you need, not the entire image
- **Color Flexibility** - Runtime-configurable output formats (7 color modes supported)
- **Memory Efficient** - Minimal RAM footprint with auto adaptive length cache buffer sizes
- **Performance Optimized** - Fast single pixel extraction with selective processing
- **32-bit MCU Targeted** - Designed specifically for 32-bit embedded systems

### Memory Footprint

Typical memory usage for different JPEG formats:

```
./zjdcli samples/rgbw_yuv400.jpg
./zjdcli samples/rgbw_yuv420.jpg
./zjdcli samples/rgbw_yuv422.jpg
./zjdcli samples/rgbw_yuv444.jpg
```

Results:
```
samples/rgbw_yuv400.jpg minimum cost (4 bytes cache): 1756
samples/rgbw_yuv444.jpg minimum cost (4 bytes cache): 2568
samples/rgbw_yuv422.jpg minimum cost (4 bytes cache): 2696
samples/rgbw_yuv420.jpg minimum cost (4 bytes cache): 2952
```

Note: A 3KB (3072B) buffer is recommended for optimal performance. You can set a larger buffer size at runtime to reduce I/O operations. The `zjd_init` function will automatically adjust the internal cache size accordingly.


## Quick Start

### 1. Build the Project

To build the project, simply run:

```bash
make clean all
```

### 2. Run Tests

```bash
./zjdcli samples/rgbw_yuv420.jpg
```


### 3. Basic Usage Examples

#### Initialization
```c
    #include <zjpgd.h>

    zjd_t zjd;
    zjd_cfg_t cfg;

    uint8_t work[3072];

    cfg.outfmt = ZJD_RGB888;
    cfg.ifunc = zjd_ifunc;
    cfg.ofunc = zjd_ofunc;
    cfg.buf = work;
    cfg.buflen = sizeof(work);
    cfg.arg = (void *)fp;
    res = zjd_init(&zjd, &cfg);
    if (res != ZJD_OK) {
        // ...
    }
```

#### Full Image Decoding

```c
    zjd_scan(&zjd, NULL, NULL);
```

Note: simply rerun `zjd_scan`, if you need decode the same image multiple times, `zjd_init` is needed only once.

#### Targeted Rectangle Decoding

```c
    zjd_scan(&zjd, NULL, tgt_rect);
```

Note: zjd_scan will skip irrelevant regions's dequant/idct calculations automactically to speed up the decoding process.

#### Snapshot and Region of Interest Recovery

Take snapshot for specific MCU regions:
```c
    int zjd_ofunc(zjd_t *zjd, zjd_rect_t *rect, void *pixels)
    {
        /* take snapshot if X, Y in rect, X,Y is the top left of ROI */
        if (X >= rect->x && X < rect->x + rect->w &&
            Y >= rect->y && Y < rect->y + rect->h) {
            zjd_save(zjd, &snapshot);
        }
    }

    zjd_scan(&zjd, NULL, NULL);
```

Region of Interest Recovery:

```
    zjd_scan(&zjd, snapshot, roi_rect);
```

## TODO

- [ ] DHT optimize, make hc right aligned
- [ ] **Rectangle boundary clipping** - Ensure decoded regions stay within actual image bounds (maybe)
- [ ] **Optimized rectangle intersection** - Faster MCU block filtering for target rectangles
- [ ] More descriptive error codes

## Limitations & Known Issues

- **Platform Support**: Optimized for 32-bit MCUs; 8/16-bit MCUs may not be supported
- **Block Processing**: Strict 8×8 block output, even in 16×16 or 16×8 MCU modes
- **YUV Subsampling**: For 4:2:0/4:2:2, output sequence is a subsequence of standard MCU order
- **Scaling**: Scale feature removed for simplified implementation
- **Boundary Clipping**: Output images may exceed actual image boundaries

## Code Formatting

Auto-format source files using astyle:

```bash
astyle \
  --suffix=none \
  --style=kr \
  --indent=spaces=4 \
  --pad-oper \
  --pad-header \
  --pad-comma \
  --unpad-paren \
  --unpad-brackets \
  --align-pointer=name \
  --align-reference=name \
  --max-code-length=160 \
  --break-after-logical \
  --lineend=linux \
  --convert-tabs \
  --verbose \
  --add-braces \
  ./zjpgd/zjpgd* main.c test.c
```

## Acknowledgments

- This project is inspired and encouraged by [@GorgonMeducer](https://github.com/GorgonMeducer)
    - [**Arm-2D Project**](https://github.com/ARM-software/Arm-2D) - Design inspiration
        - [Related technical article](https://mp.weixin.qq.com/s/qDuVUSz9FjVqmAuhFhS6rQ)
- This project borrows Zigzag / Dequantization / IDCT algorithms of ChaN and the original [TJpgDec (Tiny JPEG Decompressor)](https://elm-chan.org/fsw/tjpgd/) project.
- [**JPEG Algorithm Reference**](https://www.cnblogs.com/Dreaming-in-Gottingen/p/14428152.html)


### Original License Notice of TJpgDec
```
/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor
/-----------------------------------------------------------------------------/
/ The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
/ This is a free software that opened for education, research and commercial
/  developments under license policy of following terms.
/
/  Copyright (C) 2021, ChaN, all right reserved.
/
/ * The TJpgDec module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/
```
