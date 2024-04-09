# How to use Jinja for Arm-2D code generation.

## Preparation

To use Jinja for Arm-2D code generation, you have to install python3 and `jinjia2` with the following command line:

```sh
pip install Jinja2
```

After that, we can use the `jinja2c.py` in `tools` folder to generate C source files and Header files.

Before  writing any Jinja script, please read the [syntax manual](https://jinja.palletsprojects.com/en/3.1.x/templates/) first. 



## How to use Jinja2c.py (v1.1.0)

You can use the following command line:

```sh
jinja2c.py [-h] [-i INPUT] [-var VARIABLE] [-f FOLDER] [-out OUTPUT_FOLDER] [-o OUTPUT]
```

| Options                                               | Description                                         | NOTE                                                         |
| ----------------------------------------------------- | --------------------------------------------------- | ------------------------------------------------------------ |
| **-h, --help**                                        | Show help message and exit                          |                                                              |
| **-i INPUT, --input INPUT**                           | The name of the input jinja2 template file          |                                                              |
| **-var VARIABLE, --variable VARIABLE**                | An optional path of the JSON file containing jinja2 | Optional                                                     |
| **-f FOLDER, --folder FOLDER**                        | An optional path for specify the working folder     | Optional, the default path is `"../Library/jinja"`           |
| **-out OUTPUT_FOLDER, --output_folder OUTPUT_FOLDER** | An optional path for oputput folder                 | Optional, the default path is `"../Source"` for `"*.c.jinja"` and `"../Include"` for `"*.h.jinja"` |
| **-o OUTPUT, --output OUTPUT**                        | an optional path of the output file                 | Optional, if you ignore this argument and the input template ends with '.jinja', this tool will remove it and use the rest part as the output file name. |



## Examples

To generate `__arm_2d_fill_colour_with_mask_and_mirroring.c` and store the C source file to `Library/Source` folder, please enter the `tools` directory and type the following command line:

```sh
python3 jinja2c.py -i __arm_2d_fill_colour_with_mask_and_mirroring.c.jinja
```

**NOTE:**

1. This command line use the default working folder, i.e. `"Library/Jinja"`, where you can find the target Jinja script `__arm_2d_fill_colour_with_mask_and_mirroring.c.jinja`
2. The generated C source file, i.e. `__arm_2d_fill_colour_with_mask_and_mirroring.c` will be stored to the default output folder, i.e. `"../Source"`.
3. The output file name comes from the script name `__arm_2d_fill_colour_with_mask_and_mirroring.c.jinja` with the extension name `.jinja` removed. 



To generate `__arm_2d_fill_colour_with_mask_and_mirroring.h` and store the Header file to `Library/Include` folder, please enter the `tools` directory and type the following command line:

```sh
python3 jinja2c.py -i __arm_2d_fill_colour_with_mask_and_mirroring.h.jinja
```

**NOTE:**

1. This command line use the default working folder, i.e. `"Library/Jinja"`, where you can find the target Jinja script `__arm_2d_fill_colour_with_mask_and_mirroring.h.jinja`
3. The output file name comes from the script name `__arm_2d_fill_colour_with_mask_and_mirroring.h.jinja` with the extension name `.jinja` removed. 
