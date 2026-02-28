# Losslessly Compressed Mask (LMSK) Specification (1.2.5)



## 1 Overview 

**LMSK** is a lossless compression format optimised for alpha mask images. It employs a hybrid entropy coding strategy combining deltas, run-length encoding, linear gradient descriptions, and random line access capabilities.



#### Endianness
- **Integers**: All multi-byte integers are stored in **Little Endian**.
- **Bitstream**: Decoding uses **LSB First** bit ordering (Least Significant Bit consumed first).

#### File Structure
```
[Header: 16 bytes]
[32 Palette]
[Floor Table: floor_count * 2 bytes]
[Line Index Table: height * 2 bytes]
[Data Stream]
[End Mark: 4 bytes]
```

#### Limitations

* The image width and height should **NOT** exceed 32767. 



## 2 Data Structures

### Header & End Mark

```c
/* 16byte header */
typedef struct arm_lmsk_header_t {
    uint8_t chName[5];					 /*! {'L','M','S','K',0} Losslessly compressed MaSK */
    struct {
        uint8_t u4Minor : 4;
        uint8_t u4Major : 4;
    } Version;
  
    int16_t iWidth;
    int16_t iHeight;
  
    uint8_t u3AlphaMSBCount : 3;  /*! actual MSB alpha bits = u3AlphaMSBCount + 1 */
    uint8_t bRaw          	: 1;  /*! whether the alpha is compressed or not */
    uint8_t u2TagSetBits    : 2;  /*! must be 0x00, reserved for the future */
    uint8_t                 : 2;  /*! must be 0x00, reserved for the future */
    uint8_t chFloorCount;
    uint32_t                : 32; /*! reserved */
} arm_lmsk_header_t;
```
- `u3AlphaMSBCount`: Significant alpha bits minus 1 (i.e., effective_alpha_msb_count = `u3AlphaMSBCount` + 1).  Unless otherwise specified,  all operations apply only to these significant high bits.

  > [!NOTE]
  >
  > 1. The first alpha in each encoded line, **TAG_INDEX**, **TAG_GRADIENT** and **Palette** are not affected by `u3AlphaMSBCount`.
  > 2. When `(u3AlphaMSBCount + 1) < 8` , the remaining LSB are **don't-care**, and the decoder can have its own policy to determine the LSB value.  

- `bRaw`: 

  - `0` - The data section contains compressed data. The Palette, Floor Table and Line Index Table are valid.
  - `1`- The data section **ONLY** contains the raw alpha pixels. The Palette, Floor Table and the Line Index Table are removed. Hence, `chFloorCount` should always be `0`, and the decoder should ignore `chFloorCount` and `u3AlphaMSBCount`.

The LMSK file ends with a (**at least 4 bytes long**) marker. The content is defined by the encoder and should be ignored by the decoder. The purpose of introducing 4-byte padding is to enable the encoder to safely implement a 32-bit Tag-fetching scheme. 

### Palette Table

The compressed file has a 32-slot palette right after the header, each slot stores a 8-bit alpha value.. The encoder can place a dedicated **INDEX** tag to use the palette. How the palette is generated is fully determined by the encoder. Some commonly used strategies are:

* Places frequently used alphas. 
* Replaces the **ALPHA_TAG**.
* Stores patterns that are frequently used. 

### Floor Table and Line Index

The **Floor Table** defines base address partitions for line data. Only when `chFloorCount` is non-zero, the Floor Table will be present. 

The line numbers recorded in the Floor Table must be incremental.

When Floor Table is present, for a line number `L` where `L >= floor_table[i]` (and `L < floor_table[i+1]` if exists), the base address for that line is `(i + 1) * (1 << (16 - u2TagSetBits))` bytes or `(i + 1) * 65536` for the current version.

The **Line Index Table** stores **byte offsets** for each line relative to its **base address**. The address in the **data section** is calculated as:
```
base_address = (floor_index + 1) * (1 << (16 - u2TagSetBits))
line_offset  = line_index_table[row]
line_pos     = base_address + line_offset
```

> [!TIP]
>
> **Row-level Deduplication**
> Multiple line index entries **may point to the same offset** within the data stream. If several scanlines contain identical pixel data (e.g., solid horizontal lines or repeating patterns), the encoder may map these line indices to the same compressed data segment. The decoder processes each line independently without requiring special handling.



## 3 Data Stream Specification

The data stream (a.k.a **data section**) is organised by scanlines. Each line begins with **1 byte of raw alpha value** (the absolute value of the first pixel in the line), followed by a bitstream of Tags.

### 3.1 Tags

