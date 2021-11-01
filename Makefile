# Makefile for ledstrip85 - attiny85 version of ledstrip, using tiny-bare-metal libraries

# Copyright David Haworth
# 
# This file is part of ledstrip85.
# 
# ledstrip85 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# ledstrip85 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with ledstrip85  If not, see <http://www.gnu.org/licenses/>.


BUILD		?=	build
ISPPORT		?=	/dev/ttyUSB0
OBJ_DIR		?=	$(BUILD)/o
LIB_DIR		?=	$(BUILD)/a
TLIB_DIR	?=	../tiny-bare-metal/tinylib
TIO_DIR		?=	../tiny-bare-metal/tinyio
TIR_DIR		?=	../tiny-bare-metal/tinyir

GCC			=	avr-gcc
GLD			=	avr-gcc
GAR			=	avr-ar
OBJCOPY		=	avr-objcopy

CC_OPT		+=	-mmcu=attiny85
CC_OPT		+=	-fno-builtin
CC_OPT		+=	-Os
CC_OPT		+=	-g
CC_OPT		+=	-Wall
CC_OPT		+=	-I $(TLIB_DIR)
CC_OPT		+=	-I $(TIO_DIR)
CC_OPT		+=	-I $(TIR_DIR)
CC_OPT		+=	-D ASYNC_BITRATE=9600
CC_OPT		+=	-D ASYNC_TX_PIN=PB4
CC_OPT		+=	-D TINYIR_REMOTE="\"ir-daewoo.h\""
CC_OPT		+=	-D IR_RX_PIN=PB0
CC_OPT		+=	-D RBSIZE=31

LD_OPT		+=	-mmcu=attiny85
LD_OPT		+=	-Wl,--gc-sections
LD_OPT		+=	-Os
LD_OPT		+=	-L $(LIB_DIR)

LD_LIB		+=	-ltiny
LD_LIB		+=	-lm

OBJS		+=	$(OBJ_DIR)/ledstrip85.o
OBJS		+=	$(OBJ_DIR)/lfsr.o

VPATH		+=	$(TLIB_DIR)
VPATH		+=	$(TIO_DIR)
VPATH		+=	$(TIR_DIR)

.PHONY:		default clean hex upload

default:	$(BUILD)/ledstrip85.elf

clean:
	if [ -d $(BUILD) ] ; then rm -r $(BUILD); fi

hex:		$(BUILD)/ledstrip85.ihex

$(BUILD)/o:
	mkdir -p $(BUILD)/o

$(BUILD)/ledstrip85.elf:	$(OBJ_DIR) $(OBJS) $(LIB_DIR)/libtiny.a
	$(GLD) $(LD_OPT) -o $@ $(OBJS) $(LD_LIB)
	

$(OBJ_DIR)/%.o:	%.c
	$(GCC) $(CC_OPT) -o $@ -c $<

$(BUILD)/ledstrip85.ihex:	$(BUILD)/ledstrip85.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload:		$(BUILD)/ledstrip85.ihex
	avrdude -P $(ISPPORT) -b 19200 -c avrisp -p t85 -U flash:w:build/ledstrip85.ihex:i

include $(TIO_DIR)/tinyio.make
include $(TIR_DIR)/tinyir.make
include $(TLIB_DIR)/tinylib.make
