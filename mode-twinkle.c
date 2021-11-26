/* mode-twinkle.c - all LEDs with one of four randomly-chosen intensities.
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

/* mode_twinkle() - control the intensity of each LED individually
 *
 * Main loop 100 us performs PWM.
 *
 * Every xx loops, choose one LED at random and set its intensity to one
 * of four values, also chosen randomly.
 *
 * xx is determined by speed and random factor:
 *		speed = 0, yy = 10000
 *		speed = 19, yy = 500
 * 		==> yy = 10000/(speed+1)
 *		rnd = 0..7
 *		xx = 2*yy/(rnd+1)  
*/

#if VARIANT == RGB
#define LVL0	100
#define LVL1	66
#define LVL2	33
#define LVL3	0
#else
#define LVL0	100
#define LVL1	75
#define LVL2	50
#define LVL3	25
#endif

void mode_twinkle(void)
{
	s16_t i = 10000/(speed+1);
	
	for (;;)
	{
		for ( s8_t j = 1; j < 100; j++ )
		{
			i--;
			if ( i <= 0 )
			{
				/* Crank the LFSR three times to get three new bits
				*/
				(void)lfsr_r();
				(void)lfsr_r();
				u8_t rnd = lfsr_r() & 0x07;

				i = 20000/((speed+1)*(rnd+1));

				/* Crank the LFSR twice to get two new bits
				*/
				(void)lfsr_b();
				u8_t led = lfsr_b() & 0x03;
				if ( led < 3 )
				{
					/* Crank the LFSR twice to get two new bits
					*/
					(void)lfsr_g();
					u8_t lvl = lfsr_g() & 0x03;
					switch ( lvl )
					{
					case 0:		level[led] = LVL0;	break;
					case 1:		level[led] = LVL1;	break;
					case 2:		level[led] = LVL2;	break;
					default:	level[led] = LVL3;	break;
					}
				}
			}
			pin_set(LED_B, ( level[0] >= j ) ? HIGH : LOW);
			pin_set(LED_R, ( level[1] >= j ) ? HIGH : LOW);
			pin_set(LED_G, ( level[2] >= j ) ? HIGH : LOW);
			tick_delay(us_to_ticks(100));
		}
	}
}
