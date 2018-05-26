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


#include "ExcitThread.h"
#include "Excitation.h"


ExcitThread::ExcitThread(
	QObject* parent,
	const QString& aWrkDir,
	const ExcitCfg& aCfg,
	QWidget* aFeedback
) : QThread(parent) {
	this->wrkDir = aWrkDir;
	this->cfg = aCfg;
	this->feedback = aFeedback;
	connect(
		this,
		SIGNAL(showStatus(const QString&)),
		this->feedback,
		SLOT(showStatus(const QString&))
	);
	connect(
		this,
		SIGNAL(showStatus(const QString&, int)),
		this->feedback,
		SLOT(showStatus(const QString&, int))
	);
	connect(
		this,
		SIGNAL(showCritical(const QString&)),
		this->feedback,
		SLOT(showCritical(const QString&))
	);
	connect(this, SIGNAL(generated()), this->feedback, SLOT(generated()));
}

void ExcitThread::run() {
	emit showStatus("Generating excitation …");
	try {
		Excitation::generate( this->wrkDir, this->cfg);
		emit generated();
	} catch(QLE e) {
		emit showCritical(e.msg);
		emit showStatus("Excitation generating failed!", 2000);
		return;
	}
	emit showStatus("Excitation and inverse filter are generated!", 2000);
}
