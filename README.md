# KermiteCore_Arduino

Kermite firmware library for Arduino and PlatformIO.

You can create a Kermite-compatible keyboard firmware with Arduino.

## Getting Started
 
Here the process instructed how to create a keyboard firmware in Arduino.

In this tutorial, a single-key keyboard firmware is created with the board based on RP2040.

## Prepare a board

Prepare a development board with RP2040.
For this tutorial, the boards listed below are supported.

- Raspberry Pi Pico
- Adafruit KB2040
- Seeed XIAO RP2040
- RP2040 Zero
- Tiny2040

If you don't have them, other RP2040 boards can also be used if you change the pin settings.

As an example, I set up a board like this with KB2040.

<!-- ![](https://i.gyazo.com/c4a33994214317d46f5bd8b0a2822a85.png) -->

![](https://i.gyazo.com/67cc005212fe9d83c00a4a4c7bfcd149.png)

A tact switch or key switch is connected to a pin. The port is detailed later.


## Install and launch Arduino 
First, install Arduino IDE if you don't have it.
You can download installers from here.
https://www.arduino.cc/en/software

Launch Arduino IDE.

## Setup Board package

RP2040 is not supported officially by Arduino.
A board package forked from arduino-pico (earlephilhower) is used here. It is modified to support USB RawHID feature and has some adjustment to make it easy to create a keyboard firmware.

### Add board manager URL

From File > Preferences in main menu, open the configuration panel.

![](https://i.gyazo.com/68b76da9df8147c45c2fcf279f6e4dec.png)

In `Additional board manager URLs` field, add the URL below.
```
https://github.com/kermite-org/arduino-pico/releases/download/global/package_rp2040_index.json
```

By this configuration, a custom board package is indexed and can be installed from package manager UI.

### Install board package

Open board manager tab from left side navigation.

<!-- ![](https://i.imgur.com/NOjQsAO.png) -->

![](https://i.gyazo.com/43a1f531f7c686b02cff66dcb94860da.png)

Search `rp2040` and find the package `Raspberry Pi Pico/RP2040 by Earle F.Philhower,III ` . Select the latest `kermite-fork-*` suffixed version in the list and install it.


## Install libraries

There are two libraries provided to create a keyboard firmware for Kermite.

The library `KermieCore_Arduino` is a firmware core library. It handles keyboard layer logic and has USB HID device implementation. Communication to the Kermite's web application is also handled in this library.

`keyboard_peripheral_modules` is an optional library to be used to make a keyboard firmware easily. A basic key matrix scanner class and onboard LED abstraction class are included.

![](https://i.gyazo.com/4e013dc76c4ac6153b288a1309194081.png)

Open library manager from sidebar.

Search `kermite` and install these libraries.


<!-- ![](https://i.gyazo.com/ced6762b7523faf55cc1f12b3e0bd44f.png) -->

![](https://i.gyazo.com/625c8fc58ff21a11c82955066813c430.png)

When you install `keyboard_peripheral_modules`, a popup asks you whether to install dependent modules. Select `INSTALL ALL` here.


## Running a basic example

Here we'are going to run a basic example provided in the library.

### Open the example

Open an example sketch from File > Examples > KermiteCore_Arduino > basic.

![](https://i.gyazo.com/cf65e8fdc2104f7be34c916f01e712f1.png)


### Select a target board
Select a target board from Tools > Board > Raspberry PI Pico/RP2040.
Most of the RP2040 is covered here, but if your board is not in the list, select `Generic RP2040` in the bottom of the options. This is a generic board definition and can be used for any boards.

![](https://i.gyazo.com/1325788532057c75f2ce5fdd0e03b810.png)

### Configure the code for your board

There is a code block to setup onboard LED and an input pin definition for the button.

#### Onboard LED

```cpp
//select an appropriate boardLED initializer for your board
//BoardLED boardLED(25); //pico
//BoardLED boardLED(18, 19, 20, true); //tiny2040
BoardLED_NeoPixel boardLED(17, 0x40); //kb2040
//BoardLED_NeoPixel boardLED(12, 0x40, 11); //xiao rp2040
//BoardLED_NeoPixel boardLED(16, 0x40); //rp2040-zero
```
Comment in a boardLED initializer according to your board.

BoardLED is a class wraps up to 3 LEDs on the board.
BoardLED_NeoPixel is wrapper module for onboard NeoPixel LED to provide the interface as if it is looked as 3 individual LEDs.

If you want define a custom boardLED instance, refer the constructor signature below.

```cpp
  BoardLED(int pin0, int pin1 = -1, int pin2 = -1, bool sink = false);
  BoardLED_NeoPixel(int pin, int brightness = 0xFF, int powerPin = -1);
```

In the code, the pin number is corresponds GP0-GP31 ports of RP2040. Not for the pins D0,D1,D2... labeled in some board.

#### Input pin

```cpp
  const int pinButton = 6; //set an input pin for your board
```
  Select a pin number which is connected to your input button. It has pull-upped internally and no extra pull-up needed.

### Upload firmware (first time)

If the board is used with a firmware not compatible to arduino, it is required to reset the board manually. Make the board bootloader mode and upload the sketch.

#### Enter bootloader mode

![](https://i.gyazo.com/bdaf62ed5a1a49fb0d61c0dc12f429b1.png)

Make the board entered into bootloader with the buttons on the board.
Press and release RESET button while holding BOOT button.

#### Select port

![](https://i.gyazo.com/1d037dd22fab1b9628374ef725b3f1c6.png)

Select UF2 Board in the board menu.

#### Upload

Press Upload button.

### Upload firmware (after second)

Once a firmware based on arduino has written to the board, A virtual COM port is exposed to provide software reset feature. Using this, you don't have to reset the board manually.

#### Select port

![](https://i.gyazo.com/b066b5752703be6b15ef8684d5c38788.png)

Select the com port in board menu for the board.

#### Upload

Press Upload button.

### Check the Behavior

Check the firmware works as expected.

Onboard LED shortly lights once per second to show the program is going well.

If you press the button, onboard LED lights in another color.
If a keymapping is written by webapp, letters are emitted.
