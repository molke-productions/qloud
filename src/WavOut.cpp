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
#include "WavOut.h"

WavOut::WavOut(QString aPath) {
	this->path = aPath;
}

WavOut::~WavOut() {}

void WavOut::writeFloat(WavInfo info, float* buf) {
	this->write(info, sizeof(float), (char*)buf);
}

void WavOut::writeDouble(WavInfo info, double* buf) {
	this->write(info, sizeof(double), (char*)buf);
}

// private
void WavOut::write(WavInfo wavInfo, int size, char* buf) {
	SF_INFO sfInfo;
	sfInfo.channels = wavInfo.channels;
	sfInfo.format = wavInfo.getFormat();
	sfInfo.samplerate = wavInfo.rate;

	SNDFILE *sfFile = sf_open(
		this->path.toStdString().c_str(),
		SFM_WRITE,
		&sfInfo
	);
	if( ! sfFile )
		throw QLE("failed to open " + this->path + " for writing!");

	sf_count_t wasWritten = 0;
	if(size == sizeof(float))
		wasWritten = sf_write_float(sfFile, (float*)buf, wavInfo.length);
	else if(size == sizeof(double))
		wasWritten = sf_write_double(sfFile, (double*)buf, wavInfo.length);
	sf_close(sfFile);

	if(wavInfo.length != wasWritten)
		throw QLE("failed to write " + this->path + "!");
}

