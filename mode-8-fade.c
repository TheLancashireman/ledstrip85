/* mode-8-fade.c - cycle through all binary combinations of LEDs with gradual change
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

/* mode_8_fade() - cycle through all eight combinations of LEDs, with a gradual change.
 *
 * The cycle is a Gray code cycle - only one LED changes state at any time
*/
void mode_8_fade(void)
{
	for (;;)
	{
		vdelay(1000);
		fade_up(LED_B);
		vdelay(1000);
		fade_up(LED_R);
		vdelay(1000);
		fade_down(LED_B);
		vdelay(1000);
		fade_up(LED_G);
		vdelay(1000);
		fade_up(LED_B);
		vdelay(1000);
		fade_down(LED_R);
		vdelay(1000);
		fade_down(LED_B);
		vdelay(1000);
		fade_down(LED_G);
	}
}
