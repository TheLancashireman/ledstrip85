# ledstrip85

LED controller using an attiny85 microcontroller.

The controller can be used with an RGB LED-strip or with three independent strings of LEDs.

There are several modes - constant, fading, pseudo-random, ...

Infra-red remote control input.

The timing in this sketch uses the tinylib timing on Timer0.

The project uses the libraries from tiny-bare-metal.
No arduino libraries were harmed during the construction of this application. :-)

The project was derived from the original ledstrip sketch for an Arduino nano.

The modes of operation are selected by the numerical keys on the remote-control handset:

* 1 - All LEDs on.
* 2 - Illuminates each LED in turn.
* 3 - Cycles through all 8 combinations of on/off. One LED changes on each tick.
* 4 - Cycles through all 8 combinations of on/off. One LED fades up or down during the change.
* 5 - Cycles through the LEDs in turn. The change is gradual; at any time, one LED fades up while another fades down.
* 6 - The intensity of each LED is controlled individually using six keys on the remote control.
* 7 - Each LED turns on and off in a pseudo-random sequence. Each LED has its own sequence.
* 8 - Each cycle, a randomly-chosen LED is faded up or down depending on previous state. Or no change.
* 9 - Twinkle - at random intervals, a random LED is set to a random intensity
* 0 - "Blue" LED on, others off. Obsolescent.

The on/off or standby key of the remote-control handset turns all the LEDs off.

In the cycling and random modes, the speed of change is controlled by a pair of keys on the handset.

In mode 6, the the intensity of each LED is controlled independently, each with a pair of keys on the handset.

Look in rc.h for a mapping of the eight up/down keys.

## License, disclaimer etc.

Copyright David Haworth

This file is part of ledstrip85.

ledstrip85 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ledstrip85 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ledstrip85.  If not, see <http://www.gnu.org/licenses/>.

