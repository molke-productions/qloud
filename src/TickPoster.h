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


#ifndef TICKPOSTER_H
#define TICKPOSTER_H

#include <QtWidgets>

class TickPoster : public QWidget {
	Q_OBJECT

public:
	TickPoster(int aMin, int aMax, QWidget* parent = 0);

public slots:
	void startTick();
	void stopTick();

private slots:
	void updateTick();

signals:
	void tickStarted();
	void tickStopped();
	void zero();

private:
	int current;
	int max;

	QLabel* number;
	QTimer* timer;
};

#endif
