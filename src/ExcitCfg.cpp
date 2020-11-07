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

#include <QtCore>
#include "ExcitCfg.h"

ExcitCfg::ExcitCfg() {
	this->length = 7;
	this->rate = 48000;
	this->depth = 32;
	this->fMin = 100;
	this->fMax = 10000;
}

QString ExcitCfg::toString() {
	QString s;
	s += QVariant(this->length).toString();
	s += " s, ";
	s += QVariant(this->rate).toString();
	s += " Hz/";
	s += QVariant(this->depth).toString();
	s += " bit, ";
	s += QVariant(this->fMin).toString();
	s += "–";
	s += QVariant(this->fMax).toString();
	s += " Hz";
	return s;
}

void ExcitCfg::check() const {
	if(this->length < 1)
		throw QLE("Excitation length is too short!");
	if(this->length > 50)
		throw QLE("Excitation length is too long!");
	if((this->depth != 32) && (this->depth != 24) && (this->depth != 16))
		throw QLE("Excitation bit depth is invalid!");
	if(
		(this->rate != 32000) &&
		(this->rate != 44100) &&
		(this->rate != 48000) &&
		(this->rate != 64000) &&
		(this->rate != 88200) &&
		(this->rate != 96000) &&
		(this->rate != 128000) &&
		(this->rate != 176400) &&
		(this->rate != 192000)
	)
		throw QLE("Excitation rate is invalid!");
	if(this->fMin <  1)
		throw QLE("Excitation min. frequency is too small!");
	if(this->fMax >  (this->rate / 2))
		throw QLE("Excitation max. frequency is too high!");
	if(this->fMin * 2 > this->fMax)
		throw QLE("Excitation min. frequency must be at least twice less than max one!");
}
