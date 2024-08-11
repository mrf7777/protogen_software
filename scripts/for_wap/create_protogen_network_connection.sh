#!/bin/bash

# The device acting as the wireless access point (WAP) should run this script.
# This disables DHCP so that everyone on the network has a reliable IP address. This
# is important because mDNS does not work on all devices and is sometimes unreliable.
# 
# If you need to access the internet, you can set the
# connection.autoconnect setting for the 'preconfigured' network
# to the value 'yes'.

sudo nmcli connection modify\
	preconfigured\
	connection.autoconnect no\
	connection.autoconnect-priority -999

sudo nmcli device wifi hotspot \
	con-name Protogen-Network\
	ifname wlan0\
       	ssid Protogen-Network\
       	password 1234567890

sudo nmcli connection modify\
	Protogen-Network\
	connection.autoconnect yes\
	connection.autoconnect-priority 999\
	ipv4.addresses 10.42.0.1/24\
	ipv4.method manual\


