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

#include <sndfile.h>
#include "WavIn.h"

WavIn::WavIn(QString aPath) {
	this->path = aPath;
	this->buf = 0;
	this->sfFile = 0;

	SF_INFO sfInfo;
	this->sfFile = sf_open(this->path.toStdString().c_str(), SFM_READ, &sfInfo);
	if( ! this->sfFile )
		throw QLE("Failed to open " + this->path + "!");

	this->length = sfInfo.frames;
	this->format = sfInfo.format;
	this->rate = sfInfo.samplerate;
	this->channels = sfInfo.channels;
}

WavIn::~WavIn() {
	// buf must be deleted by user!
	if(this->sfFile)
		sf_close(this->sfFile);
}

int WavIn::getLength() {
	return this->length;
}

int WavIn::getRate() {
	return this->rate;
}

int WavIn::getChannels() {
	return this->channels;
}

int WavIn::getBitDepth() {
	if(this->format & SF_FORMAT_PCM_16)
		return 16;
	else if(this->format & SF_FORMAT_PCM_24)
		return 24;
	else if(this->format & SF_FORMAT_PCM_32)
		return 32;
	throw QLE("Undetermined bit depth");
}

float* WavIn::readFloat() {
	this->read( sizeof(float) );
	return (float*)this->buf;
}

double* WavIn::readDouble() {
	this->read( sizeof(double) );
	return (double*)this->buf;
}

WavInfo*  WavIn::getWavInfo() {
	WavInfo* info = new WavInfo();
	info->rate = this->getRate();
	info->length = this->getLength();
	info->bitDepth = this->getBitDepth();
	info->channels = this->getChannels();
	return info;
}

// private
void WavIn::read(int size) {
	sf_count_t wasRead = 0;
	if(size == sizeof(float)) {
		this->buf = new char[this->length * sizeof(float)];
		wasRead = sf_read_float(this->sfFile, (float*)this->buf, this->length);
	} else if(size == sizeof(double)) {
		this->buf = new char[this->length * sizeof(double)];
		wasRead = sf_read_double(this->sfFile, (double*)this->buf, this->length);
	}

	if(wasRead != this->length) {
		sf_close(this->sfFile); // for case destructor isn't called in catch{}
		this->sfFile = 0;
		throw QLE("Failed reading " + this->path + "!");
	}
}
