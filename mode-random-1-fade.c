/* mode-random-1-fade.c - fade one randomly-chosen LED on or off each cycle
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

/* mode_random_1_fade() - fade a randomly-chosen LED each time
*/
void mode_random_1_fade(void)
{
	char r_state = 0, g_state = 0, b_state = 0;
	unsigned long sr;
	for (;;)
	{
		/* Crank the LFSR twice to get two new bits
		*/
		(void)lfsr_g();
		sr = lfsr_g();

		switch ( sr & 0x03 )
		{
		case 1:						/* Change LED_R */
			if ( r_state )
			{
				fade_down(LED_R);
				r_state = 0;
			}
			else
			{
				fade_up(LED_R);
				r_state = 1;
			}
			break;

		case 2:						/* Change LED_G */
			if ( g_state )
			{
				fade_down(LED_G);
				g_state = 0;
			}
			else
			{
				fade_up(LED_G);
				g_state = 1;
			}
			break;

		case 3:						/* Change LED_B */
			if ( b_state )
			{
				fade_down(LED_B);
				b_state = 0;
			}
			else
			{
				fade_up(LED_B);
				b_state = 1;
			}
			break;

		default:					/* No change */
			vdelay(1000);
			break;
		}
	}
}
