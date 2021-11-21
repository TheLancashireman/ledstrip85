/* ledstrip85.c - a flashing lights project for attiny85
 *
 * Based on the original ledstrip project for arduino nano.
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

/*
 *                             |--v--|
 *                    PB5/Rst -|1   8|- Vcc
 *             (blue led) PB3 -|2   7|- PB2 (green led)
 *   (serial out; optional) 4 -|3   6|- PB1 (red led)
 *                        Gnd -|4   5|- PB0/PCINT0 (infra-red)
 *                             |-----|
*/

#include "ledstrip85.h"
#include "tinylib.h"
#include "tinyio.h"
#include <avr/pgmspace.h>
#include "rc.h"
#include <setjmp.h>

char mode =	1;
char speed = 10;					/* Range 0..19, controlled by INDEX+ and INDEX- buttons */
char level[3] = { 5, 5, 5 };		/* Range 0..100, controlled by ??? */

u16_t lastpress;
u32_t lastpresstime;

/* Shift registers for pseudo-random modes
*/
unsigned long sr_r = 1;
unsigned long sr_g = 8;
unsigned long sr_b = 64;

jmp_buf jb;

int main(void)
{
	timing_init();
	pin_mode(LED_R, OUTPUT);
	pin_mode(LED_G, OUTPUT);
	pin_mode(LED_B, OUTPUT);
	all_on();
	async_init();
	ir_init();
	enable();
#if DBG
	printf(PSTR("Hello, world!\n"));
#endif

	for (;;)
	{
		setjmp(jb);
		all_off();

#if DBG
		printf(PSTR("Mode %d\n"), mode);
#endif

		switch (mode)
		{
		case 0:		mode_all_off();			break;
		case 1:		mode_all_on();			break;
		case 2:		mode_3_cycle();			break;
		case 3:		mode_8_cycle();			break;
		case 4:		mode_8_fade();			break;
		case 5:		mode_3_fade();			break;
		case 6:		mode_variable();		break;
		case 7:		mode_flicker();			break;
		case 8:		mode_random_1_fade();	break;
		case 9:		mode_green();			break;
		case 10:	mode_blue();			break;
		default:	mode_all_on();			break;		/* Default to ON */
		}
	}

	return 0;
}

/* all_off() - turn all LEDs off
*/
void all_off(void)
{
	pin_set(LED_R, LOW);
	pin_set(LED_G, LOW);
	pin_set(LED_B, LOW);
}

/* all_on() - turn all LEDs on
*/
void all_on(void)
{
	pin_set(LED_R, HIGH);
	pin_set(LED_G, HIGH);
	pin_set(LED_B, HIGH);
}

/* fade_up() - gradually increase the intensity of a single LED
 *
 * The outer loop ramps up the intensity. The inner loop creates the PWM with a period of 10 ms.
*/
void fade_up(int pin)
{
	for (unsigned i = us_to_ticks(100); i < us_to_ticks(10000); i += us_to_ticks(100) )
	{
		for ( int j = 0; j < (20-speed); j++ )
		{
			pin_set(pin, HIGH);
			tick_delay(i);
			pin_set(pin, LOW);
			tick_delay(us_to_ticks(10000) - i);
		}
	}
	pin_set(pin, HIGH);
}

/* fade_down() - gradually decrease the intensity of a single LED
 *
 * The outer loop ramps down the intensity. The inner loop creates the PWM with a period of 10 ms.
*/
void fade_down(int pin)
{
	for (unsigned i = us_to_ticks(100); i < us_to_ticks(10000); i += us_to_ticks(100) )
	{
		for ( int j = 0; j < (20-speed); j++ )
		{
			pin_set(pin, LOW);
			delay_ticks(i);
			pin_set(pin, HIGH);
			delay_ticks(us_to_ticks(10000) - i);
		}
	}
	pin_set(pin, LOW);
}

