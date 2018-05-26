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


#ifndef IRINFO_H
#define IRINFO_H

#include <QtCore>
#include "ExcitCfg.h"

class IRInfo {
public:
	IRInfo() {}
	QString key;
	QString info;
	int length;
	int rate;
	int depth;
	int fMin;
	int fMax;
	double maxLevel; // in dB, <= 0.0

	QString format() const;
	QString maxLevelAsString() const;
	void fill(const ExcitCfg&);
};

#endif
