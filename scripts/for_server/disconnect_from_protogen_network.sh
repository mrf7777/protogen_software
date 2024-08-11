#!/bin/bash

sudo nmcli connection modify\
	Protogen-Network-for-Server\
	connection.autoconnect no

sudo nmcli connection down\
	Protogen-Network-for-Server

sudo nmcli connection modify\
	preconfigured\
	connection.autoconnect yes

sudo nmcli connection up\
	preconfigured\
	ifname wlan0

