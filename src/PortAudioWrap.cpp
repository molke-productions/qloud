/*
	Copyright (C) 2022 Manuel Weichselbaumer <mincequi@web.de>

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

#include "PortAudioWrap.h"

#include <sstream>

#include <portaudio.h>

#include "QLE.h"

#define SAMPLE_RATE (44100)
#define SAMPLE_SIZE (4)
#define FRAMES_PER_BUFFER (256)

PortAudioWrap::PortAudioWrap() {
	auto err = Pa_Initialize();
	if (err != paNoError) {
		std::stringstream ss;
		ss << "Failed to init PortAudio: " << Pa_GetErrorText(err);
		throw QLE(ss.str().c_str());
	}
}

PortAudioWrap::~PortAudioWrap() {
	Pa_Terminate();
}

QStringList PortAudioWrap::inputDevices() const {
	QStringList out;
	auto numDevices = Pa_GetDeviceCount();
	for (PaDeviceIndex i = 0; i < numDevices; ++i) {
		auto deviceInfo = Pa_GetDeviceInfo(i);
		if (deviceInfo->maxInputChannels != 0) {
			out.push_back(deviceInfo->name);
		}
	}
	return out;
}

void PortAudioWrap::selectInputDevice(const QString& /*device*/) {
}

QStringList PortAudioWrap::outputDevices() const {
	QStringList out;
	auto numDevices = Pa_GetDeviceCount();
	for (PaDeviceIndex i = 0; i < numDevices; ++i) {
		auto deviceInfo = Pa_GetDeviceInfo(i);
		if (deviceInfo->maxOutputChannels != 0) {
			out.push_back(deviceInfo->name);
		}
	}
	return out;
}

void PortAudioWrap::selectOutputDevice(const QString& /*device*/) {
}

bool PortAudioWrap::isIdle() {
	return true;
}

bool PortAudioWrap::isConnected() {
	return true;
}

void PortAudioWrap::process(const AudioInfo& info) {
	uint i = 0;
	PaStream *stream;
	/* Open an audio I/O stream. */
	auto err = Pa_OpenDefaultStream(
				   &stream,
				   1,          /* mono input */
				   1,          /* mono output */
				   paFloat32,  /* 32 bit floating point output */
				   SAMPLE_RATE,	/* sample rate */
				   FRAMES_PER_BUFFER, //paFramesPerBufferUnspecified,
				   NULL,	/* no callback, use blocking API */
				   NULL);	/* no callback, so no callback userData */
	//processPortAudioWrap,
	//this); /* Pointer that will be passed to the process callback */

	if (err != paNoError)
		goto error;

	err = Pa_StartStream(stream);
	if (err != paNoError)
		goto error;

	for (i = 0; i < info.playBuf.size()/FRAMES_PER_BUFFER; ++i) {
		// You may get underruns or overruns if the output is not primed by PortAudio.
		err = Pa_WriteStream(stream, info.playBuf.data() + i * FRAMES_PER_BUFFER, FRAMES_PER_BUFFER);
		if (err)
			goto error;
		err = Pa_ReadStream(stream, info.capBuf.data() + i * FRAMES_PER_BUFFER, FRAMES_PER_BUFFER);
		if (err)
			goto error;
	}

	err = Pa_StopStream(stream);
	if (err != paNoError)
		goto error;

	err = Pa_CloseStream(stream);
	if (err != paNoError)
		goto error;

	return;

error:
	if (stream) {
		Pa_AbortStream(stream);
		Pa_CloseStream(stream);
	}

	throw QLE(QString("Failed to open stream: ") + Pa_GetErrorText(err));
}

uint32_t PortAudioWrap::getRate() {
	return 44100;
}
