# About

This is a simple LED strip project controlled by Wifi. Mostly an experiment to test a Raspberry Pi Pico with an ESP01 for
controlling the processor with network commands.

# Status

Project runs, but still a work in progress (esp lacking visual effects)

# Hardware

- A Raspberry Pi Pico board
- An ESP01 board connected to serial 2 on the RPi
- LED strip of 50 LEDs with WS2812A controllers, with data connected to GP15 pin.
- LED strip is powered at 5V, and onboard regulator drops to 3.3V to power RP2040 chip. LED and chip share common ground.
- LED strip data pin is driven at 3.3v despite being supplied at 5V. This works because of the low "on" threshold of the
  WS2812.

# Software

- Program developed using PlatformIO, using Arduino framework
- Adafruit_NeoPixel library for driving LED strip
- Custom command and response processing with the ESP01 using AT commands. This connects to the Wifi, and then receives HTTP
  commands from a browser on the local network.

# HTTP handling

`athttp.cpp` provides a custom HTTP parser that understands a very limited subset of HTTP, and uses the ESP01's AT commands
in sending and recieving.

The HTTP parser only supports URL segments, but not query fields or fragments at this stage. However, it also does not
allocate memory, and will truncate URL segments to a configurable fixed size so it can be run on an Arduino with very limited
RAM (although the RPI has plenty)

I could not get the Wifi / async server libraries to work on the Pico, which is why I wrote a simple parser directly. Those
libraries are preferable if you can get them to work.

# Commands it understands

This understands GET HTTP requests of the forms:
 *   `<ip-address>/<command>`
 *`<ip-address>/<command>/<param>`

Current commands are:
 *  `ambient` - cycles very slowly through a blue / green / red cycle
 *  `robot` - random flashing, mostly bright colours
 *  `cylon` - from battlestar galactica. but doesn't currently work properly.
 *  `colour/<colour>` (NZ english) - sets to a solid colour (colour parsing not implemented.)

# To do

- complete colour and cylong effects, and add some more effects
- add query field support to HTTP parser
- preprocessor option to allow more full parsing using dynamic allocation for boards where memory constraint is not an issue.
- move HTTP parser to it's own library