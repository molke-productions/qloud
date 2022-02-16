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
#include "QLWin.h"
#include "Weights.h"
#include "WavInfo.h"
#include "WavIn.h"
#include "WavOut.h"

static const double START_SMOOTH = 0.05;
static const double FINISH_SMOOTH = 0.005;

Excitation::Excitation(QObject* parent)
	: QObject(parent) {
}

Excitation::~Excitation() {
	if (m_qlCfg) {
		delete m_qlCfg;
		m_qlCfg = nullptr;
	}
}

void Excitation::setWorkDir(const QString& dir) {
	if (m_qlCfg) {
		delete m_qlCfg;
		m_qlCfg = nullptr;
	}
	m_qlCfg = new QLCfg(dir);
	m_lastConfig = m_qlCfg->getExcit();
	m_newConfig = m_lastConfig;
}

void Excitation::generate() {
	newConfig().check();
	m_lastConfig = m_newConfig;
	m_qlCfg->setExcit(lastConfig());

	// prepare buffer with Farina’s excitation
	double L = double(m_newConfig.length) / log(double(m_newConfig.fMax)/m_newConfig.fMin);
	double K = 2.0 * M_PI * m_newConfig.fMin * L;
	int length = m_newConfig.length * m_newConfig.rate;
	if(length % 2)
		length++; // to simplify: 1).reversing on place, 2). r2c fft-ing
	int fileLength = length + m_newConfig.rate; // + 1 second for sound delay/decay
	m_excitation.resize(fileLength);
	// log sine at first
	for(int i = 0; i < length; i++) {
		double t = ((double)i) / m_newConfig.rate;
		// to be sure our float is below 0db
		m_excitation[i] = sin(K * (exp(t/L) - 1.0)) * 0.9999;
	}
	// apply start smoothing window...
	int winLength = (int)(START_SMOOTH * length);
	Weights* w = new Weights("hanning", winLength * 2 + 1);
	for(int i = 0; i <= winLength; i++)
		m_excitation[i] = m_excitation[i] * w->getPoint(i);
	delete w;
	// ... and finish smoothing window
	winLength = (int)(FINISH_SMOOTH * length);
	w = new Weights("hanning", winLength * 2 + 1);
	int bufIdx = length - winLength -1;
	int winIdx = winLength;
	for(int i = 0; i <= winLength; i++)
		m_excitation[bufIdx + i] = m_excitation[bufIdx + i] * w->getPoint(winIdx + i);
	delete w;

	// add 1 sec zeros
	for(int i = length; i < fileLength; i++)
		m_excitation[i] = 0.0;

	// write excitation file ------------------------------------------------------
	//writeWavFile(cfg, m_response, dirPath + "/" + Excitation::excitationFileName());

	// revers the buf
	m_filter.resize(m_excitation.size());
	for(int i = 0; i < fileLength/2; i++) {
		m_filter[i] = m_excitation[fileLength -  i - 1];
		m_filter[fileLength -  i - 1] = m_excitation[i];
	}
	// apply decay
	double factor = log2( double(m_newConfig.fMax) / m_newConfig.fMin ) / length;
	for(int i = m_newConfig.rate; i < fileLength; i++) // skip 1 sec
		m_filter[i] *= pow(0.5, factor * (i - m_newConfig.rate));

	// write reverse filter file --------------------------------------------------
	//writeWavFile(cfg, m_inverse, dirPath + "/" + Excitation::filterFileName());

	m_lastConfig = m_newConfig;
}

ExcitCfg& Excitation::newConfig() {
	return m_newConfig;
}

const ExcitCfg& Excitation::lastConfig() const {
	return m_lastConfig;
}

const std::vector<double>& Excitation::excitation() const {
	return m_excitation;
}

const std::vector<double>& Excitation::filter() const {
	return m_filter;
}

void Excitation::writeWavFile(const ExcitCfg& cfg, const std::vector<double>& data, const QString& dirPath) {
	WavInfo wavInfo;
	wavInfo.rate = cfg.rate;
	wavInfo.length = data.size();
	wavInfo.channels = 1;
	wavInfo.bitDepth = cfg.depth;

	WavOut wavOut(dirPath);
	try {
		wavOut.writeDouble(wavInfo, data.data());
	} catch(QLE e) {
		throw QLE(e.msg);
	}
}
