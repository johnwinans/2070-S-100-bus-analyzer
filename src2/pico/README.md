# A PICO Logic Analyzer

NOTE: This software is based on the [Dr.G Logic Analyzer](https://github.com/gusmanb/logicanalyzer) but altered to add new features.
This implementation uses a protocol that is not compatible with the Dr.G GUI application.

This has been verified by building on a Raspberry PI 4B running Trixie.

This is pre-configured for a PICO2.  
Adjust your build if needed by editing `LogicAnalyzer_Build_Settings.cmake`


1. Install the PICO development toolchain as described at: https://github.com/raspberrypi/pico-setup
1. Clone this REPO and build it using the above PICO toolchain from this directory:
```
mkdir build
cd build
cmake ..
make
```
1. Install the firmware on your PICO: power up your PICO while holding down the BOOTSEL button and then uising `picotool`:
```
picotool load LogicAnalyzer.uf2 -vx
```
