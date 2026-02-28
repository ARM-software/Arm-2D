#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
GIF to PNG Sprite Sheet Converter
Flattens GIF animation frames into a single PNG sprite sheet
"""

import argparse
import sys
from pathlib import Path
from PIL import Image


def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description='Convert GIF animation to a flattened PNG sprite sheet',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python gif2png.py animation.gif -n 5
  python gif2png.py animation.gif -n 8 --dim 64 64
  python gif2png.py animation.gif -n 4 --dim 100 100 -o output.png
        """
    )
    
    parser.add_argument(
        'input',
        type=str,
        help='Input GIF file path'
    )
    
    parser.add_argument(
        '-n', '--frames-per-row',
        type=int,
        default=5,
        metavar='N',
        help='Number of frames per row (default: 5)'
    )
    
    parser.add_argument(
        '--dim',
        type=int,
        nargs=2,
        metavar=('WIDTH', 'HEIGHT'),
        help='Resize each frame to specified width and height, e.g.: --dim 64 64'
    )
    
    parser.add_argument(
        '-o', '--output',
        type=str,
        metavar='PATH',
        help='Output file path (default: same name as input with .png extension)'
    )
    
    return parser.parse_args()


def process_gif(input_path, frames_per_row, target_size=None, output_path=None):
    """
    Process GIF file and flatten frames into a PNG sprite sheet
    
    Args:
        input_path: Input GIF file path
        frames_per_row: Number of frames per row
        target_size: (width, height) tuple, or None to keep original size
        output_path: Output file path, or None to auto-generate
    """
    # Open GIF file
    try:
        gif = Image.open(input_path)
    except FileNotFoundError:
        print(f"Error: File not found - {input_path}")
        sys.exit(1)
    except Exception as e:
        print(f"Error: Cannot open file - {e}")
        sys.exit(1)
    
    # Check if file is GIF format
    if gif.format != 'GIF':
        print(f"Warning: Input file format is {gif.format}, not standard GIF format")
    
    # Extract all frames
    frames = []
    try:
        while True:
            # Copy current frame (seek changes current frame)
            frame = gif.copy()
            
            # Convert to RGBA mode to support transparency
            if frame.mode != 'RGBA':
                frame = frame.convert('RGBA')
            
            # Resize if target size specified
            if target_size:
                frame = frame.resize(target_size, Image.Resampling.LANCZOS)
            
            frames.append(frame)
            
            # Move to next frame
            gif.seek(gif.tell() + 1)
    except EOFError:
        pass  # Reached end of frames
    
    if not frames:
        print("Error: No frames extracted")
        sys.exit(1)
    
    print(f"Successfully extracted {len(frames)} frames")
    
    # Get frame dimensions (all frames have same size after resize)
    frame_width, frame_height = frames[0].size
    
    # Calculate grid layout
    total_frames = len(frames)
    num_cols = min(frames_per_row, total_frames)
    num_rows = (total_frames + frames_per_row - 1) // frames_per_row  # Ceiling division
    
    print(f"Layout: {num_rows} rows x {num_cols} columns")
    print(f"Frame size: {frame_width}x{frame_height}")
    
    # Create canvas
    canvas_width = num_cols * frame_width
    canvas_height = num_rows * frame_height
    
    print(f"Output image size: {canvas_width}x{canvas_height}")
    
    # Create canvas with transparent background
    sprite_sheet = Image.new('RGBA', (canvas_width, canvas_height), (0, 0, 0, 0))
    
    # Arrange frames row by row
    for idx, frame in enumerate(frames):
        row = idx // frames_per_row
        col = idx % frames_per_row
        
        x = col * frame_width
        y = row * frame_height
        
        sprite_sheet.paste(frame, (x, y), frame)  # Use frame as mask to preserve transparency
    
    # Determine output path
    if output_path is None:
        input_path = Path(input_path)
        output_path = input_path.with_suffix('.png')
    else:
        output_path = Path(output_path)
    
    # Save PNG file
    try:
        # Optimize PNG file size
        sprite_sheet.save(
            output_path,
            'PNG',
            optimize=True,
            compress_level=9
        )
        print(f"Saved: {output_path.absolute()}")
    except Exception as e:
        print(f"Error: Failed to save file - {e}")
        sys.exit(1)


def main():
    args = parse_args()
    
    # Validate arguments
    if args.frames_per_row <= 0:
        print("Error: Frames per row must be greater than 0")
        sys.exit(1)
    
    if args.dim:
        width, height = args.dim
        if width <= 0 or height <= 0:
            print("Error: Dimensions must be greater than 0")
            sys.exit(1)
        target_size = (width, height)
    else:
        target_size = None
    
    # Process GIF
    process_gif(
        input_path=args.input,
        frames_per_row=args.frames_per_row,
        target_size=target_size,
        output_path=args.output
    )


if __name__ == '__main__':
    main()