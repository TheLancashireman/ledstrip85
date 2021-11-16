/* rc.h - IR remote control codes
 *
 * (c) 2020 David Haworth
 *
 * rc.h is part of ledstrip
 *
 * ledstrip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ledstrip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ledstrip.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ledstrip is an Arduino sketch, written for an Arduino Nano
*/
#ifndef RC_H
#define RC_H	1

#include <tinyir.h>

#if defined IR_DAEWOO_H

/* Mappings from Daewoo keys to ledstrip controls
*/
#define BTN_OFF		IRBTN_ONOFF
#define BTN_M1		IRBTN_1
#define BTN_M2		IRBTN_2
#define BTN_M3		IRBTN_3
#define BTN_M4		IRBTN_4
#define BTN_M5		IRBTN_5
#define BTN_M6		IRBTN_6
#define BTN_M7		IRBTN_7
#define BTN_M8		IRBTN_8
#define BTN_M9		IRBTN_9
#define BTN_M10		IRBTN_0

#define BTN_FASTER	IRBTN_IPLUS
#define BTN_SLOWER	IRBTN_IMINUS

#define BTN_L1u		IRBTN_SPLP
#define BTN_L1d		IRBTN_LEFT
#define BTN_L2u		IRBTN_PRPLUS
#define BTN_L2d		IRBTN_OK
#define BTN_L3u		IRBTN_MENU
#define BTN_L3d		IRBTN_RIGHT

#elif defined IR_EUROPA_SR150_H

#warning "Europa SR-150 to do"

/* Mappings from Europa SR-150 keys to ledstrip controls
*/
#define BTN_OFF		IRBTN_STANDBY
#define BTN_M1		IRBTN_1
#define BTN_M2		IRBTN_2
#define BTN_M3		IRBTN_3
#define BTN_M4		IRBTN_4
#define BTN_M5		IRBTN_5
#define BTN_M6		IRBTN_6
#define BTN_M7		IRBTN_7
#define BTN_M8		IRBTN_8
#define BTN_M9		IRBTN_9
#define BTN_M10		IRBTN_0

#define BTN_FASTER	IRBTN_CH_UP
#define BTN_SLOWER	IRBTN_CH_DN

#define BTN_L1u		IRBTN_VID_UP
#define BTN_L1d		IRBTN_VID_DN
#define BTN_L2u		IRBTN_AB
#define BTN_L2d		IRBTN_VH
#define BTN_L3u		IRBTN_AUD_UP
#define BTN_L3d		IRBTN_AUD_DN

#else
#error "Unknown/unsupported handset"
#endif

#endif
