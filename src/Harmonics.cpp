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
#include "Harmonics.h"
#include "WavIn.h"
#include "WavInfo.h"
#include "WavOut.h"
#include "Weights.h"
#include "IR.h"
#include "QLUtl.h"

Harmonics::Harmonics(const QString& aDirPath, IRInfo anInfo)
	: dirPath(aDirPath) {
	this->info = anInfo;
	this->irSamples =0;
	this->tone = 0;
	this->wavInfo = 0;
	this->init();
}

Harmonics::~Harmonics() {
	if(this->irSamples)
		delete this->irSamples;
	if(this->tone)
		delete this->tone;
	if(this->wavInfo)
		delete this->wavInfo;
}

void Harmonics::init() {
	// read ir.wav file
	WavIn* irWav = new WavIn(
		this->dirPath + "/" + this->info.key + IR::irFileName()
	);
	try {
		this->irSamples = irWav->readDouble();
	} catch(QLE e) {
		delete irWav;
		if(this->irSamples)
			delete this->irSamples;
		throw QLE(e);
	}
	this->wavInfo = irWav->getWavInfo();
	delete irWav;

	// find peak
	double max = 0.0;
	double tmp = 0.0;
	for(unsigned i=0; i < this->wavInfo->length; i++) {
		tmp = fabs(this->irSamples[i]);
		if(tmp > max) {
			max = tmp;
			this->maxIdx = i;
		}
	}
	if(
		(this->maxIdx < (this->wavInfo->length * 0.3)) ||
		(this->maxIdx > (this->wavInfo->length * 0.7))
	) {
		delete this->irSamples;
		throw QLE("peak position is very strange!");
	}
}

HarmData* Harmonics::getHarm(int num) {
	int leftShift = int(
		(this->getHarmIdx(num) - this->getHarmIdx(num+1)) * 0.2 + 0.5
	);
	if(leftShift > this->info.rate)
		leftShift = this->info.rate; // 1 second
	int rightShift = int(
		(this->getHarmIdx(num-1) - this->getHarmIdx(num)) * 0.8 + 0.5
	);

	if( ! this->tone)
		this->tone = this->doHarmonicFFT(1, leftShift, rightShift);
	HarmData* harm = this->doHarmonicFFT(num, leftShift, rightShift);
	if( ! harm)
		return 0; // have not more harmonics
	for(int i = 0; i < Harmonics::POINTS_AMOUNT; i++)
		harm->values[i] -= tone->getValue(harm->freqs[i]);

	return harm;
}

int Harmonics::getHarmIdx(int num) {
	if(num == 1)
		return this->maxIdx;
	if(num < 1)
		throw QLE("getHarmIdx: invalid harmonic number");

	// in accordance with Farina - time shift for n-th harmonic's IR
	double dt = this->info.length;
	dt *= log(num);
	dt /= log(this->info.fMax / this->info.fMin);

	return this->maxIdx - int(dt*this->info.rate + 0.5);
}

HarmData* Harmonics::doHarmonicFFT(int num, int leftShift, int rightShift) {
	// inverse filter is rounded with this freq
	double maxFreq = this->info.fMax;
	double minFreq = this->info.fMin * num;
	if(maxFreq < minFreq)
		return 0; // skip this harmonic

	// get samples
	int peak = this->getHarmIdx(num);
	int left = peak - leftShift;
	if(left < 0)
		return 0; // skip this harmonic at all

	int right = peak + rightShift;
	if(right > (int)this->wavInfo->length - 1)
		right = int(this->wavInfo->length - 1);

	int length = right - left + 1;
	double* samples = new double[length];
	for(int i = 0; i < length; i++)
		samples[i] = this->irSamples[left + i];

	// apply windows at edges
	Weights* w = new Weights("hanning", leftShift * 2 + 1);
	for(int i = 0; i <= leftShift; i++)
		samples[i] *= w->getPoint(i);
	delete w;

	w = new Weights("hanning", rightShift * 2 + 1);
	for(int i = 0; i <= rightShift; i++)
		samples[i + leftShift] *= w->getPoint(rightShift + i);
	delete w;

	// main harmonics-related work is here

	fftw_complex* fft = QLUtl::doFFT(samples, length, this->wavInfo->rate);
    delete[] samples;

	int fftResultLength = this->wavInfo->rate / 2;
	double* linAmps = new double[fftResultLength];
	for(int i = 0; i < fftResultLength; i++)
		linAmps[i] = sqrt( fft[i][0] * fft[i][0] + fft[i][1] * fft[i][1] );
	fftw_free(fft);

	QLUtl::toDbInPlace(linAmps, fftResultLength, false);

	double* freqs = QLUtl::logFreqs(
		Harmonics::POINTS_AMOUNT, minFreq, maxFreq
	);
	double* logAmps = QLUtl::spaceAmpsToFreqs(
		Harmonics::POINTS_AMOUNT, freqs, fftResultLength, linAmps
	);
    delete[] linAmps;

	if(num != 1)
		for(int i=0; i < Harmonics::POINTS_AMOUNT; i++)
			freqs[i] /=num;

	double* smoothed = QLUtl::smoothForLog(
		logAmps, freqs, 1.0/12.0, Harmonics::POINTS_AMOUNT
	);
	delete logAmps;

	HarmData* data = new HarmData();
	data->length = Harmonics::POINTS_AMOUNT;
	data->freqs = freqs;
	data->values = smoothed;
	return data;
}
