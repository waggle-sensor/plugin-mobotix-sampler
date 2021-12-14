#!/usr/bin/env python3
"""
Created on Mon Dec 13 11:05:11 2021

1. Runs the Mobotix C++ sampler with the given arguments.
2. The .rgb files are too large, so we used ffmpeg to convert to JPG.
"""

import argparse
import sys
import glob
import os
import subprocess
import re


from waggle.plugin import Plugin


def convertRGBtoJPG():
    rgb_files = glob.glob("*.rgb")
    for f_rgb in rgb_files:
        # make JPEG file name
        f_jpg = f_rgb.replace(".rgb", ".jpg")
        
        # get the Width and Height of the image from filename
        match_str = re.search('\d\d\d\dx\d\d\d\d', f_rgb)
        image_dims = match_str.group()
        subprocess.run(['ffmpeg', '-f', 'rawvideo', '-pixel_format', 'bgra', 
                        '-video_size', image_dims, '-i', f_rgb, f_jpg])
        
        print("Removing "+f_rgb)
        os.remove(f_rgb)
        
        
def renameFiles():
    files = sum([ glob.glob(f) for f in ("*.raw","*.jpg","*.csv") ], [])
    timestamp = []
    new_filenames = []
    for f in files:
        timestamp_str, filename = f.split("_", 1)
        
        timestamp.append(int(timestamp_str))
        new_filenames.append(filename)
        
        print("Renaming "+f)
        os.rename(f, filename)
        
    return new_filenames, timestamp
        
        
def cleanup():
    all_files = glob.glob("*")
    for f in all_files:
        print("Removing "+f)
        os.remove(f)


def main(args):
        
    #create output directory if it does not exist and change WD.
    if not os.path.exists(args.o):
        os.makedirs(args.o)
        
    os.chdir(args.o)
    
    
    while True:
        # Run the Mobotix sampler
        subprocess.run(["/eventstreamclient/samples/thermal-raw/build/thermal-raw", 
                            args.ip]) #, args.id, args.pw])

        convertRGBtoJPG()
        filenames, timestamp = renameFiles()
        
        for index, filename in enumerate(filenames):
            print(filename)
            print(timestamp[index])
            plugin.upload_file(filename, timestamp=timestamp[index])


        cleanup()

        exit()

    

    

    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='''
                                     This program runs Mobotix sampler for raw 
                                     storing thermal data.''')
    parser.add_argument('--ip', type=str, 
                        help='Camera IP or URL', 
                        default="10.10.10.1")
    parser.add_argument('--id', type=str, 
                        help='Authenticator User ID.',
                        default="admin")
    parser.add_argument('--pw', type=str,
                        help='Authenticator Password.', default="password")
    parser.add_argument('--o', type=str, 
                        help='Output directory', default="./data/")
    parser.add_argument('--i', type=int,
                        help='Interval [sec] (Unused)', default=1)
    
    args = parser.parse_args()
    main(args)


