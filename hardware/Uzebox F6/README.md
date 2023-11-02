<img src="http://uzebox.org/belogic.com/uzebox/images/new_banner3.jpg" alt="Uzebox logo" />

As a request of forum member Danboid I created a new version of the original Uzebox PCB. He wants a version which supports RGB SCART connection without the need for the expensive AD725. He could use the Uzebox SCART, but that doesn't fit into the original case. This version does.

## What is the same
* General schematic and function
* mechanical dimensions. All buttons, connectors, holes, etc. are in the same place, so you can use the original case.


## New features
* SCART RGB support. Because space is too limited for the giant SCART connector, we use an Segadrive 2 Mini-DIN 9 pin connector
* The AD725 chip and it's surrounding parts are optional now. If you don't populate them RGB is working, but SVideo and CVBS is without function.
* Optional footprint for an USB to serial module on the bottom side below the SD card socket. You can use this as a powersupply connector to power your Uzebox from a USB wall plug. Also you can use it for debug messages, it is connected to the second UART of the AVR CPU. This leads to some small incompatibilities, so you have to activate it by a couple of solder jumpers on the bottom side. See below instructions.
* The ESP12 Wifi Chip is connected to the CPU's SPI and reset lines. With this is should be possible to flash the AVR CPU via Wifi. But you need a special firmware for the ESP for that, which isn't written yet.
* You can use an WS2812 "Neopixel" LED instead of the normal LED. Again, there is no software for it yet. It uses the same pin as the original LED. A connector for adding more WS2812 is also there, so maybe you can create a colorful Uzebox Sign in the top of your case.
* I overworked all the traces, so they are on raster and layouted them a bit cleaner - in my opinion.

## Instructions for using SCART
Use an XRGB to SCART cable. Put a jumper into the left position to route sync signal to the connector. If you populated the AD725, you can alternatively a jumper into the right position, which routes CVBS to the sync pin, which should also work. You can the use TVs which don't support RGB on the SCART input.

I routed the SVideo signals to unused pins 5 (LUMA) and 6 (CHROMA), so with an adapter cable you can still use SVideo.
See [this page](https://members.optusnet.com.au/eviltim/gamescart/gamescart.htm) for a compatible cable.

![blah](mega2.png)

<img src="mega2.png" alt="Mini Din 9 pinout" />
 
## Instructions for using the ESP's SPI connection
The ESP's SPI pins GPIO12, 13 and 14 are connected to the SPI lines of the AVR CPU. So you have a very fast communication line between the two CPUs. I also connect GPIO16 to Reset of the AVR. It should be possible to flash the AVR from the ESP with this. Should this connection create issues with some software, I placed four solder jumper on the bottom under the ESP. These are connected by default with a small trace. Cut these to disconnect the ESP.

## Instructions for using the USB to serial module
WARNING: You have to leave out the 5V regulator if you want to use USB as power source!!!

The module have to be soldered to the bottom side of the PCB under the SD card socket. Because we cannot use holes there as the would conflict with the SD card, you have to solder the module directly onto the pads, or solder some angled pin hedaers onto the pad and put the module onto these. This method is perferred because it gives you a bit of distance between the SD card and the USB socket.

There are two solder jumpers near the module. They are open by default. So the module is connected to nothing, only to 5V for power supply.
If you close these jumpers the serial lines of the module will be connected to the second serial port of the AVR CPU. Because these are use by the standard Uzebox' power button and ESP_RST pin there are two solder jumper which may connect these to PA5 and PA6 instead. This would need a software modification for software that uses the power button or the ESP_RST pin.

These jumpers are connected by a small wire in compatibility position. Without cutting these lines everything is like the original Uzebox

## Instructions for WS2812 LED
If you want a colorfull Neopixel LED instead of the normal one, just solder a WS2812 mini LED instead of the normal one. You can also skip the LEDs resistor then. There is a 3-pin connector at the right side of the PCB to connect more LEDs. But don't overload the power supply.
Keep in mind that controlling such LEDs require a tight timing. So it might be tricky to controll such a LED during video output.

## To find out more, please check out the project's sites:
* [Getting started](https://uzebox.org/wiki/Getting_Started_on_the_Uzebox): How to install the toolchains, IDEs and build the codebase. Then move on to tutorials and the rest of the documentation.  
* [Main website](https://uzebox.org): The main hub with news, links, downloads and more.
* [Wiki](https://uzebox.org/wiki): All the project's documentation.
* [Forums](https://uzebox.org/forums): Share your new game and discuss everything Uzebox.
