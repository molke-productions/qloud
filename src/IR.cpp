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

#include <fftw3.h>
#include <cmath>
#include "QLUtl.h"
#include "QLCfg.h"
#include "Excitation.h"
#include "Capture.h"
#include "WavInfo.h"
#include "WavIn.h"
#include "WavOut.h"
#include "Weights.h"
#include "IR.h"

IR::IR(QString aDirPath, QString aPrefix) {
	this->dirPath = aDirPath;
	this->prefix = aPrefix;
	this->maxIdx = -1;
	this->maxTrimLength = -1.0;
}


double IR::getMaxTrimLength() {
	if(this->maxTrimLength > 0.0)
		return this->maxTrimLength;

	// read ir.wav file
	WavIn* irWav = new WavIn(
		this->dirPath + "/" + this->prefix + IR::irFileName()
	);
	double* irSamples =0;
	try {
		irSamples = irWav->readDouble();
	} catch(QLE e) {
		delete irWav;
		if(irSamples)
			delete irSamples;
		throw QLE(e);
	}
	WavInfo* wavInfo = irWav->getWavInfo();
	delete irWav;

	// find peak
	double max = 0.0;
	double tmp = 0.0;
	for(unsigned i=0; i < wavInfo->length; i++) {
		tmp = fabs(irSamples[i]);
		if(tmp > max) {
			max = tmp;
			this->maxIdx = i;
		}
	}
	if(
		(this->maxIdx < (wavInfo->length * 0.3)) ||
		(this->maxIdx > (wavInfo->length * 0.7))
	) {
		delete irSamples;
		throw QLE("Peak position is very strange!");
	}
	delete irSamples;

	// get max window width
	this->maxTrimLength = double(wavInfo->length - maxIdx - 2) / wavInfo->rate;
	delete wavInfo;
	return this->maxTrimLength;
}

void IR::generate() {
	// read input files
	WavIn* filterWav = new WavIn(
		this->dirPath + "/" + Excitation::filterFileName()
	);
	WavIn* respWav = new WavIn(
		this->dirPath + "/" + Capture::responseFileName()
	);

	double* realFilter = 0;
	double* realResp = 0;

	try {
		realFilter = filterWav->readDouble();
		realResp = respWav->readDouble();
	} catch(QLE e) {
		delete filterWav;
		delete respWav;
		if(realFilter)
			delete realFilter;
		if(realResp)
			delete realResp;
		throw QLE(e);
	}

	if(filterWav->getLength() != respWav->getLength()) {
		delete filterWav;
		delete respWav;
		delete realFilter;
		delete realResp;
		throw QLE(
			Excitation::filterFileName() +
			" and " +
			Capture::responseFileName() +
			" have different lengths!"
		);
	}

	// saved to use while IR writing
	WavInfo* wavInfo = filterWav->getWavInfo();
	delete filterWav;
	delete respWav;

	// convert input samples to complex
	unsigned fftLength = wavInfo->length * 2;

	fftw_complex* filterBuf =
		(fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftLength);
	fftw_complex* respBuf =
		(fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftLength);
	for(unsigned i = 0; i < wavInfo->length; i++) {
		filterBuf[i][0] = realFilter[i];
		filterBuf[i][1] = 0.0;
		respBuf[i][0] = realResp[i];
		respBuf[i][1] = 0.0;
	}
	// zero pad
	for(unsigned i = wavInfo->length; i < fftLength; i++) {
		filterBuf[i][0] = 0.0;
		filterBuf[i][1] = 0.0;
		respBuf[i][0] = 0.0;
		respBuf[i][1] = 0.0;
	}
	delete realFilter;
	delete realResp;

	// do forward FFT transform
	fftw_complex* filterFft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftLength);
	fftw_plan plan = fftw_plan_dft_1d(fftLength, filterBuf, filterFft, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	fftw_free(filterBuf);

	fftw_complex* respFft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftLength);
	plan = fftw_plan_dft_1d(fftLength, respBuf, respFft, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	fftw_free(respBuf);

	// multiply ----------------------------------------------------------------
	fftw_complex* product = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftLength);
	double normFactor = 1.0 / fftLength;
	for(unsigned i = 0; i < fftLength; i++) {
		product[i][0] = filterFft[i][0] * respFft[i][0] - filterFft[i][1] * respFft[i][1];
		product[i][0] *= normFactor;
		product[i][1] = filterFft[i][0] * respFft[i][1] + filterFft[i][1] * respFft[i][0];
		product[i][1] *= normFactor;
	}
	fftw_free(filterFft);
	fftw_free(respFft);

	// inverse complex to complex transform, here we get IR at last :-) --------
	fftw_complex* irBuf = new fftw_complex[fftLength];
	plan = fftw_plan_dft_1d(fftLength, product, irBuf, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	fftw_free(product);

	// get real IR part and normalize ------------------------------------------
	double* realIr = new double[fftLength];
	double max = 0.0;
	double tmpAbs = 0.0;
	for(unsigned i=0; i < fftLength; i++) {
		realIr[i] = irBuf[i][0];
		tmpAbs = fabs(realIr[i]);
		if( tmpAbs > max )
			max = tmpAbs;
	}
	delete irBuf;
	for(unsigned i=0; i < fftLength; i++)
		realIr[i] /= max;

	// save IR -----------------------------------------------------------------
	WavOut* wavOut = new WavOut(this->dirPath + "/" + this->prefix + IR::irFileName());
	wavInfo->length = fftLength;
	try {
		wavOut->writeDouble(*wavInfo, realIr);
	} catch(QLE e) {
		delete wavOut;
		delete realIr;
		throw QLE(e.msg);
	}
	delete wavOut;
	delete wavInfo;
	delete realIr;
}


