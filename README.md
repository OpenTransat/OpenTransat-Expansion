# OpenTransat-Expansion

The [Navigator](https://github.com/OpenTransat/OpenTransat-Navigator) is mounted on top of Expansion Board that will provide additional functions more specific to the boat. This modular design allows to make quick modifications on the Expansion Board while keeping the more complex Navigation Board unchanged.

![Expansion PCB](https://github.com/OpenTransat/OpenTransat-Expansion/blob/master/images/controller1.jpg "Expansion PCB")

![Expansion PCB](https://github.com/OpenTransat/OpenTransat-Expansion/blob/master/images/controller2.jpg "Expansion PCB")

## Features

* Watchdog timer that monitors the “heartbeat” of the main CPU. If the Navigator doesn't respond for a minute or it reports any kind of failure, the watchdog will cut power for a few seconds.

* MOSFETs to turn on particular hardware only when needed.

## Boat Hardware Scheme

![Boat Hardware Scheme](https://github.com/OpenTransat/OpenTransat-Navigator/blob/master/images/hw_scheme1.gif "Boat Hardware Scheme")

![Boat Hardware Scheme](https://github.com/OpenTransat/OpenTransat-Navigator/blob/master/images/hw_scheme2.gif "Boat Hardware Scheme")
