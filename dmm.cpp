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
#include <time.h>

#include "dmm.h"

// This is a map of segments from a 7 segment
// display. You can use this to encode or decode
// a seven segment display.
// The bits are the segments that are illuminated
//   ---
//  |   |
//  |   |
//   ---
//  |   |
//  |   |
//   ---
seven_segment_t DMM::seven_segment_map[]={
	{0,' '},
	{0b0000101, '1'},
	{0b1011011, '2'},
	{0b0011111, '3'},
	{0b0100111, '4'},
	{0b0111110, '5'},
	{0b1111110, '6'},
	{0b0010101, '7'},
	{0b1111111, '8'},
	{0b0111111, '9'},
	{0b1111101, '0'},
	{0b1101000, 'L'},
	{0,0}
};

DMM::DMM(const char *filename)
{
	m_file = open(filename, O_RDWR);

	assert(m_file >= 0);
	setparms(2400,1,8);
}

DMM::~DMM()
{
}

// Simple print function
void DMM::print_output(float value, const char *units, unsigned int flags)
{
	time_t now = time(0);
	char *t = ctime(&now);
	while(*t)
	{
		if(*t==0 || *t==0xa ||  *t==0xd)
			break;
		putchar(*t);
		t++;
	}
	printf("%0.3f %s\n", value, units);
}

// Convert an unsigned character to a binary number string
char *DMM::fmtbinary(unsigned char c, int bits, char *ptr)
{
	char *p = ptr;	
	int i;
	for (i=(bits-1); i>=0; i--)
	{
		unsigned char m = (1<<i);
		if(c & m)
			*p++='1';
		else
			*p++='0';
	}
	*p=0;
	return ptr;
}

// Take the segments for a display character and
// return that character
char DMM::seven_segment_to_char(unsigned char map)
{
	for(int i=0; seven_segment_map[i].letter != 0; i++)
	{
		if(map == seven_segment_map[i].bitmap)
			return seven_segment_map[i].letter;
	}
	return ' ';
}

// Read the data bits from the packet structure
unsigned char DMM::read_data_bits(int ndx)
{
	return (m_packet[ndx] & 0x7) << 4 | (m_packet[ndx+1] & 0xF);
}

// Convert display info to a character
char DMM::read_char(int ndx)
{
	unsigned char map = read_data_bits(ndx);
	return seven_segment_to_char(map);
}

// Return the decimal point
bool DMM::read_decimal(int ndx)
{
	return (m_packet[ndx] & 0x8) ? true : false;
}

// Read the packet and format a number string
float DMM::read_numeric()
{
	char string[16];
	int pos=0;

	memset(string,0, sizeof(string));

	for(int i=2; i < 10; i+=2)
	{
		if(read_decimal(i))
			string[pos++] = (i==2) ? '-' : '.';
		string[pos++] = read_char(i);
	}
	return atof(string);
}

// Reads all the non-numeric fields and
// concatinates them into one unsigned int
unsigned int DMM::read_flags()
{
	unsigned flags = 0;
	int i;
	for(i=1; i < 15; i++)
	{
		if(i > 1 && i < 10) // Skip the digits in 2-9
			continue;

		flags = flags << 4;
		flags |= m_packet[i] & 0xF;
#ifdef DEBUG
		unsigned int nibble = m_packet[i] & 0xF;
		char str[16];
		printf("[%s %x]", fmtbinary(nibble, 4, str), nibble);
#endif
	}
	return flags;
}

// Loop and read data from the DMM
void DMM::loop()
{
	while(ReadPacket())
	{
		float f = read_numeric();
		unsigned int flags = read_flags();
		print_output(f, format_units(flags), flags);
	}
}

// This will set the baud rate of the file handle.
void DMM::setparms (int newbaud, char par, int bits)
{
	int spd = -1;
	int bit = bits;
	struct termios tty;

	tcgetattr (m_file, &tty);

	/*
	 * We generate mark and space parity ourself. 
	 */
	if (bit == 7 && (par == 'M' || par == 'S'))
		bit = 8;

	switch (newbaud)
	{
	case 0:
		spd = 0;
		break;
	case 300:
		spd = B300;
		break;
	case 600:
		spd = B600;
		break;
	case 1200:
		spd = B1200;
		break;
	case 2400:
		spd = B2400;
		break;
	case 4800:
		spd = B4800;
		break;
	case 9600:
		spd = B9600;
		break;
	case 19200:
		spd = B19200;
		break;
	case 38400:
		spd = B38400;
		break;
	case 57600:
		spd = B57600;
		break;
	case 115200:
		spd = B115200;
		break;
	}

	if (spd != -1)
	{
		cfsetospeed (&tty, (speed_t) spd);
		cfsetispeed (&tty, (speed_t) spd);
	}

	switch (bit)
	{
	case 5:
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
		break;
	case 6:
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
		break;
	case 7:
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
		break;
	case 8:
	default:
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
		break;
	}
	cfmakeraw (&tty);
	tcsetattr (m_file, TCSANOW, &tty);
}


