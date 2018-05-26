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

#ifndef QLUTL_H
#define QLUTL_H

#include <QtWidgets>
#include <fftw3.h>
#include "QLUtl.h"
#include "QLE.h"

static const bool QL_DEBUG = true;

class QLUtl {
public:
	static double toDb(double d); // in comparison with 1.0

	static void d(std::string);
	static void d(QString);
	static void d(int);
	static void d(double);
	static void d(char*);
	static void d();

	static void checkFileError(const QFile&);

	static void showCritical(QWidget* parent, const QString&);
	static void showInfo(QWidget* parent, const QString&);
	static bool setComboToData(QComboBox* combo, int data);

	static double* smooth(double* in, double smoothFactor, int length);
	static double* fastSmooth(double* in, double smoothFactor, int length);
	static double* smoothForLog(
		double* in,
		double* freqs,
		double smoothFactor,
		int length
	);

	static void toDbInPlace(double* in, int length, bool normToZero);
	static double* logFreqs(int pointsAmount, double fMin, double fMax);
	static double* spaceAmpsToFreqs(
		int pointsAmount,
		double* freqs,
		int ampsAmount,
		double* amps
	);
	static fftw_complex* doFFT(double* inBuf, int inLength, int rate);
};

#endif
