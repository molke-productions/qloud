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


#ifndef CAPTURE_H
#define CAPTURE_H

#include <QtCore>
#include "Excitation.h"
#include "WavInfo.h"
#include "QLE.h"

class IAudioIo;

class Capture {
public:
	Capture(IAudioIo* audioIo);
	~Capture();
	static QString responseFileName() {
		return QString("response.wav");
	}
	int getAudioIoRate();
	bool isAudioIoConnected();
	void initBuffers(const std::vector<double>& data, int playDb);
	void doJob();
	float getMaxResponse();
	const std::vector<float>& getCaptureBuffer() const;

private:
	IAudioIo* audioIo = nullptr;

	std::vector<float> playBuf;
	std::vector<float> capBuf;

	float maxResponse = 0.0f;

	void closeAudioIo();
};

#endif
