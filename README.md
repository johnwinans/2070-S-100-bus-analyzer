# 2070-S-100-bus-analyzer

A 120 channel S-100 bus logic analyzer

This project is based on the open source ["gusmanb" Logic Analyzer](https://github.com/gusmanb/logicanalyzer) project from Agustín Gimenez Bernad

I have started a [YouTube playlist](https://www.youtube.com/playlist?list=PL3by7evD3F51xCtWdVP3EZzuDttMwDS_U) of videos about this project and my testing of the board.

By using the software from the gusmanb/logicanalyzer repo, a 120 channel logic analyzer can be assembled by installing 5 Raspberry PI PICO boards on this S-100 card and accessed using a USB cables to a PC and/or WiFi as described in the above gusmanb github project Wiki.

Here is a [CSV of the project BOM](./2070-S-100-bus-analyzer.csv) with Dikikey order numbers. 

NOTE: I used generic male-headers that I got on Amazon and snapped to length.  I only use gold-plated headers.  Anything else is annoying.
Here is a link to the type that I use for most everything: https://a.co/d/02YRvqt7 (Hint: I snap them to length using a flat [Hakko Wire Cutters](https://a.co/d/055L9axg). Then use shunts to hold them in pairs, tripples, or whatever you need so they stay straight and aligned when soldering.)

# Rev 1.0

***NOTICE: As of April 20, 2026 Rev 1.0 of this design has been tested and is a fully functional board.  However, I am making some cosmetic tweaks (silkscreen, better testpoints, and I might shorten the PCB by 1/8"... see my self-critique in the above playlist.)  I will post a release (and remove this message) when I consider it in final form.***

# Rev 2.0

![PCB image](./2070-S-100-bus-analyzer.jpg)

***NOTE: This is a release in progress.  It has not yet been tested.***

## Changes
- Change the board height to 5.0 inches to better fit into IEEE card cages.
- Change to use 3-row headers so that signals can be connected to a trigger input at the same time as the normal analyzedr channel with simpler jumper wires.
- Remove the alternate regulator option.
- Add special headers for pins 20 and 70 to make their purpose more obvious.
- Change to use thru-hole ground test-points for easier use with gator-clips.
- Use larger fonts and better-center the signal names.
