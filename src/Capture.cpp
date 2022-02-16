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

#include "Capture.h"

#include "Excitation.h"
#include "IAudioIo.h"
#include "WavIn.h"
#include "WavOut.h"

Capture::Capture(IAudioIo* audioIo) {
	this->audioIo = audioIo;
}

Capture::~Capture() {
	this->closeAudioIo();
}

void Capture::initBuffers(const std::vector<double>& data, int playDb) {
	this->playBuf.resize(data.size());
	this->capBuf.resize(data.size());

	const float playDb_ = pow(10.0f, playDb/20.0f);
	for (uint i = 0; i < data.size(); ++i) {
		playBuf[i] = data[i] * playDb_;
	}
}

void Capture::doJob() {
	if( ! this->isAudioIoConnected())
		throw QLE("JACK ports are not connected!");
	if( ! this->audioIo->isIdle())
		throw QLE("JACK client isn’t IDLE now!");

	AudioInfo info{ this->playBuf, this->capBuf, this->audioIo->getRate() };
	this->audioIo->process(info);

	// Compute max response
	this->maxResponse = 0.0;
	for (auto& d : info.capBuf) {
		this->maxResponse = std::max(this->maxResponse, fabsf(d));
	}
}

bool Capture::isAudioIoConnected() {
	if(this->audioIo )
		return this->audioIo->isConnected();
	return false;
}

float Capture::getMaxResponse() {
	return this->maxResponse;
}

int Capture::getAudioIoRate() {
	if(this->audioIo)
		return this->audioIo->getRate();
	return -1;
}

const std::vector<float>& Capture::getCaptureBuffer() const {
	return this->capBuf;
}

//////////////////////////////// privates //////////////////////////////////////
void Capture::closeAudioIo() {
	if(this->audioIo) {
		delete this->audioIo;
		this->audioIo = nullptr;
	}
}
