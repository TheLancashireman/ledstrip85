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

#include "tinylib.h"
#include "tinyio.h"
#include <avr/pgmspace.h>
#include "rc.h"
#include <setjmp.h>

/* Debugging info
*/
#define DBG	1

/* Build variants
*/
#define RGB		1		/* Strip of RGB leds */
#define MONO	2		/* Three LED light chains, monochrome */

/* Remote handset variants
*/
#define DAEWOO	1
#define EUROPA	2

/* Pin numbers
*/
#define LED_R	PB1
#define LED_G	PB2
#define LED_B	PB3
#define IR_PIN	PB0			/* Not selectable; Must be PCINT0 */
/*				PB4 is the serial output pin, configured in the makefile */

/* Hold-off time before key repeat starts (milliseconds)
*/
#define REPEAT_DELAY	ms_to_ticks(250)

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

void all_off(void);
void all_on(void);

void mode_0(void);	/* All LEDs off */
void mode_1(void);	/* All LEDs on (white light, or as near as we can get) */
void mode_2(void);	/* Three-colour cycle (exactly one colour on at any time) */
void mode_3(void);	/* Eight-colour cycle (all possible combinations of one, two and three colours) */
void mode_4(void);	/* Eight-colour cycle (like mode_3) but with a gradual change */
void mode_5(void);	/* Three-colour cycle with a gradual change; while one colour decreases another increases. */
void mode_6(void);	/* Fully-adjustable single colour */
void mode_7(void);	/* Red */
void mode_8(void);	/* Green */
void mode_9(void);	/* Blue */
void mode_a(void);	/* Pseudo-random on/off */

void fade_up(int pin);
void fade_down(int pin);
void fade_up_down(int up_pin, int down_pin);
void tick_delay(unsigned long tix);

void mode_check(void);

extern unsigned long lfsr(unsigned long v, unsigned long tap, unsigned long fb);

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
		case 0:		mode_0();	break;
		case 1:		mode_1();	break;
		case 2:		mode_2();	break;
		case 3:		mode_3();	break;
		case 4:		mode_4();	break;
		case 5:		mode_5();	break;
		case 6:		mode_6();	break;
		case 7:		mode_7();	break;
		case 8:		mode_8();	break;
		case 9:		mode_9();	break;
		case 10:	mode_a();	break;
		default:	mode_1();	break;		/* Default to ON */
		}
	}

	return 0;
}

/* vdelay() - a variable delay - the delay depends on the current speed
 *
 * Higher speed ==> shorter delay
 *
 * The nominal delay for speed 10 is in milliseconds.
 *
 * The arduino delay() function is not used because of the need to monitor the remote control receiver.
*/
static inline void vdelay(unsigned long ms)
{
	tick_delay(us_to_ticks(ms*100*(20-speed)));
}

/* all_off() - turn all colours off
*/
void all_off(void)
{
	pin_set(LED_R, LOW);
	pin_set(LED_G, LOW);
	pin_set(LED_B, LOW);
}

/* all_on() - turn all colours on
*/
void all_on(void)
{
	pin_set(LED_R, HIGH);
	pin_set(LED_G, HIGH);
	pin_set(LED_B, HIGH);
}

/* mode_0() - turn all colours off, then wait for a mode change
*/
void mode_0(void)
{
	all_off();
	for (;;)
	{
		mode_check();
	}
}

/* mode_1() - turn all colours on, then wait for a mode change
*/
void mode_1(void)
{
	all_on();
	for (;;)
	{
		mode_check();
	}
}

/* mode_2() - switch the individual colours on and off in a cycle
*/
void mode_2(void)
{
	for (;;)
	{
		pin_set(LED_R, HIGH);
		vdelay(1000);
		pin_set(LED_R, LOW);
		pin_set(LED_G, HIGH);
		vdelay(1000);
		pin_set(LED_G, LOW);
		pin_set(LED_B, HIGH);
		vdelay(1000);
		pin_set(LED_B, LOW);
	}
}

/* mode_3() - cycle through all eight on/off states (Gray code)
*/
void mode_3(void)
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

/* mode_4() - cycle through all eight on/off states with gradual change (Gray code)
*/
void mode_4(void)
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

/* mode_5() - ramp colour intensity up and down simultaneously.
*/
void mode_5(void)
{
	fade_up(LED_B);		/* Initial condition */
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

/* mode_6() - variable intensity with three independent colour intensities.
 *
 * Algorithm:
 *		- In the loop (every 100 us):
 *			LEDs whose level is >= j are on, others are off
*/
void mode_6(void)
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

/* mode_7() - red
*/
void mode_7(void)
{
	pin_set(LED_R, HIGH);
	pin_set(LED_G, LOW);
	pin_set(LED_B, LOW);
	for (;;)
	{
		mode_check();
	}
}

/* mode_8() - green
*/
void mode_8(void)
{
	pin_set(LED_R, LOW);
	pin_set(LED_G, HIGH);
	pin_set(LED_B, LOW);
	for (;;)
	{
		mode_check();
	}
}

/* mode_9() - blue
*/
void mode_9(void)
{
	pin_set(LED_R, LOW);
	pin_set(LED_G, LOW);
	pin_set(LED_B, HIGH);
	for (;;)
	{
		mode_check();
	}
}

/* mode_a() - pseudo random sequence on each LED
*/
void mode_a(void)
{
	for (;;)
	{
		sr_r = lfsr(sr_r, 0x8, 0x20000);	
		pin_set(LED_R, ( ((sr_r & 0x1) == 0) ) ? HIGH : LOW);
		sr_g = lfsr(sr_g, 0x8, 0x100000);
		pin_set(LED_G, ( ((sr_g & 0x1) == 0) ) ? HIGH : LOW);
		sr_b = lfsr(sr_b, 0x20, 0x80000);
		pin_set(LED_B, ( ((sr_b & 0x1) == 0) ) ? HIGH : LOW);
		vdelay(1000);
	}
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

/* fade_down() - gradually change the intensity of two LEDs - one up, one down.
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
