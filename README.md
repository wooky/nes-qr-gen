# NES QR Generator Demo

A small NES demo that allows you to input text and have it generate a valid QR code. Currently it can generate QR codes up to Version 27, 125x125 codes, supporting up to 1465 characters.

## Get the ROM
Download the latest ROM from the [releases](https://github.com/wooky/nes-qr-demo/releases) page. You should be able to run it on any modern NES emulator, however this was only tested with the Mesen emulator.

## Usage Instructions
### Editor Screen
When starting up the ROM, you are greeted with a primitive text editor. Here you can type in whatever text you want using the Family BASIC Keyboard.

You can set some configurations to your liking using the function keys:
* F1 - ECL (error correction level) - the higher the ECL, the more resilient it is against damage and corruption, but correspondingly the less characters you can use.
* F2 - mask - different masks produce better resiliency based on the encoded data. If you pick mask "A", it will pick the best mask, but be warned, this is excruciatingly slow - about _17 times slower_ - as it will generate a code for every mask!
* F3 - boost ECL - will attempt to upgrade the ECL without increasing the QR code version.

Once you're ready to generate the QR code, press F8. This will move you to the QR Screen.

### QR Screen
When the QR code is generating, you will see a black screen. _Be patient_, code generation takes a long time, and the more characters you have, the longer it'll take. Once generation is complete, it will be rendered to the screen and you can scan it.

If a red screen appears, that means that code generation has failed. The most likely reason for that is that the input text size is greater than the maximum supported text size.

Once you're done, press any key to return to the Editor Screen. Note that your input text will be discarded.

## Compiling
The following prerequisites are required:
* CMake 3.18+
* A supported build automation tool (e.g. GNU make)
* Python 3
* cc65, preferably added to PATH

Checkout the repo and run the following commands from the repo folder (make is assumed here):
```bash
cmake -B build
make -C build
```
The generated NES file will be built as build/qrdemo.nes.

## Technical Blurbs
This demo uses the [QR-Code-generator library](https://github.com/nayuki/QR-Code-generator). Parts of the code were changed to make it compile with cc65 and to optimize performance somewhat. Reed-Solomon multiplication was particularly slow and was reimplemented into a table of constants, spanning a whopping 4 ROM banks. As such, this ROM uses the MMC1 mapper.

## License
Licensed under the MIT license.
