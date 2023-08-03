#!/bin/bash
set -e
set -o pipefail
mkdir /tmp/.W$$.tmpbin
cat > /tmp/.W$$.tmpbin/ffmpeg << __EOF__
#!/bin/bash
exec ffmpeg -hwaccel auto "$@"
__EOF__
chmod +x /tmp/.W$$.tmpbin/ffmpeg
export PATH=/tmp/.W$$.tmpbin/ffmpeg:"$PATH"
scenes=$(cat scene.py | sed -n '/^class \(\w\+\)(Scene):$/p' | sed 's/^class \(\w\+\)(Scene):$/\1/g')
if false; then
    python -O -m manim -qh scene.py $scenes
    # for x in $scenes; do
    #     echo python -O -m manim -qh scene.py $x >> /tmp/.W$$.cmdlist.txt
    # done
    # ~/.local/bin/runp /tmp/.W$$.cmdlist.txt
fi
echo > /tmp/.W$$.filelist.txt
for x in $scenes; do
    echo "file '$PWD/media/videos/scene/1080p60/$x.mp4'" >> /tmp/.W$$.filelist.txt
done
ffmpeg -y -hwaccel auto -f concat -safe 0 -i /tmp/.W$$.filelist.txt -c copy /tmp/manim.mp4
rm -rf /tmp/.W$$.tmpbin /tmp/.W$$.filelist.txt /tmp/.W$$.cmdlist.txt
