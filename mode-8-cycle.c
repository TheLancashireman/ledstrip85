/* mode-8-cycle.c - cycle through all binary combinations of LEDs
 *
 * (c) David Haworth
 *
 *  This file is part of ledstrip85.
 *
 *  ledstrip85 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ledstrip85 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ledstrip85.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ledstrip85.h"

/* mode_8_cycle() - cycle through all eight combinations of LEDs.
 *
 * The cycle is a Gray code cycle - only one LED changes state at any time
*/
void mode_8_cycle(void)
{
	for (;;)
	{
		vdelay(1000);
		pin_set(LED_B, HIGH);
		vdelay(1000);
		pin_set(LED_R, HIGH);
		vdelay(1000);
		pin_set(LED_B, LOW);
		vdelay(1000);
		pin_set(LED_G, HIGH);
		vdelay(1000);
		pin_set(LED_B, HIGH);
		vdelay(1000);
		pin_set(LED_R, LOW);
		vdelay(1000);
		pin_set(LED_B, LOW);
		vdelay(1000);
		pin_set(LED_G, LOW);
	}
}
