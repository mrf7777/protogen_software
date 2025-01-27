#!/bin/bash

apt update

# required for some utilites in the rpi rgb led matrix library
apt-get install libgraphicsmagick++-dev libwebp-dev -y

# SDL for window-in-desktop visualization and control when
# real protogen hardware is not avaliable.
apt-get install libsdl2-dev -y

# nginx as a reverse proxy web server
apt-get install nginx -y

# compile tools
apt-get install cmake curl -y

# disable the sound module if not disabled already
blacklist_file=/etc/modprobe.d/alsa-blacklist.conf
touch $blacklist_file
if grep -q "audio_has_been_blacklisted_evidence" $blacklist_file; then
	echo "Audio already disabled."
else
	echo "" | tee -a $blacklist_file
	echo "# auto-generated audio blacklist from protogen software project setup script" | tee -a $blacklist_file
	echo "# audio_has_been_blacklisted_evidence" | tee -a $blacklist_file
	echo "blacklist snd_bcm2835" | tee -a $blacklist_file
fi

# If running in a CI/CD environment, just go ahead and get the rpi-rgb-led-matrix library source.
# Usually, a developer would do this manually because it requires some human input.
if [ "$1" = "ci" ]; then
	cd "$HOME"
	curl https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/main/rgb-matrix.sh >rgb-matrix.sh
	bash rgb-matrix.sh <<< "y
1
2
y
n
"
	rm rgb-matrix.sh
fi

# build matrix library if not already built
cd ~/rpi-rgb-led-matrix
make
