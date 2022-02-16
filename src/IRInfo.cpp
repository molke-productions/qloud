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

#include "IRInfo.h"

void IRInfo::fill(const ExcitCfg& ecfg) {
	this->length = ecfg.length;
	this->rate = ecfg.rate;
	this->depth = ecfg.depth;
	this->fMin = ecfg.fMin;
	this->fMax = ecfg.fMax;
}

QString IRInfo::maxLevelAsString() const {
	return QString::number(maxLevel, 'f', 1);
}

QString IRInfo::format() const {
	QString s;
	s += QVariant(this->length).toString();
	s += " s, ";
	s += QVariant(this->rate).toString();;
	s += " Hz/";
	s += QVariant(this->depth).toString();
	s += " bit, ";
	s += QVariant(this->fMin).toString();
	s += "–";
	s += QVariant(this->fMax).toString();
	s += " Hz";
	return s;
}
