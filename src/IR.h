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

#ifndef IR_H
#define IR_H

#include <QtCore>
#include <fftw3.h>
#include <QLE.h>

class IR {

public:
	IR(const QString& aDirPath, QString aPrefix);

	static QString irFileName() {
		return QString("ir.wav");
	}
	static QString trimmedIrFileName() {
		return QString("irTrimmed.wav");
	}
	static QString powerFileName() {
		return QString("irPower.wav");
	}

	void generate();
	void trim(double secs);
	double getMaxTrimLength();

private:
	const QString& dirPath;
	QString prefix;
	int maxIdx;
	double maxTrimLength; // secs
};

#endif
