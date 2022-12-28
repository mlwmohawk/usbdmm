/*
	This program is intended to interface with cheap USB/RS-232 
	Digital DMM.

	Copyright (C) 2022  Mark L. Woodward

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/termios.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dmm.h"
#include "hp90epc.h"

HoldPeak_90EPC::HoldPeak_90EPC(const char *filename) : DMM(filename)
{
}

// These are the bits on the display and
// what they map to.
#define _90EPC_AC			0x800000
#define _90EPC_DC			0x400000
#define _90EPC_AUTO			0x200000

#define _90EPC_MICRO		0x080000
#define _90EPC_NANO			0x040000
#define _90EPC_KILO		    0x020000
#define _90EPC_DIODE		0x010000

#define _90EPC_MILI			0x008000
#define _90EPC_DUTY			0x004000
#define _90EPC_MEGA		    0x002000
#define _90EPC_SPK		    0x001000

#define _90EPC_FARAD		0x000800
#define _90EPC_OHMS			0x000400
#define _90EPC_REL			0x000200
#define _90EPC_HOLD			0x000100

#define _90EPC_AMPS			0x000080
#define	_90EPC_VOLTS		0x000040
#define	_90EPC_HERTZ		0x000020
#define _90EPC_UK1			0x000010

#define _90EPC_UK2			0x000008
#define	_90EPC_DEG			0x000004
#define	_90EPC_UK3			0x000002
#define _90EPC_UK4			0x000001

// Map the display flags to a "units" string.
const char *HoldPeak_90EPC::format_units(unsigned int flags)
{
	const char *acdc = "";
	const char *units = "";

	if(flags & _90EPC_VOLTS)
	{
		acdc = (flags &_90EPC_AC) ? "AC" : "DC";
		units = "Volts";
		if(flags & _90EPC_MICRO)
			units = "Microvolts";
		else if (flags & _90EPC_NANO)
			units = "Nanovolts";
		else if(flags & _90EPC_KILO)
			units = "Kilovolts";
		else if (flags & _90EPC_MILI)
			units= "Miliivolts";
		else if(flags & _90EPC_MEGA)
			units = "Megavolts";

	}
	else if(flags & _90EPC_OHMS)
	{
		units = "Ohms";
		if(flags & _90EPC_MICRO)
			units = "Micro ohms";
		else if (flags & _90EPC_NANO)
			units = "Nano ohms";
		else if(flags & _90EPC_KILO)
			units = "Kilo ohms";
		else if (flags & _90EPC_MILI)
			units= "Milli ohms";
		else if(flags & _90EPC_MEGA)
			units = "Mega ohms";
	}
	else if(flags & _90EPC_FARAD)
	{
		units = "Farads";
		if(flags & _90EPC_MICRO)
			units = "Micro Farads";
		else if (flags & _90EPC_NANO)
			units = "Nano Farads";
		else if(flags & _90EPC_KILO)
			units = "Kilo Farads";
		else if (flags & _90EPC_MILI)
			units= "Milli Farads";
		else if(flags & _90EPC_MEGA)
			units = "Mega Farads";
	}
	else if(flags & _90EPC_HERTZ)
	{
		units = "Hz";
		if(flags & _90EPC_MICRO)
			units = "microhertz";
		else if (flags & _90EPC_NANO)
			units = "nanoHz";
		else if(flags & _90EPC_KILO)
			units = "Khz";
		else if (flags & _90EPC_MILI)
			units= "Milli hertz";
		else if(flags & _90EPC_MEGA)
			units = "Mhz";
	}
	else if(flags & _90EPC_DEG)
	{
		units="C";
	}
	else if(flags & _90EPC_AMPS)
	{
		units="A";
		acdc = (flags &_90EPC_AC) ? "AC" : "DC";
		if(flags & _90EPC_MICRO)
			units = "microamps";
		else if (flags & _90EPC_NANO)
			units = "nanoamps";
		else if(flags & _90EPC_KILO)
			units = "kiloamps";
		else if (flags & _90EPC_MILI)
			units= "milliamps";
		else if(flags & _90EPC_MEGA)
			units = "Mega amps";
	}
	snprintf(m_units, sizeof(m_units)-1, "%s %s", units, acdc);

	return m_units;
}

bool HoldPeak_90EPC::ReadPacket()
{
	memset(m_packet, 0, sizeof(m_packet));
	while(1)
	{
		unsigned char c=0;
		if(read(m_file, &c, 1)!=1)
			break;
		int index = ((c&0xF0) >> 4);
		int data = ((c&0xF));
		m_packet[index]=data;

		if(index == 0xe && m_packet[index])
			return true;
	}
	return false;
}


int main(int argc, char **argv)
{
	HoldPeak_90EPC dmm("/dev/ttyUSB0");
	dmm.loop();
}
