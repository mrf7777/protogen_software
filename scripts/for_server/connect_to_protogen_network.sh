#!/bin/bash

# Connects to the protogen network.
# This script is intended to be ran on the device that is running
# the protogen code/server.
# This script will connect it to the protogen network.

sudo nmcli connection modify\
	preconfigured\
	connection.autoconnect no\
	connection.autoconnect-priority -999

sudo nmcli connection add\
	type wifi\
	con-name Protogen-Network-as-Server\
	ifname wlan0\
	ssid Protogen-Network\
	password 1234567890\
	connection.autoconnect yes\
	connection.autoconnect-priority 999\
	ipv4.addresses 10.42.0.2/24\
	ipv4.method manual
