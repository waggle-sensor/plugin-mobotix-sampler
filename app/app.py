#!/usr/bin/env python3
"""
Created on Mon Dec 13 11:05:11 2021

1. Runs the Mobotix C++ sampler with the given arguments.
2. The .rgb files are too large, so we used ffmpeg to convert to JPG.
"""

import argparse
import glob
import logging
import os
from pathlib import Path
import re
import shutil
import subprocess
from select import select

import timeout_decorator
from waggle.plugin import Plugin

# camera image fetch timeout (seconds)
DEFAULT_CAMERA_TIMEOUT = 120


def convertRGBtoJPG(fname_rgb):
        fname_jpg = fname_rgb.replace(".rgb", ".jpg")

        # get the Width and Height of the image from filename
        match_str = re.search("\d+x\d+", fname_rgb)
        image_dims = match_str.group()
        subprocess.run(
            [
                "ffmpeg",
                "-f",
                "rawvideo",
                "-pixel_format",
                "bgra",
                "-video_size",
                image_dims,
                "-i",
                fname_rgb,
                fname_jpg,
            ],
            check=True
        )

        logging.debug("Removing " + fname_rgb)
        os.remove(fname_rgb)
        return fname_jpg


def renameFiles(f):
    fname = os.path.basename(f)
    dirname = os.path.dirname(f)
    
    timestamp_str, filename = fname.split("_", 1)
    filename = os.path.join(dirname, filename)
    timestamp = int(timestamp_str)

    logging.debug("Renaming " + f)
    
    os.rename(f, filename)

    return filename, timestamp


@timeout_decorator.timeout(DEFAULT_CAMERA_TIMEOUT)
def get_camera_frames(args):
    cmd = [
        "/thermal-raw",
        "--url",
        args.ip,
        "--user",
        args.user,
        "--password",
        args.password,
        "--dir",
        args.workdir,
    ]
    with subprocess.Popen(cmd, stdout=subprocess.PIPE) as process:
        while True:
            pollresults = select([process.stdout], [], [], 5)[0]
            if pollresults:
                output = pollresults[0].readline()
                if output:
                    m = re.search("frame\s#(\d+)", output.strip().decode())
                    logging.info(output.strip().decode())
                    if m and int(m.groups()[0]) > args.frames:
                        logging.info("Max frame count reached, closing camera capture")
                        return


def main(args):

    with Plugin() as plugin:
        while True:
            # Run the Mobotix sampler
            try:
                get_camera_frames(args, timeout=args.camera_timeout)
            except timeout_decorator.timeout_decorator.TimeoutError:
                logging.warning(f"Timed out attempting to capture {args.frames} frames.")
                pass
            
            files = glob.glob(os.path.join(args.workdir, "*"))


            #for index, filename in enumerate(filenames):
                        
            for file in files:
                if Path(file).suffix == '.rgb':
                    file = convertRGBtoJPG(file)
                    
                filename, timestamp = renameFiles(file)
                
                logging.debug(filename)
                logging.debug(timestamp)
                
                plugin.upload_file(filename, timestamp=timestamp)
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="The plugin runs Mobotix sampler and collects raw thermal data."
    )
    parser.add_argument("--debug", action="store_true", help="enable debug logs")
    parser.add_argument(
        "--ip",
        required=True,
        type=str,
        dest="ip",
        default=os.getenv("CAMERA_IP", ""),
        help="Camera IP or URL",
    )
    parser.add_argument(
        "-u",
        "--user",
        dest="user",
        type=str,
        default=os.getenv("CAMERA_USER", "admin"),
        help="Camera User ID",
    )
    parser.add_argument(
        "-p",
        "--password",
        dest="password",
        type=str,
        default=os.getenv("CAMERA_PASSWORD", "meinsm"),
        help="Camera Password",
    )
    parser.add_argument(
        "-w",
        "--workdir",
        dest="workdir",
        type=str,
        default="./data",
        help="Directory to cache Camara data before upload",
    )
    parser.add_argument(
        "-f",
        "--frames",
        dest="frames",
        type=int,
        default=os.getenv("FRAMES_PER_RUN", 1),
        help="Frames to capture per run",
    )
    parser.add_argument(
        "-t",
        "--timeout",
        dest="camera_timeout",
        type=int,
        default=os.getenv("CAMERA_TIMEOUT", DEFAULT_CAMERA_TIMEOUT),
        help="Max time (in seconds) to capture frames from camera per run",
    )

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.debug else logging.INFO,
        format="%(asctime)s %(message)s",
        datefmt="%Y/%m/%d %H:%M:%S",
    )

    main(args)
