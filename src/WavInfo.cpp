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
#include <QtCore>
#include "QLUtl.h"
#include "WavInfo.h"

WavInfo::WavInfo() {}

WavInfo::~WavInfo() {}

void WavInfo::show() {
    QLUtl::d(QString("Length: ") + QString::number(this->length));
    QLUtl::d(QString("Rate: ") + QString::number(this->rate));
    QLUtl::d(QString("Bit depth: ") + QString::number(this->bitDepth));
}

int WavInfo::getFormat() {
	if(this->bitDepth == 16)
		return SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	else if(this->bitDepth == 24)
		return SF_FORMAT_WAV | SF_FORMAT_PCM_24;
	else if(this->bitDepth == 32)
		return SF_FORMAT_WAV | SF_FORMAT_PCM_32;
	throw QLE("Unsupported format");
}
