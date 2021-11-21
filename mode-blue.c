/* mode-blue.c - "blue" LED on
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

/* mode_blue() - turn the "blue" LED on, then wait for a mode change
 *
 * This mode is fairly pointless because the same can be achieved with mode_variable().
 * It is retained for posterity and as a placeholder until a better mode is developed.
*/
void mode_blue(void)
{
	pin_set(LED_R, LOW);
	pin_set(LED_G, LOW);
	pin_set(LED_B, HIGH);
	for (;;)
	{
		mode_check();
	}
}
