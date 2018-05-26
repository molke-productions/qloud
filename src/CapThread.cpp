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


#include "CapThread.h"
#include "QLUtl.h"
#include "QLCfg.h"


CapThread::CapThread(QObject* parent, Capture* aCapture, TickPoster* aTicker, int playDb) : QThread(parent) {
	this->capture = aCapture;
	this->ticker = aTicker;
	this->playDbLevel = playDb;
	connect(this, SIGNAL(showCritical(const QString&)), this->parent(), SLOT(showCritical(const QString&)));
	connect(this, SIGNAL(showStatus(const QString&)), this->parent(), SLOT(showStatus(const QString&)));
	connect(this, SIGNAL(showStatus(const QString&, int)), this->parent(), SLOT(showStatus(const QString&, int)));
}


void CapThread::run() {
	try {
		this->capture->doJob(this->playDbLevel);
	} catch(QLE e) {
		emit showCritical(e.msg);
	}
	emit workIsDone(); // for ticker and IR-management

	QString msg("Recording is done. Maximum response level is ");
	msg += QVariant(QLUtl::toDb(this->capture->getMaxResponse())).toString();
	emit showStatus(msg);
}
