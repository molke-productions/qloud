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

#ifndef WAVOUT_H
#define WAVOUT_H

#include <QtCore>
#include <WavInfo.h>
#include <QLE.h>

class WavOut {
public:
	WavOut(QString aPath);
	~WavOut();
	void writeFloat(WavInfo info, float* buf);
	void writeDouble(WavInfo info, double* buf);

private:
	void write(WavInfo info, int size, char* buf);
	QString path;
};

#endif
