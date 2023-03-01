docker run --name mppe --rm -e DISPLAY=$DISPLAY  -v /home/$USER:/scratch -v /tmp/.X11-unix:/tmp/.X11-unix:ro  -w /home/jupyter -p 8000:8000 akalinow/root-fedora35 /home/jupyter/start-jupyter.sh

