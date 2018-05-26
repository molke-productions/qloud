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

#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#include <QtCore>
#include "Capture.h"
#include "TickPoster.h"

class CapThread: public QThread {
	Q_OBJECT

public:
	CapThread(
		QObject* parent,
		Capture* aCapture,
		TickPoster* aPoster,
		int playDb
	);

signals:
	void workIsDone();
	void showCritical(const QString&);
	void showStatus(const QString&);
	void showStatus(const QString&, int);

protected:
	void run();

private:
	Capture* capture;
	TickPoster* ticker;
	int playDbLevel;
};

#endif
