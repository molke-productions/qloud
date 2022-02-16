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

#include "GenThread.h"
#include "QLCfg.h"
#include "IR.h"
#include "IRInfo.h"

GenThread::GenThread(
	QObject* parent,
	const Excitation& aExcitation,
	const Capture& aCapture,
	const QString& aWrkDir,
	const QString& aDescription,
	double aMaxLevel,
	QWidget* aFeedback
) : QThread(parent),
	excitation(aExcitation),
	capture(aCapture) {
	this->workDir = aWrkDir;
	this->description = aDescription;
	this->maxLevel = aMaxLevel;
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
	connect(
		this,
		SIGNAL(generated()),
		this->feedback,
		SLOT(irCalculated())
	);
}

void GenThread::run() {
	emit showStatus(tr("Calculating IR file …"));
	try {
		QLCfg* qlCfg = new QLCfg(this->workDir);
		QString prefix  = qlCfg->nextIrKey();
		IR* ir = new IR(this->workDir, prefix);
		ir->generate(excitation, capture);
		delete ir;

		ExcitCfg eCfg = qlCfg->getExcit();
		IRInfo ii;
		ii.fill(eCfg);
		ii.key = prefix;
		ii.info = this->description;
		ii.maxLevel = this->maxLevel;
		qlCfg->appendIr(ii);

		delete qlCfg;
	} catch(QLE e) {
		QString msg = tr("IR-file calculation failed:\n\n");
		msg += e.msg;
		emit showCritical(msg);
		return;
	}
	emit showStatus(tr("IR-file calculated successfully."), 2000);
	emit generated();
}
