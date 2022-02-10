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

#ifndef PORTAUDIOWRAP_H
#define PORTAUDIOWRAP_H

#include "IAudioIo.h"

#include <vector>

class PortAudioWrap : public IAudioIo {
public:
	PortAudioWrap();
	~PortAudioWrap();

	bool isIdle() override;
	bool isConnected() override;
	void process(AudioInfo info) override;
	int getRate() override;

private:
	std::vector<float> playBuf;
	float* capBuf = nullptr;
	unsigned length = 0;
};

#endif // PORTAUDIOWRAP_H
