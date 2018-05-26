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

#ifndef FILEFFT_H
#define FILEFFT_H

#include <QtCore>
#include <fftw3.h>

#include "QLE.h"
#include "IRInfo.h"

class FileFft {
public:
	static const int POINTS_AMOUNT = 1024 * 4;

	FileFft(const QString& path, const IRInfo& ii);
	~FileFft();

	double* getAmps(double smooth);
	double* getPhase(double smooth);
	double* getFreqs();

private:
	IRInfo ii;
	int fftResultLength; // rate/2
	fftw_complex* fftResult;
	double* freqs;

	void doFFT(QString path);
};

#endif
