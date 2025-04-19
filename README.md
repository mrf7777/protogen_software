# Protogen Software
![Protogen demo animation.](/assets/protogen_demo_short.gif)

This is the repository for a protogen software application.

## How to build and run
Follow these steps in order.

### Install rgb matrix library.
Go here and read this: https://learn.adafruit.com/adafruit-rgb-matrix-bonnet-for-raspberry-pi/driving-matrices

Run step 6 at the home directory of your pi.

### Setup development dependencies.
Run `sh setup.sh` at the root of this repository. This will build the led matrix library that this repo depends on which allows this project to access the led matrices. It also installs packages which allow for the compilation of utilities in the led matrix library.

### Build the protogen software.
At the root of this repository, run `mkdir build`. This will be your build directory. Run `cd build`.
Run `cmake ..` and then `cmake --build .` to build the protogen software. If it does not build, open an issue if it does not yet exist.

If the build is successful, there should be an executable named `protogen` in your build directory.
To run it, you may need `sudo`: try running as `sudo ./protogen` in your build directory. The `sudo` is needed
to be able to drive the led matrices.

### Install the protogen software.
Run `sudo cmake --install .` in your build directory. This will place the executable and resources in your standard
install location and should be accessible via your PATH variable by default on most unix/linux systems.

To test if your install worked, try running `sudo protogen`. If changes are made to your local repository, running
`cmake --build .` then `sudo cmake --install .` will install the latest changes.

## How to run on device boot
Often, one just wants the protogen software to start when the device is booted.
This repository has scripts to manage just that.

### Enable/disable start on device boot.
To **enable**, at the root of this repository, run `sh scripts/for_server/enable_protogen_service.sh`.

To **disable**, at the root of this repository, run `sh scripts/for_server/disable_protogen_service.sh`.

## Resources (for developers)
How to use a small MEMS mic and I2C sound sensor module to measure loudness:
https://pcbartists.com/product-documentation/accurate-raspberry-pi-decibel-meter/

## New feature ideas
- Party games: bowling (with phone-swipe as "throwing" ball), trivia, battleship, multi-player pokemon, bomb defusing (or protogen gets blue screen of death)
- Solo games: zen garden, pokemon, tetris, fnaf
- Standardization
  - File format for packaging parametric protogen emotion sets.
  - "app store" idea for community-created apps for the protogen.
    - How can we ensure that installing apps is easy for non-technical or non-expert users?
- Vowel detection and switching between mouth-frame sets based on vowel sound.
- Protogen-to-Protogen interaction/communication.
  - If connected to internet and have gps-like module, each protogen can publish their lat/lon continuously. 
  - If protogen has some kind of NFC or bluetooth-like connectivity, maybe that could work.
- Voice-modification with mic and speaker.

## Tasks:
- [ ] document interface for creating and destroying extensions.
- [x] implement initializer for ISensor.
- [x] implement initializer for IRenderSurface.

## Refactor ideas
- How do we run nginx and reload it if its already running in C++?
  - How about simply attempting to manage nginx, and if there are permission issues, report to user and exit.
- Do we need to abstract away nginx and just think in terms of "reverse proxy server"?
  - Would not hurt.