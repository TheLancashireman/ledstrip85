/* mode-variable.c - all LEDs with variable intensity, independently controlled
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

/* mode_variable() - control the intensity of each LED individually
 *
 * Algorithm:
 *	- In the loop (every 100 us):
 *		LEDs whose level is >= j are on, others are off
*/
void mode_variable(void)
{
	for (;;)
	{
		for ( int j = 1; j < 100; j++ )
		{
			pin_set(LED_B, ( level[0] >= j ) ? HIGH : LOW);
			pin_set(LED_R, ( level[1] >= j ) ? HIGH : LOW);
			pin_set(LED_G, ( level[2] >= j ) ? HIGH : LOW);
			tick_delay(us_to_ticks(100));
		}
	}
}
