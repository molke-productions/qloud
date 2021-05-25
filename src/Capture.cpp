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

#include <unistd.h>

#include "Excitation.h"
#include "Capture.h"
#include "JackWrap.h"
#include "WavIn.h"
#include "WavOut.h"

Capture::Capture(QString aDirPath) {
	this->dirPath = aDirPath;
	this->jack = 0;
	this->playBuf = 0;
	this->capBuf = 0;
	this->wavInfo = 0;
	this->maxResponse = 0.0f;
}

Capture::~Capture() {
	this->closeJack();
	this->freeBuffers();
	if(this->wavInfo)
		delete this->wavInfo;
}

void Capture::openJack() {
	if(this->jack)
		throw QLE("JACK client is already opened!");
	this->freeBuffers();

	this->jack = new JackWrap();
}

void Capture::initBuffers() {
	this->freeBuffers();
	WavIn* excitWav = new WavIn(
		this->dirPath + "/" + Excitation::excitationFileName()
	);
	this->wavInfo = excitWav->getWavInfo();
	if(this->wavInfo->rate != this->jack->getRate())
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
	if( ! this->jack->isIdle())
		throw QLE("JACK client isn’t IDLE now!");

	this->maxResponse = 0.0;

	JackInfo info;
	info.playBuf = this->playBuf;
	info.capBuf = this->capBuf;
	info.length = this->wavInfo->length;
	info.playDb = playDbLevel;

	info.rate = this->jack->getRate(); // suppress gcc warning

	this->jack->start(info);

	while( ! this->jack->isIdle() )
		usleep(100000); // 0.1 sec

	this->maxResponse = this->jack->getMaxResponse();

	WavOut* respWav = new WavOut(
		this->dirPath + "/" + Capture::responseFileName()
	);
	respWav->writeFloat(*this->wavInfo, this->capBuf);
	delete respWav;

	this->freeBuffers();
}

void Capture::closeJack() {
	if(this->jack) {
		delete this->jack;
		this->jack = 0;
	}
}

bool Capture::jackIsConnected() {
	if(this->jack )
		return this->jack->isConnected();
	return false;
}

float Capture::getMaxResponse() {
	return this->maxResponse;
}

int Capture::getJackRate() {
	if(this->jack)
		return this->jack->getRate();
	return -1;
}

//////////////////////////////// privates //////////////////////////////////////
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
