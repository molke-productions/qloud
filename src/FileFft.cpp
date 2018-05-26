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

#include <cmath>
#include "QLUtl.h"
#include "WavIn.h"
#include "FileFft.h"

FileFft::FileFft(const QString& path, const IRInfo& anIi) {
	this->ii = anIi;

	this->fftResult = 0;
	this->freqs = 0;

	this->doFFT(path);
	this->freqs = QLUtl::logFreqs(
		FileFft::POINTS_AMOUNT,
		this->ii.fMin,
		this->ii.fMax
	);
}

FileFft::~FileFft() {
	if(this->fftResult)
		fftw_free(this->fftResult);
	if(this->freqs)
		delete this->freqs;
}

double* FileFft::getAmps(double smoothFactor) {
	double* linAmps = new double[this->fftResultLength];
	for(int i=0; i < this->fftResultLength; i++) {
		double tmp = this->fftResult[i][0] * this->fftResult[i][0];
		tmp += this->fftResult[i][1] * this->fftResult[i][1];
		linAmps[i] = sqrt(tmp);
	}

	QLUtl::toDbInPlace(linAmps, this->fftResultLength, true);

	double* logAmps = QLUtl::spaceAmpsToFreqs(FileFft::POINTS_AMOUNT, this->freqs,
											  this->fftResultLength, linAmps);
	delete linAmps;

	double* smoothed = QLUtl::smoothForLog(logAmps, this->freqs, smoothFactor,
										   FileFft::POINTS_AMOUNT);
	delete logAmps;

	// amplitudes must be scaled to max recorded level. Do it on place.
	for(int i=0; i < FileFft::POINTS_AMOUNT; i++)
		smoothed[i] += this->ii.maxLevel;

	return smoothed;
}

double* FileFft::getPhase(double smoothFactor) {
	double* linPhase = new double[this->fftResultLength];
	double factor = 180.0 / M_PI;

	for(int i=0; i < this->fftResultLength; i++) {
		double tmp = atan2(this->fftResult[i][1], this->fftResult[i][0]);
		linPhase[i] = tmp * factor;
	}

	double* smoothed = QLUtl::fastSmooth(linPhase, smoothFactor, this->fftResultLength);
	delete linPhase;

	double* logPhase = QLUtl::spaceAmpsToFreqs(FileFft::POINTS_AMOUNT, this->freqs, this->fftResultLength, smoothed);

	delete smoothed;

	return logPhase;
}

double* FileFft::getFreqs() {
	double* tmp = new double[FileFft::POINTS_AMOUNT];
	for(int i=0; i < FileFft::POINTS_AMOUNT; i++)
		tmp[i] = this->freqs[i];
	return tmp;
}

// private
void FileFft::doFFT(QString path) {
	WavIn* wav = new WavIn(path);
	WavInfo* wavInfo = wav->getWavInfo();
	this->fftResultLength = wavInfo->rate / 2;

	double* wavSamples = wav->readDouble();
	this->fftResult = QLUtl::doFFT(wavSamples, wavInfo->length, wavInfo->rate);

	delete wavInfo;
	delete wav;
	delete wavSamples;
}
