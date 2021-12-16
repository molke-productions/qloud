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

#include "Excitation.h"
#include "ExcitForm.h"
#include "ExcitThread.h"
#include "QLUtl.h"
#include "QLWin.h"

ExcitForm::ExcitForm(
	QWidget* aFeedback,
	QString aWorkDir,
	QWidget* parent
) : QWidget(parent) {
	this->feedback = aFeedback;

	// Layout
	QVBoxLayout* mainLayout = new QVBoxLayout();

	QGroupBox* exGroup = new QGroupBox(tr("Parameters for excitation signal"));
	QVBoxLayout* exLayout = new QVBoxLayout();

	// Excitation top
	QHBoxLayout* exTop = new QHBoxLayout();

	exTop->addWidget(new QLabel(tr("Length [s]")));
	this->lengthCombo = new QComboBox();
	this->lengthCombo->setEditable(false);
	this->lengthCombo->addItem("3");
	this->lengthCombo->addItem("5");
	this->lengthCombo->addItem("7");
	this->lengthCombo->addItem("10");
	this->lengthCombo->addItem("15");
	this->lengthCombo->addItem("25");
	this->lengthCombo->addItem("40");
	this->lengthCombo->setCurrentIndex(2);
	exTop->addWidget(this->lengthCombo);
	connect(
		this->lengthCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(lengthChanged(const QString&))
	);

	exTop->addStretch(1);
	exTop->addSpacing(QLWin::SMALL_SPACE);
	exTop->addWidget(new QLabel(tr("Sample rate [Hz]")));
	this->rateCombo = new QComboBox();
	this->rateCombo->setEditable(false);
	this->rateCombo->addItem("32000");
	this->rateCombo->addItem("44100");
	this->rateCombo->addItem("48000");
	this->rateCombo->addItem("64000");
	this->rateCombo->addItem("88200");
	this->rateCombo->addItem("96000");
	this->rateCombo->addItem("128000");
	this->rateCombo->addItem("176400");
	this->rateCombo->addItem("192000");
	this->rateCombo->setCurrentIndex(2);
	exTop->addWidget(this->rateCombo);
	connect(
		this->rateCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(rateChanged(const QString&))
	);

	exTop->addStretch(1);
	exTop->addSpacing(QLWin::SMALL_SPACE);
	exTop->addWidget(new QLabel(tr("Bit depth")));
	this->depthCombo = new QComboBox();
	this->depthCombo->setEditable(false);
	this->depthCombo->addItem("16");
	this->depthCombo->addItem("24");
	this->depthCombo->addItem("32");
	this->depthCombo->setCurrentIndex(2);
	exTop->addWidget(this->depthCombo);
	connect(
		this->depthCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(depthChanged(const QString&))
	);

	exTop->addSpacing(QLWin::BIG_SPACE);
	QWidget* excitLabel = new QLabel(tr("<b>Excitation</b>"));
	excitLabel->setFixedWidth(QLWin::rightSize().width());
	exTop->addWidget(excitLabel);

	exLayout->addLayout(exTop);

	// Excitation bottom
	QHBoxLayout* exBottom = new QHBoxLayout();

	exBottom->addWidget(new QLabel(tr("Min. freq. [Hz]")));
	this->fMinCnt = new QDoubleSpinBox(this);
	this->fMinCnt->setRange(1, 100000);
	this->fMinCnt->setSingleStep(1);
	this->fMinCnt->setValue(100);
	QWidget* tmp = new QLabel("W100000W");
	this->fMinCnt->setFixedWidth(
		this->fMinCnt->sizeHint().width() + tmp->sizeHint().width()
	);
	delete tmp;
	exBottom->addWidget(this->fMinCnt, 5);
	connect(
		this->fMinCnt,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(fMinChanged(double))
	);

	exBottom->addStretch(1);
	exBottom->addSpacing(QLWin::SMALL_SPACE);

	exBottom->addWidget(new QLabel(tr("Max. freq. [Hz]")));
	this->fMaxCnt = new QDoubleSpinBox(this);
	this->fMaxCnt->setRange(1, 100000);
	this->fMaxCnt->setSingleStep(1);
	this->fMaxCnt->setValue(10000);
	tmp = new QLabel("W1000000W");
	this->fMaxCnt->setFixedWidth(
		this->fMaxCnt->sizeHint().width() + tmp->sizeHint().width()
	);
	delete tmp;
	exBottom->addWidget(this->fMaxCnt, 5);
	connect(
		this->fMaxCnt,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(fMaxChanged(double))
	);

	exBottom->addSpacing(QLWin::BIG_SPACE);
	QPushButton* genBtn = new QPushButton(tr("Generate"));
	genBtn->setFixedWidth(QLWin::rightSize().width());
	exBottom->addWidget(genBtn);
	connect(genBtn, SIGNAL(clicked()), this, SLOT(generate()));

	exLayout->addLayout(exBottom);

	exGroup->setLayout(exLayout);
	mainLayout->addWidget(exGroup);
	this->setLayout(mainLayout);
	this->layout()->setContentsMargins(0,0,0,0);

	// Signals
	connect(
		this,
		SIGNAL(setStatus(const QString&)),
		this->feedback,
		SLOT(showStatus(const QString&))
	);
	connect(
		this,
		SIGNAL(setStatus(const QString&, int)),
		this->feedback,
		SLOT(showStatus(const QString&, int))
	);
	connect(
		this->feedback,
		SIGNAL(forceRate(int)),
		this,
		SLOT(forceRate(int)),
		Qt::QueuedConnection
	);
	connect(
		this->feedback,
		SIGNAL(workDirChanged(const QString&)),
		this,
		SLOT(setWorkDir(const QString&))
	);
	connect(
		this,
		SIGNAL(excitInfoChanged(const QString&)),
		this->feedback,
		SLOT(changeExcitInfo(const QString&))
	);

	this->setWorkDir(aWorkDir);
}

