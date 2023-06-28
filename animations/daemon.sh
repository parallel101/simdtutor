#!/bin/bash
(readlink media && test -d media) || (rm -rf media && mkdir -p /tmp/.W$$.manim.media && ln -sf /tmp/.W$$.manim.media media)
while true
do
    python -O -m manim -pqh scene.py $1
    inotifywait scene.py -o /tmp/.W$$.ionotify.log -e close --timefmt '%y/%m/%d %H:%M:%S' --format '%T %w %f %e'
done