/* fade_up_down() - gradually change the intensity of two LEDs - one up, one down.
 *
 * The outer loop controls the intensity. The inner loop creates the PWM with a period of 10 ms with
 * the two LEDs working in opposite directions.
*/
void fade_up_down(int up_pin, int down_pin)
{
	for (unsigned i = us_to_ticks(100); i < us_to_ticks(10000); i += us_to_ticks(100) )
	{
		for ( int j = 0; j < (20-speed); j++ )
		{
			pin_set(up_pin, HIGH);
			pin_set(down_pin, LOW);
			tick_delay(i);
			pin_set(up_pin, LOW);
			pin_set(down_pin, HIGH);
			tick_delay(us_to_ticks(10000) - i);
		}
	}
	pin_set(up_pin, HIGH);
	pin_set(down_pin, LOW);
}

/* tick_delay() - delay by the specified number of ticks
 *
 * While the delay is in progress, the IR receiver is monitored.
*/
void tick_delay(unsigned long tix)
{
	u32_t start = read_time_32();

	do
	{
		mode_check();
	} while ( (read_time_32() - start) < tix );	/* Overflow/underflow doesn't matter */
}

/* mode_check() - check for a mode change
 *
 * If there has been a mode change, longjmp back to the main loop.
 * Speed and intensity changes just modify the control variables.
*/
void mode_check(void)
{
	ir_key_t ir_data;
	char newmode = mode;

	if ( ir_receive(&ir_data) )
	{
#if DBG
		u32_t ir_sr = (u32_t)ir.shiftreg;
		printf(PSTR("sr=%08lx\n"), ir_sr);
		if ( sizeof(ir_data) == 1 )
			printf(PSTR("k=%02x\n"), ir_data);
		else if ( sizeof(ir_data) == 2 )
			printf(PSTR("k=%04x\n"), ir_data);
		else
			printf(PSTR("k=%08lx\n"), ir_data);
#endif

		if ( ir_data == lastpress )
		{
			/* Ignore all repeats until the repeat time has elapsed.
			*/
			if ( (read_time_32() - lastpresstime) < REPEAT_DELAY )
			{
				ir_data = IRBTN_NONE;
			}
		}
		else
		{
			/* Restart the repeat timing.
			*/
			lastpresstime = read_time_32();
			lastpress = ir_data;
		}

		if ( ir_data != 0xffff )
		{
			printf(PSTR("+%02x\n"), ir_data & 0xff);
		}

		switch ( ir_data )
		{
		case BTN_OFF:		newmode = 0;	break;
		case BTN_M1:		newmode = 1;	break;
		case BTN_M2:		newmode = 2;	break;
		case BTN_M3:		newmode = 3;	break;
		case BTN_M4:		newmode = 4;	break;
		case BTN_M5:		newmode = 5;	break;
		case BTN_M6:		newmode = 6;	break;
		case BTN_M7:		newmode = 7;	break;
		case BTN_M8:		newmode = 8;	break;
		case BTN_M9:		newmode = 9;	break;
		case BTN_M10:		newmode = 10;	break;

		case BTN_SLOWER:	if ( speed > 0 )	speed--;	break;
		case BTN_FASTER:	if ( speed < 19 )	speed++;	break;

		case BTN_L1u:		if ( level[0] < 100 )	level[0]++;		break;
		case BTN_L1d:		if ( level[0] > 0 )		level[0]--;		break;
		case BTN_L2u:		if ( level[1] < 100 )	level[1]++;		break;
		case BTN_L2d:		if ( level[1] > 0 )		level[1]--;		break;
		case BTN_L3u:		if ( level[2] < 100 )	level[2]++;		break;
		case BTN_L3d:		if ( level[2] > 0 )		level[2]--;		break;

		default:						break;	/* No change */
		}
	}
	else
	if ( lastpress != 0xffff )
	{
		/* If no key received for REPEAT_DELAY time, cancel the repeat mechanism.
		*/
		if ( (read_time_32() - lastpresstime) >= REPEAT_DELAY )
		{
			lastpress = 0xffff;
		}
	}

	if ( mode != newmode )
	{
		mode = newmode;
		longjmp(jb, 42);
#if DBG
		printf(PSTR("Oops! longjmp() returned\n"));
#endif
	}
}
