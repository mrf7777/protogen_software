# Getting started
Go here and read this: https://learn.adafruit.com/adafruit-rgb-matrix-bonnet-for-raspberry-pi/driving-matrices

Run step 6 at the home directory of your pi.

Run `sh setup.sh` in this directory. This will build the led matrix library that this repo depends on which allows this project to access the led matrices. It also installs packages which allow for the compilation of utilities in the led matrix library.

Run `sh build.sh` to build this project.

## Resources
Wi-Fi Direct seems to be a method which allows mobile devices (such as Android) to connect directly to the Pi even with other network settings.

Detailed setup: https://raspberrypi.stackexchange.com/a/117239

Script someone made that does all of these steps: https://github.com/MrStashley/Android-RasPi-WifiP2P-Communication/blob/main/zero-w-config.sh

The repo where that script lives: https://github.com/MrStashley/Android-RasPi-WifiP2P-Communication?tab=readme-ov-file

How to use a small MEMS mic and I2C sound sensor module to measure loudness:
https://pcbartists.com/product-documentation/accurate-raspberry-pi-decibel-meter/
