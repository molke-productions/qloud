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

#include <math.h>

#include "Excitation.h"
#include "Weights.h"
#include "WavInfo.h"
#include "WavIn.h"
#include "WavOut.h"

static const double START_SMOOTH = 0.05;
static const double FINISH_SMOOTH = 0.005;

void Excitation::generate(const QString& dirPath, const ExcitCfg& cfg) {
	// prepare buffer with Farina’s excitation
	double L = double(cfg.length) / log(double(cfg.fMax)/cfg.fMin);
	double K = 2.0 * M_PI * cfg.fMin * L;
	int length = cfg.length * cfg.rate;
	if(length % 2)
		length++; // to simplify: 1).reversing on place, 2). r2c fft-ing
	int fileLength = length + cfg.rate; // + 1 second for sound delay/decay
	double* buf = new double[fileLength];
	// log sine at first
	for(int i=0; i < length; i++) {
		double t = ((double)i) / cfg.rate;
		buf[i] = sin(K * (exp(t/L) - 1.0)) * 0.9999; // to be sure our float is below 0db
	}
	// apply start smoothing window...
	int winLength = (int)(START_SMOOTH * length);
	Weights* w = new Weights("hanning", winLength * 2 + 1);
	for(int i=0; i <= winLength; i++)
		buf[i] = buf[i] * w->getPoint(i);
	delete w;
	// ... and finish smoothing window
	winLength = (int)(FINISH_SMOOTH * length);
	w = new Weights("hanning", winLength * 2 + 1);
	int bufIdx = length - winLength -1;
	int winIdx = winLength;
	for(int i=0; i <= winLength; i++)
		buf[bufIdx + i] = buf[bufIdx + i] * w->getPoint(winIdx + i);
	delete w;

	// add 1 sec zeros
	for(int i = length; i < fileLength; i++)
		buf[i] = 0.0;

	// write excitation file ------------------------------------------------------
	WavInfo wavInfo;
	wavInfo.rate = cfg.rate;
	wavInfo.length = fileLength;
	wavInfo.channels = 1;
	wavInfo.bitDepth = cfg.depth;

	WavOut* excitOut = new WavOut(dirPath + "/" + Excitation::excitationFileName());
	try {
		excitOut->writeDouble(wavInfo, buf);
	} catch(QLE e) {
		delete excitOut;
		delete buf;
		throw QLE(e.msg);
	}
	delete excitOut;

	// write reverse filter file --------------------------------------------------
	// revers the buf on place
	for(int i=0; i < fileLength/2; i++) {
		double t = buf[i];
		buf[i] = buf[fileLength -  i - 1];
		buf[fileLength -  i - 1] = t;
	}
	// apply decay
	double factor = log2( double(cfg.fMax) / cfg.fMin ) / length;
	for(int i = cfg.rate; i < fileLength; i++) // skip 1 sec
		buf[i] *= pow(0.5, factor * (i - cfg.rate));
	// write with the same WavInfo
	WavOut* filterOut = new WavOut(dirPath + "/" + Excitation::filterFileName());
	try {
		filterOut->writeDouble(wavInfo, buf);
	} catch(QLE e) {
		delete filterOut;
		delete buf;
		throw QLE(e.msg);
	}
	delete filterOut;
	delete buf;
}
