#!/bin/bash
set -e
set -o pipefail
scenes=$(cat scene.py | sed -n '/^class \(\w\+\)(Scene):$/p' | sed 's/^class \(\w\+\)(Scene):$/\1/g')
python -O -m manim -qh scene.py $scenes
echo > /tmp/.W$$.filelist.txt
for x in $scenes; do
    echo "file '$PWD/media/videos/scene/1080p60/$x.mp4'" >> /tmp/.W$$.filelist.txt
done
ffmpeg -hwaccel auto -f concat -safe 0 -i /tmp/.W$$.filelist.txt -c copy /tmp/manim.mp4
