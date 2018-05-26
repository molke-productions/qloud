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

#include <cmath>
#include "JackWrap.h"

// class-friend
int processJackWrap(jack_nframes_t nframes, void *arg) {
	JackWrap* wrap = (JackWrap*)arg;
	return wrap->processJack(nframes);
}

JackWrap::JackWrap() {
	this->client = 0;
	this->fsmState = IDLE;
	this->currentPosition = 0;
	this->maxResponse = 0.0;

	this->client = jack_client_open("qloud", JackNoStartServer, &this->status, 0);
	if ( ! this->client )
		throw QLE("Failed to open JACK client");

	jack_set_process_callback(this->client, processJackWrap, this);

	this->inPort = jack_port_register(
		client, "in",
		JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsInput,
		0
	);
	this->outPort = jack_port_register(
		client,
		"out",
		JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput,
		0
	);

	if(( ! this->inPort) || ( ! this->outPort))
		throw QLE("Failed registering JACK ports!");

	if( jack_activate(this->client) )
		throw QLE("Cannot activate JACK client!");
}

JackWrap::~JackWrap() {
	this->closeClient();
}

bool JackWrap::isIdle() {
	return (this->fsmState == IDLE);
}

bool JackWrap::isConnected() {
	return (
		jack_port_connected(this->inPort) && jack_port_connected(this->outPort)
	);
}

void JackWrap::start(JackInfo info) {
	if( ! this->isConnected() )
		throw QLE("Connect JACK ports before capturing!");
	this->playBuf = info.playBuf;
	this->capBuf = info.capBuf;
	this->playDb = pow(10.0, info.playDb/20.0);
	this->length = info.length;
	this->maxResponse = 0.0;

	this->fsmState = CAPTURE;
}

float JackWrap::getMaxResponse() {
	return this->maxResponse;
}

void JackWrap::closeClient() {
	if(this->client) {
		jack_client_close(this->client);
		this->client = 0;
	}
}

int JackWrap::getRate() {
	if(this->client)
		return int(jack_get_sample_rate(this->client));
	return -1;
}

// private
int JackWrap::processJack(jack_nframes_t nframes) {
	jack_default_audio_sample_t *in, *out;
	if(this->fsmState == IDLE) {
		out = (jack_default_audio_sample_t*)jack_port_get_buffer(this->outPort, nframes);
		for(jack_nframes_t i = 0; i < nframes; i++)
			out[i] = 0.0; // keep silent
		return 0;
	}

	jack_nframes_t samplesToProcess = nframes;

	bool lastBuffer = false;
	if(nframes > this->length -this->currentPosition) {
		samplesToProcess = this->length - this->currentPosition;
		lastBuffer = true;
	}


	in = (jack_default_audio_sample_t*)jack_port_get_buffer(this->inPort, nframes);
	out = (jack_default_audio_sample_t*)jack_port_get_buffer(this->outPort, nframes);

	for(jack_nframes_t i = 0; i < samplesToProcess; i++) {
		out[i] = this->playBuf[this->currentPosition + i] * this->playDb;
		this->capBuf[this->currentPosition + i] = in[i];
		if(fabs(in[i]) > this->maxResponse)
			this->maxResponse = fabs(in[i]);
	}

	this->currentPosition += samplesToProcess;

	if(lastBuffer) {
		this->fsmState = IDLE;
		this->currentPosition = 0;
	}

	return 0;
}