| Tag Bits (LSB) |    Size    | Name            | Description                                                  |
| :------------- | :--------: | :-------------- | :----------------------------------------------------------- |
| `0b0xxx-xx00` | **8 bits** | **INDEX**       | An index for a constant Palette.                             |
| `0b1000-0000` | **8bits** | **DO** | The start marker of the **DO / WHILE** loop. |
| `0b1xxx-xx00` | **8bits** | **WHILE** | The end marker of the **DO / WHILE** loop. `count = bits[6:2]` (5-bit):<br/> repeat iteration count = `count + 1` here, `count` **MUST** be a none-zero value. |
| `0bxxxx-xx01` | **8 bits** | **REPEAT**      | Run-length control. `count = bits[7:2]` (6-bit):<br>• `0–61`: **REPEAT**, run length = `count + 1`<br>• `62` (0xF9): **GRADIENT_TAG**<br>• `63` (0xFD): **ALPHA_TAG** |
| `0bxxxx-xx10` | **8 bits** | **DELTA_SMALL** | Small delta.  Two 3bits delta fields in this tag (i.e. `delta0, delta1`), which encode two pixels, here`delta0 = sign_extend(bits[4:2], 3)`, `delta1 = sign_extend(bits[7:5], 3)`range **[-4, +3]**. The decoder should generate the first alpha with `delta0` then the second alpha with `delta1`. |
| `0bxxxx-xx11` | **8 bits** | **DELTA_LARGE** | Large delta. `delta = sign_extend(bits[7:2], 6)`, range **[-32, +31]**. |

##### Special Tags Details

* **ALPHA_TAG (0xFD)**  

  Followed by **1 byte** of raw alpha value, setting the current pixel directly. 

  > [!NOTE]
  >
  > The `u3AlphaMSBCount` does **NOT** affect **ALPHA_TAG**. It always uses 8-bit. 

  

* **GRADIENT_TAG (0xF9)**  

  Followed by **3 bytes**: `uint8_t to_alpha`, `int16_t count`.  Linearly interpolates from current alpha to target alpha using **Q15.16 fixed-point** arithmetic over `count + 1` pixels:

```c
steps = count + 1;
q16_step = ((to_alpha - previous_alpha) << 16) / steps;

for (i = 1; i < steps; i++)
    out[i-1] = prev + ((q16_step * i) >> 16);

out[steps-1] = to_alpha;
previous_alpha = to_alpha;
```

> [!IMPORTANT]
>
> When using a gradient mode, regardless of `u3AlphaMSBCount`, the start and stop alpha values are treated as 8-bit alpha. If the previous pixel is different from the actual alpha, an **ALPHA_TAG** **MUST**  be inserted to set the start alpha. 



* **DO / WHILE**  
  - **Paired**: Instructions between **DO** and **WHILE** form a **macro**, executed `count + 1` times. **Nesting is not allowed**.
  - **Standalone DO** (without **WHILE**) or **WHILE** (without **DO**) is ignored. 


> [!IMPORTANT]
>
> The **DO / WHILE** implements a loop structure. If you want to repeat the exact same string of pixels, you have to explicitly specify the pixel alpha at the start of an iteration; otherwise, for each iteration, the start alpha (a.k.a the alpha at the end of the previous iteration) might be different, leading to a totally different string of pixels. 
>
> Please implement **DO / WHILE** as a feature, not a **BUG**. 



#### **Delta arithmetic (DELTA_SMALL / DELTA_LARGE)**

All delta operations are performed in the msb-domain with wrap-around. 

```pseudocode
Let msb = u3AlphaMSBCount + 1, shift = 8 - msb, and MOD = 1 << msb.

prev_msb = previous_alpha >> shift
next_msb = (prev_msb + delta) mod MOD    // wrap-around, NOT saturation
next_alpha = (next_msb << shift) | LSB_policy
previous_alpha = next_alpha

When msb < 8, LSB_policy is decoder-defined (LSBs are don't-care).
(For example: set all LSBs to 0 for deterministic output.)
```



### 3.2 Decoding Procedure
Per-line decoder state:
1. Calculate the start address of a line based on the **Floor Table** and the **Line Index Table**.
2. Read 1 byte as `previous_alpha`.
3. Loop until `width` pixels are output:
   i)  Read a tag
   ii) Inspect the lowest 2 bits:
     - If `...00`: consume 8 bits, parse **INDEX** or **DO / WHILE**.
     - If `...01`: consume 8 bits, parse **REPEAT** or **special tags**, i.e. **ALPHA_TAG** and **GRADIENT_TAG**
     - If `...10`: consume 8 bits, execute **DELTA_SMALL**.
     - If `...11`: consume 8 bits, execute **DELTA_LARGE**.
4. Each Tag should update the `previous_alpha`.
5. Truncate immediately when line width is reached; discard remaining bits for that line.



## 4 Tag Sets Scheme Reserved for Future

There is a bitfield `u2TagSetBits` defined in the `arm_cm_header_t`. It determines how many **MSB** bits in each line index are used as **TAG_SET**.  Here:

* When `u2TagSetBits` are `0`, the **TAG_SET** of each line is seen as `0`.  In this version of the spec, the `u2TagSetBits` should always be `0`.
* **TAG_SETs** are read in little-endian. 
  * `0` is the TAG set defined in this spec.
  * Other values are reserved for the future.
* The floor advance is calculated as `1 << (16 - u2TagSetBits)`.
* Each line can have a different **TAG_SET** value in the future. In this version of the spec, all lines have the default **TAG_SET** value `0`.