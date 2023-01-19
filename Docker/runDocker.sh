docker run --name mppe --rm -ti -e DISPLAY=$DISPLAY  -v /home/$USER:/scratch -v /tmp/.X11-unix:/tmp/.X11-unix:ro  -p 8000:8000 akalinow/root-fedora35

