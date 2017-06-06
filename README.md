Planned features:
 * brake light: light up when braking
 * one button on/off: no need to press often 
 * advanced blinking modes: not only blinking, support also fading 
 * auto off: turn off light after certain time or if no movement for some defined period

Currently for motion a tilt switch is used. This won't work very well. You
could also connect it directly to your brakes. In future an acceleration sensor
would be fun.


How to upload firmware
======================

How to upload new firmware:

(Details see http://highlowtech.org/?p=1695)

Tested with Arduino 1.8.1.

In Arduino preferences add repository:
https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json

In Boardmanager add "attiny by David A Mellis"

Then select:

Board: ATiny
Processor: ATiny 85
Clock: 1Mhz internal

And:
Arduino as ISP

Use an Arduino Uno or Mega (Leonardo does not easily work!) and upload the ISP
firmware to the Arduino (Examples --> Arduino ISP). Then connect a 10uF
capacitor between Arduion reset and GND and the following pins of the attiny85
to the Arduino:

    pin name:    not-mega:         mega(1280 and 2560)
    slave reset: 10:               53
    MOSI:        11:               51
    MISO:        12:               50
    SCK:         13:               52

...to the bike light:

    5 VCC
    6 SCK
    1 Reset
    7 MISO
    8 MOSI
    4 GND
    
    ^--- the pinched off here end

These pins are connect to attiny85:

             ___
    RST  1 -|°  |- 5  VCC
    AIO  2 -|   |- 6  PIN2,  button on/off
    AIO  3 -|   |- 7  PIN1,  brake button
    GND  4 -|___|- 8  PIN0,  LED output


Circuits
========

Reverse engineered circuits of popular USB bike lights:

3 LED bike light
----------------

[Find it e.g. here](https://de.aliexpress.com/item/New-Arrival-Cycling-Bicycle-Light-Bike-Accessories-3-LED-Head-Front-Rear-Tail-light-USB-Rechargeable/32610725037.html?spm=2114.010208.3.58.IQsTXI&ws_ab_test=searchweb0_0,searchweb201602_5_10152_5010012_10065_10151_10068_10136_10137_10060_10138_10155_10062_437_10154_10056_10055_10054_10059_303_100031_10099_10103_10102_10096_10052_10053_10142_10107_10050_10051_5030013_10084_10083_10080_10082_10081_10177_10110_519_10111_10112_10113_10114_10182_10078_10079_10073_10123_10189_142,searchweb201603_1,ppcSwitch_4&btsid=e8e9287a-2dda-44a6-aff1-5629da231ffb&algo_expid=25c27754-bb50-4d8b-b381-e85c8292a612-7&algo_pvid=25c27754-bb50-4d8b-b381-e85c8292a612)

              ___
            -|°  |-  GND 
    Button  -|   |- 
            -|   |-  Signal PWM
    VCC     -|___|-  ? also signal?


Signal pin is a sink, pull to GND to turn on LED!


1 LED bike light small 
----------------------

[Find it e.g. here](https://de.aliexpress.com/item/Bicycle-light-CYCLE-ZONE-Bicycle-LED-USB-Charging-Rear-Tail-Warning-Safety-Light-Lamp-Red-Light/32809807755.html?spm=2114.010208.3.137.JN9Y3C&ws_ab_test=searchweb0_0,searchweb201602_5_10152_5010012_10065_10151_10068_10136_10137_10060_10138_10155_10062_437_10154_10056_10055_10054_10059_303_100031_10099_10103_10102_10096_10052_10053_10142_10107_10050_10051_5030013_10084_10083_10080_10082_10081_10177_10110_519_10111_10112_10113_10114_10182_10078_10079_10073_10123_10189_142,searchweb201603_1,ppcSwitch_4&btsid=6f5868fd-9615-4f83-8d04-5e0f5045fd3d&algo_expid=360df20c-7544-4744-a9e9-5fab181c8ad6-17&algo_pvid=360df20c-7544-4744-a9e9-5fab181c8ad6)

          ___
    VCC?    -|°  |-  GND 
    Button  -|   |-  high?
    high?   -|   |-  Signal PWM
    VCC?    -|___|-  ? also signal?



Size constraints
================

Marcel's bike frame: 37.7mm diameter, 14mm - 24mm depth
  

Arduino sleep
=============

Some links:

* http://www.engblaze.com/hush-little-microprocessor-avr-and-arduino-sleep-mode-basics/
* https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
* http://forum.arduino.cc/index.php?topic=8640.0


vUSB
====

"V-USB is a software-only implementation of a low-speed USB device for Atmel’s AVR® microcontrollers, making it possible to build USB hardware with almost any AVR® microcontroller, not requiring any additional chip."

* https://www.obdev.at/products/vusb/index.html
* https://github.com/micronucleus/micronucleus/blob/master/Devices_with_Micronucleus.md
* https://metalab.at/wiki/Usblinky
* https://cpldcpu.com/2014/04/25/the-nanite-85/
* http://www.electroschematics.com/12102/learn-to-use-attiny85-usb-mini-development-board/
