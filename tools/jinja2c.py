#!/usr/bin/python
# -*- coding: utf-8 -*-

# *************************************************************************************************
#  Arm 2D project
#  @file        jinja2c.py
#  @brief       Jinja Launcher for arm-2d code templates
#
# *************************************************************************************************
#
# * Copyright (C) 2010-2024 ARM Limited or its affiliates. All rights reserved.
# *
# * SPDX-License-Identifier: Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the License); you may
# * not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an AS IS BASIS, WITHOUT
# * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# */

import sys
import argparse
import json
import os
from jinja2 import Environment, FileSystemLoader


def main(argv):

    # Parse the input
    parser = argparse.ArgumentParser(description='Jinja Launcher for arm-2d code templates (v1.1.0)')

    parser.add_argument("-i",   "--input",          type = str, help="Path of the input jinja2 template file", required=False)
    parser.add_argument("-var", "--variable",       type = str, help="an optional path of the JSON file containing jinja2 template variables", required=False)
    parser.add_argument("-f",   "--folder",         type = str, help="an optional path for specify the working folder", required=False)
    parser.add_argument("-out", "--output_folder",  type = str, help="an optional path for oputput folder", required=False)
    parser.add_argument("-o",   "--output",         type = str, help="an optional path of the output file, if you ignore this argument and the input template ends with '.jinja', this tool will remove it and use the rest part as the output file name.", required=False)

    args = parser.parse_args()

    if args.input == None or args.input == "" :
        parser.print_help()
        exit(1)


    if args.folder == None or args.folder == "" :
        args.folder = "../Library/jinja"
    
    if not os.path.isdir(args.folder):
        print(f"Error: The folder {args.folder} does not exist.")
        sys.exit(1)

    output_file = args.output
    if output_file is None:
        if args.input.endswith('.jinja'):
            output_file = args.input[:-6]  # remove ".jinja" extension

            if args.output_folder == None or args.output_folder == "" :
                if output_file.endswith('.c') :
                    args.output_folder = "../Source"
                if output_file.endswith('.h') :
                    args.output_folder = "../Include"

            output_file = os.path.join(args.folder, args.output_folder, output_file)
        else:
            output_file = None

    if not os.path.isdir(os.path.join(args.folder, args.output_folder)):
        print(f"Error: The folder {os.path.join(args.folder, args.output_folder)} does not exist.")
        sys.exit(1)
    

    #Template environment
    env = Environment(loader=FileSystemLoader(args.folder))

    # Load the template file
    if args.input and not os.path.isfile(os.path.join(args.folder, args.input)):
        print(f"Error: The template file {os.path.join(args.folder, args.input)} does not exist.")
        sys.exit(1)
    
    template = env.get_template(args.input)

    # Load variables

    variables = {}
    if args.variable is not None:
        if not os.path.isfile(os.path.join(args.folder, args.variable)):
            print(f"Error: The jason file {os.path.join(args.folder, args.variable)} does not exist.")
            sys.exit(1)
        with open(os.path.join(args.folder, args.variable), 'r') as f:
            variables = json.load(f)

    # generate output
    output = template.render(variables)

    if output_file is not None:
        with open(output_file, 'w') as f:
            f.write(output)
    else:
        print(output)


if __name__ == '__main__':
    main(sys.argv[1:])