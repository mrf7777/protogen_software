sudo apt update
sudo apt upgrade

# required for some utilites in the rpi rgb led matrix library
sudo apt-get install libgraphicsmagick++-dev libwebp-dev -y

# SDL for window-in-desktop visualization and control when
# real protogen hardware is not avaliable.
sudo apt-get install libsdl2-dev -y

# compile tools
sudo apt-get install cmake -y

# disable the sound module if not disabled already
blacklist_file=/etc/modprobe.d/alsa-blacklist.conf
sudo touch $blacklist_file
if sudo grep -q "audio_has_been_blacklisted_evidence" $blacklist_file; then
	echo "Audio already disabled."
else
	echo "" | sudo tee -a $blacklist_file
	echo "# auto-generated audio blacklist from protogen software project setup script" | sudo tee -a $blacklist_file
	echo "# audio_has_been_blacklisted_evidence" | sudo tee -a $blacklist_file
	echo "blacklist snd_bcm2835" | sudo tee -a $blacklist_file
fi

cd ~/rpi-rgb-led-matrix
make
