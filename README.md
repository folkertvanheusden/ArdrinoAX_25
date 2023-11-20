ArdrinoAX.25
============


What it is
----------
This is a packet/APRS router for the ESP32 (lilygo t-beam series).
It should be relatively easy to port to other systems.


What it can
-----------
It can route from:

* serial TNC (configure your pc serial port at 115200 bps)
* bluetooth
* AX.25 over UDP over WiFi (set ipd-udp-port and ipd-udp-target-address in the configuration json file to the port/address of the other end, WiFi can be configured via a browser by connecting your laptop to the accesspoint called "ArdrinoAX.25" and then browse to http://192.168.4.1/)
* send AX.25 beacons of a configurable text over LoRa
* send APRS packets with GPS location and a configurable text (interval can be configured to be depending on the move-speed) over LoRa

It can run multiple serial TNCs or multiple AX.25-over-UDP instances but for that you need to adjust main.ino (the 'setup' procedure).


How to install
--------------
Using platformio:

* pio run -t uploadfs
* pio run -t upload

You may first want to copy "example-configuration.json" to the "data/" folder and adjusting its contents. Especially the callsign is important.


(C) 2023 by Folkert van Heusden
Released under the AGPL license.
