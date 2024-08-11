#!/bin/bash

sudo nmcli connection modify\
	Protogen-Network\
	connection.autoconnect no

sudo nmcli connection down\
	Protogen-Network\

sudo nmcli connection modify\
	preconfigured\
	connection.autoconnect yes

sudo nmcli connection up\
	preconfigured\
	ifname wlan0

