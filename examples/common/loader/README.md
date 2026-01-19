# Resource Loader

## Overview





## Comparison Among Image Decoders

| Format Name | Lossless Compression | Support Alpha | Decoding Speed | Compressed Image Size | Working Memory | Note                                 |
| ----------- | -------------------- | ------------- | -------------- | --------------------- | -------------- | ------------------------------------ |
| TJpgDec     | NO                   | NO            | Slow           | Smallest              | 3.3K           |                                      |
| ZJpgDec     | NO                   | NO            | Slow           | Smallest              | 3K             | Optimised for ROI                    |
| QOI         | YES                  | YES           | Fast           | Similar to PNG        | 1.5K           | Best for storing Icons and Animation |
| zhRGB565    | Only for RGB565      | NO            | Very Fast      | Balanced              | NO             | Best for RGB565 Animation            |

