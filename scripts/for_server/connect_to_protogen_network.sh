#!/bin/bash

# Connects to the protogen network.
# This script is intended to be ran on the device that is running
# the protogen code/server.
# This script will connect it to the protogen network.

sudo nmcli connection modify\
	preconfigured\
	connection.autoconnect no\
	connection.autoconnect-priority -999

sufo nmcli device wifi connect\
	Protogen-Network\
	password 1234567890\
	ifname wlan0\
	name Protogen-Network-as-Server

sudo nmcli connection modify\
	Protogen-Network-as-Server\
	ipv4.method manual\
	ipv4.addresses 10.42.0.2/24\
	connection.autoconnect yes\
	connection.autoconnect-priority 999