ExcitForm::~ExcitForm() {
	delete this->qlCfg;
}

void ExcitForm::showCritical(const QString& msg) {
	QLUtl::showCritical(this, msg);
}

void ExcitForm::showInfo(const QString& msg) {
	QLUtl::showInfo(this, msg);
}

void ExcitForm::showStatus(const QString& msg, int delay) {
	emit setStatus(msg, delay);
}

void ExcitForm::showStatus(const QString& msg) {
	emit setStatus(msg);
}

void ExcitForm::setWorkDir(const QString& dir) {
	this->workDir = dir;
	this->qlCfg = new QLCfg(this->workDir);
	try {
		this->lastCfg = this->qlCfg->getExcit();
		this->newCfg = this->lastCfg;
		this->mapCfgToControls();
	} catch(QLE e) {
		emit showCritical(e.msg);
		return;
	}

	emit excitInfoChanged(this->getInfoString());
}

void ExcitForm::forceRate(int newRate) {
	this->rateCombo->setEnabled(false);
	if(QLUtl::setComboToData(this->rateCombo, newRate))
		this->newCfg.rate = newRate;
	else
        this->showCritical(tr("Failed accepting JACK rate!"));
}

void ExcitForm::generated() {
	emit excitInfoChanged(this->getInfoString());
}

// private
void ExcitForm::lengthChanged(const QString& in) {
	this->newCfg.length = QVariant(in).toInt();
}

void ExcitForm::rateChanged(const QString& in) {
	this->newCfg.rate = QVariant(in).toInt();
}

void ExcitForm::depthChanged(const QString& in) {
	this->newCfg.depth = QVariant(in).toInt();
}

void ExcitForm::fMinChanged(double in) {
	this->newCfg.fMin = int(in + 0.5);
}

void ExcitForm::fMaxChanged(double in) {
	this->newCfg.fMax = int(in + 0.5);
}

void ExcitForm::generate() {
	try {
		this->newCfg.check();
		this->lastCfg = this->newCfg;
		this->qlCfg->setExcit(this->lastCfg);
		ExcitThread* eThread = new ExcitThread(
			this,
			this->workDir,
			this->lastCfg,
			this
		);
		eThread->start(QThread::LowestPriority);
		emit excitInfoChanged(this->getInfoString());
	} catch(QLE e) {
		QLUtl::showCritical(this, e.msg);
	}
}

QString ExcitForm::getInfoString() {
	QFile e(this->workDir + "/" + Excitation::excitationFileName());
	bool exists = e.exists();
	QLUtl::checkFileError(e);
	QFile f(this->workDir + "/" + Excitation::filterFileName());
	exists &= f.exists();
	QLUtl::checkFileError(f);

	if( ! exists)
		return "NONE";

	return this->lastCfg.toString();
}

void ExcitForm::mapCfgToControls() {
	QLUtl::setComboToData(this->lengthCombo, this->newCfg.length);
	QLUtl::setComboToData(this->rateCombo, this->newCfg.rate);
	QLUtl::setComboToData(this->depthCombo, this->newCfg.depth);
	this->fMinCnt->setValue(this->newCfg.fMin);
	this->fMaxCnt->setValue(this->newCfg.fMax);
}
