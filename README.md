# Saitek Throttle USB Retrofit

This project contains code for AVR MCUs to allow using the throttle which ships
with the Saitek Pro Flight Yoke System as a standalone USB device.
The code targets the AVR ATmega16U2 microcontroller which is found on the
Arduino Leonardo prototyping board.

# How To Use

I did not succeed in getting the code to run as part of an Arduino sketch.
Likely, this is due to the bootloader on the Arduino Leonardo, which interferes
with the USB peripheral. Thus, the code must be flashed onto the target by direct
serial programming, using a serial programmer such as the ATMEL-ICE or by using
a separate Arduino board as a programmer.

## Compiling & Uploading

### Using Microchip Studio

Simply create a new project. Select the ATmega16U2 as the target device, then add all files in the `src/` subdirectory of this repository to the project. Compile, then upload using a programmer of your choice.

### Using AVR-GCC

A makefile is provided. To compile, simply type

    make

To upload the binary to the device using avrdude, use

    make program

You may have to specify your programmer type in the makefile using the `PROGRAMMER` variable.

## Pin Mapping

The throttle is connected to the yoke by a 6-pin miniDIN connector.
The pinout is as follows:

| miniDIN Pin # | Wire Color | Function             | Driven by | ATmega16U2 pin | Arduino Leonardo Pin |
| ------------- | ---------- | -------------------- | --------- | -------------- | -------------------- |
| 1             | blue       | Clock                | Yoke      | PD1            | D2                   |
| 2             | brown      | "ready" (active low) | Throttle  | PD0            | D3                   |
| 3             | red        | +5V                  | Yoke      | -              | -                    |
| 4             | orange     | GND                  | -         | -              | -                    |
| 5             | yellow     | Data                 | Throttle  | PD2            | D0                   |
| 6             | green      | +5V                  | Yoke      | -              | -                    |

Additionally, two status LEDs can be connected on pins PD3 / D1 (green) and PD4 / D4 (red).