void IR::trim(double secs) {
	if(secs < 0.0001)
		throw QLE("window width too small");

	// read ir.wav file --------------------------------------------------------
	WavIn* irWav = new WavIn(this->dirPath + "/" + this->prefix + IR::irFileName());
	double* irSamples =0;
	try {
		irSamples = irWav->readDouble();
	} catch(QLE e) {
		delete irWav;
		if(irSamples)
			delete irSamples;
		throw QLE(e);
	}
	WavInfo* wavInfo = irWav->getWavInfo();
	delete irWav;

	// find peak ---------------------------------------------------------------
	if(this->maxIdx < 0) {
		double max = 0.0;
		double tmp = 0.0;
		for(unsigned i=0; i < wavInfo->length; i++) {
			tmp = fabs(irSamples[i]);
			if(tmp > max) {
				max = tmp;
				this->maxIdx = i;
			}
		}
	}
	if((this->maxIdx < (wavInfo->length * 0.3)) || (this->maxIdx > (wavInfo->length * 0.7))) {
		delete irSamples;
		throw QLE("peak position is very strange!");
	}

	// apply left window -------------------------------------------------------
	// find second harmonic's IR peak
	QLCfg* cfg = new QLCfg(this->dirPath);
	IRInfo ii = cfg->getIr(this->prefix);
	delete cfg;
	double dt = ii.length * 0.2;
	dt *= log(2.0);
	dt /= log(ii.fMax / ii.fMin);
	int leftShift = int(dt * ii.rate + 0.5);
	if(leftShift > maxIdx)
		leftShift = maxIdx;
	int left = maxIdx - leftShift;
	Weights* w = new Weights("hanning", leftShift * 2 + 1);
	for(int i=0; i <= leftShift; i++)
		irSamples[left + i] *= w->getPoint(i);
	delete w;

	// apply right window in accordance with trim value ------------------------
	int rightShift = int(secs * ii.rate + 0.5);
	int right = maxIdx + rightShift;
	if(right > int(wavInfo->length))
		throw QLE("Window too wide");
	w = new Weights("hanning", rightShift * 2 + 1);
	for(int i=0; i <= rightShift; i++)
		irSamples[maxIdx + i] *= w->getPoint(i + rightShift);
	delete w;

	// save result -------------------------------------------------------------
	int winLength = right - left + 1;
	double* trimmed = new double[winLength];
	for(int i=0; i < winLength; i++)
		trimmed[i] = irSamples[left + i];
	delete irSamples;

	wavInfo->length = winLength;
	WavOut* trimOut = new WavOut(this->dirPath + "/" + this->prefix + IR::trimmedIrFileName());
	try {
		trimOut->writeDouble(*wavInfo, trimmed);
	} catch(QLE e) {
		delete trimOut;
		delete trimmed;
		throw QLE(e.msg);
	}
	delete trimOut;
	delete wavInfo;
	delete trimmed;
}
