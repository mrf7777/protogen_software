#!/bin/bash

# Make the default connection not auto-connect.
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
	connection.autoconnect-priority 999

