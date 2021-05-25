/*
	Copyright (C) 2006 Andrew Gaydenko <a@gaydenko.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "HarmData.h"

HarmData::HarmData() {
	this->length = 0;
	this->freqs = 0;
	this->values = 0;
}

HarmData::~HarmData() {
	if( this->freqs )
		delete this->freqs;
	if( this->values )
		delete this->values;
}

double HarmData::getValue(double freq) {
	if( (freq < this->freqs[0]) || (freq > this->freqs[this->length -1]) )
		throw QLE("frequency is out of range");

	for(int i=0; i < this->length; i++)
		if(this->freqs[i] > freq) {
			double tmp = this->values[i] - this->values[i-1];
			tmp *= freq - this->freqs[i-1];
			tmp /= this->freqs[i] - this->freqs[i-1];
			tmp += this->values[i-1];
			return tmp;
		}

	throw QLE("must not take place");
}
