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
|G0|Green bit for the top scan line|
|B0|Blue bit for the top scan line|
|R1|Red bit for the bottom scan line|
|G1|Green bit for the bottom scan line|
|B1|Blue bit for the bottom scan line|
|SEL0 (or A)|LSB for the row selection|
|SEL1 (or B)|bit for the row selection|
|SEL2 (or C)|bit for the row selection|
|SEL3 (or D)|MSB for the row selection|
|SEL4 (or E)|Selects panel 0 or 1 (only used with chained panels, unused here)|
|CLK|Clock for shifting in pixel, triggers on rising edge|
|LAT|Latches in row of pixels to a specific row on the display|
|OE|Output Enable, enables whole display output|

A HUB75 RGB LED matrix is almost always controlled in this fashion (starting with OE deasserted):
1. Using pins R0, G0, B0, R1, G1, B1, and CLK, shift in a row of color bits from a single color bit depth. It doesn't really matter whether this goes from LSB to MSB or MSB to LSB, or even in amy order really.
2. Output the row selection with SEL0, SEL1, SEL2, and SEL3
3. Toggle LAT high and then low
4. Assert OE (OE is usually active-low) for an amount of time proportional to 2 ^ currect color bit depth. This is known as binary code modulation (or BCM) and is how we achieve a whole gradient of colors. Since HUB75 RGB LED displays can only turn LEDs on or off, we modulate how long they are on in this way to effectively dim them from our point of view.
5. Repeat 1-4 for all 16 row-pairs
6. Repeat 1-5 for all 8 color bit depths
7. Repeat 1-6 continuously to display your frame for longer

Since only a single row-pair is on at a time, the microcontroller, or whatever is connected to the display, is responsible for continuously shifting in new pixels, row by row, bit depth by bit depth, to maintain a static image.

### PIO State Machines
The PIO (which stands for Programmable Input/Output) is a unique peripheral that features 8 state machines that can be used for many different things. In this case we are using 2 of them to clock in RGB pixels to the display

## Display Utils

## Pico Display

# A few other things
## platformio.ini
Make sure to set the optimization build flag to something other than 0 (e.x. `-O1` or `-O3`). If it is set to zero the display driver will run slow enough that the display will have visible flickers and/or artifacts.
