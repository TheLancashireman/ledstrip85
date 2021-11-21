/* mode-3-fade.c - cycle through the LEDs with gradual fading.
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

/* mode_3_fade() - cycle through 3 states where one LED is on, with gradual change.
 *
 * The LED to turn off is ramped down while the LED to turn on is ramped up.
*/
void mode_3_fade(void)
{
	fade_up(LED_B);	 /* Initial condition */
	for (;;)
	{
		vdelay(1000);
		fade_up_down(LED_R, LED_B);
		vdelay(1000);
		fade_up_down(LED_G, LED_R);
		vdelay(1000);
		fade_up_down(LED_B, LED_G);
	}
}
