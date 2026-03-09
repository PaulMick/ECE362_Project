# Overview
This repository is for the code for an ECE 36200 group project at Purdue University. The project is a retro-style game similar to Space Invaders implemented on the Proton board, an RP2350-based board. The game comes with a HUB75 RGB LED matrix, small PWM speakers to play audio from the game, an analog dial and buttons for game input, and a 24AA32AF EEPROM for saving high score data.

# Code Structure
The code, in its current form, is broken up into a few main sections.

## Display Driver
This is the part of the code that handles all of the lower-level HUB75 display control. In `display_driver.c` all of the output pins to the display and some display constants (e.x. width, height) are defined.

## Display Utils

## Pico Display

# A few other things
## platformio.ini
Make sure to set the optimization build flag to something other than 0 (e.x. `-O1` or `-O3`). If it is set to zero the display driver will run slow enough that the display will have visible flickers and/or artifacts.
