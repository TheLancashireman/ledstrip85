/* ledstrip85.h - header file for ledstrip85
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
#ifndef LEDSTRIP85_H
#define LEDSTRIP85_H	1

#include "tinylib.h"
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

/* Global variables. For descriptions, see ledstrip85.c
*/
extern char mode;
extern char speed;
extern char level[3];
extern u16_t lastpress;
extern u32_t lastpresstime;
extern unsigned long sr_r;
extern unsigned long sr_g;
extern unsigned long sr_b;
extern jmp_buf jb;

/* Mode functions. For descriptions, see the corresponding C file
*/
extern void mode_all_off(void)			__attribute__ ((noreturn));
extern void mode_all_on(void)			__attribute__ ((noreturn));
extern void mode_3_cycle(void)			__attribute__ ((noreturn));
extern void mode_8_cycle(void)			__attribute__ ((noreturn));
extern void mode_8_fade(void)			__attribute__ ((noreturn));
extern void mode_3_fade(void)			__attribute__ ((noreturn));
extern void mode_variable(void) 		__attribute__ ((noreturn));
extern void mode_flicker(void)			__attribute__ ((noreturn));
extern void mode_random_1_fade(void)	__attribute__ ((noreturn));

/* Obsolescent ...
*/
extern void mode_red(void)				__attribute__ ((noreturn));
extern void mode_green(void)			__attribute__ ((noreturn));
extern void mode_blue(void)				__attribute__ ((noreturn));

/* Global functions. For descriptions, see ledstrip85.c
*/
extern void all_off(void);
extern void all_on(void);
extern void fade_up(int pin);
extern void fade_down(int pin);
extern void fade_up_down(int up_pin, int down_pin);
extern void tick_delay(unsigned long tix);
extern void mode_check(void);
extern unsigned long lfsr(unsigned long v, unsigned long tap, unsigned long fb);

/* vdelay() - a variable delay - the delay depends on the current speed
 *
 * Higher speed ==> shorter delay
 *
 * The nominal delay for speed 10 is in milliseconds.
*/
static inline void vdelay(unsigned long ms)
{
	tick_delay(us_to_ticks(ms*100*(20-speed)));
}

/* lfsr_r() - crank the "red" lfsr
*/
static inline unsigned long lfsr_r(void)
{
	sr_r = lfsr(sr_r, 0x8, 0x20000);
	return sr_r;
}

/* lfsr_g() - crank the "green" lfsr
 *
 * This is the longest of the LSFRs.
*/
static inline unsigned long lfsr_g(void)
{
	sr_g = lfsr(sr_g, 0x8, 0x100000);
	return sr_g;
}

/* lfsr_b() - crank the "blue" lfsr
*/
static inline unsigned long lfsr_b(void)
{
	sr_b = lfsr(sr_b, 0x20, 0x80000);
	return sr_b;
}

#endif
