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
// HoldPeakHP-90EPC DMM

#define MAX_UNITS_STRING 64

class HoldPeak_90EPC : public DMM
{
	protected:
	char m_units[MAX_UNITS_STRING];
	
	bool ReadPacket();
	const char *format_units(unsigned int flags);
	

	public:
	HoldPeak_90EPC(const char *filename);
};
