xhost + local:
docker run --cap-add CAP_SYS_ADMIN -e DISPLAY -e XAUTHORITY=/root/.Xauthority -v /tmp/.X11-unix:/tmp/.X11-unix:rw -v $HOME/.Xauthority:/root/.Xauthority:rw -v $(realpath $(dirname $0))/..:/root/workspace -v /tmp:/tmp/host -v /home/bate/Codes/zeno3:/root/zeno3 -it --rm archibate/oneapi
