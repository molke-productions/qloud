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

#ifndef JACKWRAP_H
#define JACKWRAP_H

#include <vector>
#include <jack/jack.h>
#include "IAudioIo.h"

class JackWrap : public IAudioIo {
public:
	JackWrap();
	~JackWrap() override;

	QStringList inputDevices() const override;
	void selectInputDevice(const QString& device) override;

	QStringList outputDevices() const override;
	void selectOutputDevice(const QString& device) override;

	bool isIdle() override;
	bool isConnected() override;
	void process(const AudioInfo& info) override;
	uint32_t getRate() override;

private:
	enum FSMState {
		IDLE,
		CAPTURE
	};
	volatile FSMState fsmState = IDLE;

	const AudioInfo* audioInfo = nullptr;

	jack_client_t* client = nullptr;
	jack_port_t* inPort;
	jack_port_t* outPort;
	jack_status_t status;
	jack_nframes_t currentPosition = 0;

	void closeClient();
	int processJack(jack_nframes_t nframes);
	friend int processJackWrap(jack_nframes_t nframes, void* arg);
};

#endif
