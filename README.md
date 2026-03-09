# Overview
This repository is for the code for an ECE 36200 group project at Purdue University. The project is a retro-style game similar to Space Invaders implemented on the Proton board, an RP2350-based board. The game comes with a HUB75 RGB LED matrix, small PWM speakers to play audio from the game, an analog dial and buttons for game input, and a 24AA32AF EEPROM for saving high score data.

# Code Structure
The code, in its current form, is broken up into a few main sections.

## Display Driver
This is the part of the code that handles all of the lower-level HUB75 display control. In `display_driver.c` all of the output pins to the display and some display constants (e.x. width, height) are defined. The two buffers that are used for storing the image to be displayed are created here. Additionally, the code to initialize and control all of the peripherals used in driving the display lies in this file too (with the exeption of the PIO assembly).

Overall, the display driver works like this:<br>
1. `init_display_driver` is called which:<br>
1a. allocates and initializes the two buffers<br>
1b. initializes the PIO state machines<br>
1c. starts the refresh process on core 1<br>
1d. returns a `DisplayHandle` instance, which is later used to start drawing things on the display<br>

2. `refresh` starts running on core 1, which:<br>
2a. pull 2 rows from the buffer (since this is intended to use a 2-scan matrix), and applies gamma correction<br>
2b. feeds these gamma-corrected pixels to the `rgb` PIO state machine<br>
2c. feeds the row selection and brightness value to the `row` PIO state machine<br>
2d. repeats for all row pairs (of which there are 16)<br>
2e. repeats for all color depths (of which there are 8)<br>

3. some other process with a `DisplayHandle` instance invokes `update_frame` which:<br>
3a. pushes a SWAP command into the FIFO of core 1. This SWAP command is then popped by the `refresh` function, indicating to it to swap the buffers and start displaying the new one

### Double Buffering
This display driver uses a technique known as "double-buffering", which is a common way of avoiding issues when two processes are trying to operate on the same region of memory at the same time (in this case, the `refresh` process is reading data from the buffer to be sent to the PIO state machines and another process is writing data to the buffer to draw new stuff on the frame). This issue can often lead to glitches and artifacts caused by the two processes clashing over that memory.

Double buffering works by creating two buffers that can be readily swapped when needed. In this scenario, the lets `refresh` read from one buffer and the drawing process write to the other. When the drawing process is done drawing the next frame, it pushes a SWAP command into core 1's FIFO. `refresh` checks before displaying a frame if there is anything in its FIFO, and if there is a SWAP command it will swap the pointers to the two buffers, then continue displaying. This means that the buffer with the new drawings in it is now being displayed, and the old buffer is now being drawn over.

### HUB75 Protocol
The HUB75 protocol is a protocal for controlling an RGB LED matrix with shift registers, usually ones onboard the display. It uses the following pins:
|Pin|Function|
|---|--------|
|R0|Red bit for the top scan line|

### PIO State Machines
The PIO (which stands for Programmable Input/Output) is a unique peripheral that features 8 state machines that can be used for many different things. In this case we are using 2 of them to clock in RGB pixels to the display

## Display Utils

## Pico Display

# A few other things
## platformio.ini
Make sure to set the optimization build flag to something other than 0 (e.x. `-O1` or `-O3`). If it is set to zero the display driver will run slow enough that the display will have visible flickers and/or artifacts.
