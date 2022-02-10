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

#include "AudioIoManager.h"

#include "JackWrap.h"
#include "PortAudioWrap.h"
#include "QLE.h"

AudioIoManager::AudioIoManager() {
	try {
		auto jack = new JackWrap();
		m_backends["JACK"] = jack;
	}  catch (QLE e) {
	}
	try {
		auto pa = new PortAudioWrap();
		m_backends["PortAudio"] = pa;
	}  catch (QLE e) {
	}

	if (m_backends.empty()) {
		throw QLE("Failed to open audio backend. Is JACK running?");
	}

	m_selectedBackend = m_backends.begin()->second;
}

AudioIoManager::~AudioIoManager() {
	for (auto& kv : m_backends) {
		delete kv.second;
	}
}

QStringList AudioIoManager::backends() const {
	QStringList out;
	for (auto& kv : m_backends) {
		out.append(kv.first);
	}

	return out;
}

void AudioIoManager::selectBackend(const QString& backend) {
	auto it = m_backends.find(backend);
	if (it == m_backends.end()) {
		return;
	}

	m_selectedBackend = it->second;
}

bool AudioIoManager::isIdle() {
	return m_selectedBackend->isIdle();
}

bool AudioIoManager::isConnected()  {
	return m_selectedBackend->isConnected();
}

void AudioIoManager::process(AudioInfo info)  {
	m_selectedBackend->process(info);
}

int AudioIoManager::getRate() {
	return m_selectedBackend->getRate();
}
