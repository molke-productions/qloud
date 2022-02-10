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

Capture::Capture(QString aDirPath, IAudioIo* audioIo) {
	this->dirPath = aDirPath;
	this->audioIo = audioIo;
}

Capture::~Capture() {
	this->closeAudioIo();
	this->freeBuffers();
	if(this->wavInfo)
		delete this->wavInfo;
}

void Capture::openAudioIo() {
	this->freeBuffers();
}

void Capture::initBuffers() {
	this->freeBuffers();
	WavIn* excitWav = new WavIn(
		this->dirPath + "/" + Excitation::excitationFileName()
	);
	this->wavInfo = excitWav->getWavInfo();
	if(this->wavInfo->rate != this->audioIo->getRate())
		throw QLE("Excitation and JACK sample rates are not equal!");

	try {
		this->playBuf = excitWav->readFloat();
	} catch(QLE e) {
		delete excitWav;
		this->freeBuffers();
		throw QLE(e.msg);
	}

	delete excitWav;
	this->capBuf = new float[this->wavInfo->length];
}

void Capture::doJob(int playDbLevel) {
	if( ! this->jackIsConnected())
		throw QLE("JACK ports are not connected!");
	if( ! this->audioIo->isIdle())
		throw QLE("JACK client isn’t IDLE now!");

	this->maxResponse = 0.0;

	AudioInfo info;
	info.playBuf = this->playBuf;
	info.capBuf = this->capBuf;
	info.length = this->wavInfo->length;
	info.playDb = playDbLevel;

	info.rate = this->audioIo->getRate(); // suppress gcc warning

	this->audioIo->process(info);

	for (unsigned i = 0; i < this->wavInfo->length; ++i) {
		this->maxResponse = std::max(this->maxResponse, fabsf(this->capBuf[i]));
	}

	WavOut* respWav = new WavOut(
		this->dirPath + "/" + Capture::responseFileName()
	);
	respWav->writeFloat(*this->wavInfo, this->capBuf);
	delete respWav;

	this->freeBuffers();
}

bool Capture::jackIsConnected() {
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

//////////////////////////////// privates //////////////////////////////////////
void Capture::closeAudioIo() {
	if(this->audioIo) {
		delete this->audioIo;
		this->audioIo = nullptr;
	}
}

void Capture::freeBuffers() {
	if(this->playBuf) {
		delete this->playBuf;
		this->playBuf = 0;
	}
	if(this->capBuf) {
		delete this->capBuf;
		this->capBuf = 0;
	}
}
