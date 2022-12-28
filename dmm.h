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
#ifndef _DMM_H_
#define _DMM_H_

#define MAX_DMM_PACKET 64

struct seven_segment_t
{
	unsigned char bitmap;
	char letter;
};


class DMM
{
	protected:
	static seven_segment_t seven_segment_map[];

	virtual void print_output(float value, const char *units, unsigned int flags);
	virtual bool ReadPacket()=0;
	virtual const char *format_units(unsigned int flags)=0;

	int m_file;
	unsigned char m_packet[MAX_DMM_PACKET];

	float read_numeric();
	unsigned int read_flags();

	bool read_decimal(int ndx);
	char read_char(int ndx);
	char *fmtbinary(unsigned char c, int bits, char *ptr);
	char seven_segment_to_char(unsigned char bits);
	unsigned char read_data_bits(int ndx);

	public:
	DMM(const char *filename);
	~DMM();
	void setparms (int newbaud, char par, int bits);
	void loop();
};

#endif
