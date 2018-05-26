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

#ifndef HARMONICS_H
#define HARMONICS_H

#include <QtCore>
#include "HarmData.h"
#include "IRInfo.h"
#include "WavInfo.h"

class Harmonics {
public:
	static const int POINTS_AMOUNT = 4096;

	Harmonics(QString aDirPath, IRInfo anInfo);
	~Harmonics();

	HarmData* getHarm(int num);

private:
	QString dirPath;
	IRInfo info;
	double* irSamples;
	int maxIdx;
	WavInfo* wavInfo;
	HarmData* tone;

	void init();
	double getMaxFreq(int num);
	int getHarmIdx(int num);
	HarmData* doHarmonicFFT(int num, int leftShift, int rightShift);
};

#endif
