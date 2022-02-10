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

#ifndef AUDIOIOMANAGER_H
#define AUDIOIOMANAGER_H

#include <map>
#include <QObject>
#include <QStringList>

#include "IAudioIo.h"

class AudioIoManager : public QObject, public IAudioIo {
	Q_OBJECT

public:
	AudioIoManager();
	virtual ~AudioIoManager();

	QStringList backends() const;
	void selectBackend(const QString& backend);

	QStringList inputDevices() const override;
	void selectInputDevice(const QString& device) override;

	QStringList outputDevices() const override;
	void selectOutputDevice(const QString& device) override;

	bool isIdle() override;
	bool isConnected() override;
	void process(const AudioInfo& info) override;
	uint32_t getRate() override;

private:
	std::map<QString, IAudioIo*> m_backends;
	IAudioIo* m_selectedBackend = nullptr;
};

#endif // AUDIOIOMANAGER_H
