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

#include "ExcitForm.h"

#include "AudioIoManager.h"
#include "Capture.h"
#include "CapThread.h"
#include "Excitation.h"
#include "GenThread.h"
#include "QLUtl.h"
#include "TickPoster.h"

ExcitForm::ExcitForm(
	QWidget* aFeedback,
	QWidget* parent
) : QGroupBox(tr("Parameters for measurement"), parent) {
	this->feedback = aFeedback;

	this->excitation = new Excitation(this);;

	try {
		this->audioIo = new AudioIoManager();
	} catch(QLE e) {
		QString s = tr("Failed to init audio backends. The error is:\n\n");
		s += e.msg;
		s += tr("\n\nCapturing will be disabled.");
		this->showCritical(s);
	}

	// Layout
	auto* topLayout = new QHBoxLayout();

	// Audio system
	topLayout->addWidget(new QLabel(tr("Backend")));
	backendCombo = new QComboBox();
	backendCombo->addItems(this->audioIo->backends());
	connect(backendCombo, &QComboBox::currentTextChanged, this, &ExcitForm::onBackendChanged);
	topLayout->addWidget(backendCombo);

	// Audio input/output
	topLayout->addWidget(new QLabel(tr("Input")));
	inputCombo = new QComboBox();
	inputCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	inputCombo->setEnabled(false);
	topLayout->addWidget(inputCombo);
	topLayout->addWidget(new QLabel(tr("Output")));
	outputCombo = new QComboBox();
	outputCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	outputCombo->setEnabled(false);
	topLayout->addWidget(outputCombo);

	// Sample rate
	topLayout->addWidget(new QLabel(tr("Sample rate [Hz]")));
	this->rateCombo = new QComboBox();
	this->rateCombo->setEditable(false);
	this->rateCombo->addItem("44100");
	this->rateCombo->addItem("48000");
	this->rateCombo->addItem("88200");
	this->rateCombo->addItem("96000");
	this->rateCombo->addItem("176400");
	this->rateCombo->addItem("192000");
	this->rateCombo->setCurrentIndex(1);
	topLayout->addWidget(this->rateCombo);
	connect(
		this->rateCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(rateChanged(const QString&))
	);

	// Bit depth
	topLayout->addWidget(new QLabel(tr("Bit depth")));
	this->depthCombo = new QComboBox();
	this->depthCombo->setEditable(false);
	this->depthCombo->addItem("16");
	this->depthCombo->addItem("24");
	this->depthCombo->addItem("32");
	this->depthCombo->setCurrentIndex(2);
	topLayout->addWidget(this->depthCombo);
	connect(
		this->depthCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(depthChanged(const QString&))
	);

	// Spacer at end
	topLayout->addStretch();

	auto* bottomLayout = new QHBoxLayout();

	// Measurement duration
	bottomLayout->addWidget(new QLabel(tr("Duration [s]")));
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
	bottomLayout->addWidget(this->lengthCombo);
	connect(
		this->lengthCombo,
		SIGNAL(currentTextChanged(const QString&)),
		this,
		SLOT(lengthChanged(const QString&))
	);

	// Measurement delay
	bottomLayout->addWidget(new QLabel(tr("Delay [s]")));
	this->delayCombo = new QComboBox();
	this->delayCombo->setEditable(false);
	this->delayCombo->addItem("1");
	this->delayCombo->addItem("2");
	this->delayCombo->addItem("5");
	this->delayCombo->addItem("10");
	this->delayCombo->addItem("20");
	this->delayCombo->addItem("50");
	this->delayCombo->setCurrentIndex(0);
	bottomLayout->addWidget(this->delayCombo);

	// Min Frequency
	bottomLayout->addWidget(new QLabel(tr("Min. freq. [Hz]")));
	this->fMinCnt = new QDoubleSpinBox(this);
	this->fMinCnt->setRange(1, 16000);
	this->fMinCnt->setSingleStep(1);
	this->fMinCnt->setValue(100);
	this->fMinCnt->setDecimals(0);
	bottomLayout->addWidget(this->fMinCnt);
	connect(
		this->fMinCnt,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(fMinChanged(double))
	);

	// Max Frequency
	bottomLayout->addWidget(new QLabel(tr("Max. freq. [Hz]")));
	this->fMaxCnt = new QDoubleSpinBox(this);
	this->fMaxCnt->setRange(1, 20000);
	this->fMaxCnt->setSingleStep(1);
	this->fMaxCnt->setValue(10000);
	this->fMaxCnt->setDecimals(0);
	bottomLayout->addWidget(this->fMaxCnt);
	connect(
		this->fMaxCnt,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(fMaxChanged(double))
	);

	// Playback level
	bottomLayout->addWidget(new QLabel(tr("Playback level [dB]")));
	this->playDb = new QDoubleSpinBox();
	this->playDb->setRange(-96, 0);
	this->playDb->setSingleStep(1);
	this->playDb->setValue(-6);
	this->playDb->setDecimals(0);
	bottomLayout->addWidget(this->playDb);

	// Measure button
	this->capBtn = new QPushButton(tr("Start recording"));
	connect(this->capBtn, SIGNAL(clicked()), this, SLOT(startCapture()));
	this->capBtn->setEnabled(this->initCapture());

	// Assign layout to group
	auto* layout = new QGridLayout();
	layout->addLayout(topLayout, 0, 0, 1, 3);
	layout->addLayout(bottomLayout, 1, 0, 1, 1);
	layout->addWidget(this->capBtn, 1, 2, 1, 1);
	layout->setColumnStretch(1, 1);

	setLayout(layout);

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
		this,
		SLOT(changeExcitInfo())
	);
}

ExcitForm::~ExcitForm() {
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

	try {
		this->excitation->setWorkDir(dir);
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
		this->excitation->newConfig().rate = newRate;
	else
        this->showCritical(tr("Failed accepting JACK rate!"));
}

void ExcitForm::generated() {
	emit excitInfoChanged(this->getInfoString());
}

// private
void ExcitForm::lengthChanged(const QString& in) {
	this->excitation->newConfig().length = QVariant(in).toInt();
}

void ExcitForm::rateChanged(const QString& in) {
	this->excitation->newConfig().rate = QVariant(in).toInt();
}

void ExcitForm::depthChanged(const QString& in) {
	this->excitation->newConfig().depth = QVariant(in).toInt();
}

void ExcitForm::fMinChanged(double in) {
	this->excitation->newConfig().fMin = int(in + 0.5);
}

void ExcitForm::fMaxChanged(double in) {
	this->excitation->newConfig().fMax = int(in + 0.5);
}

void ExcitForm::changeExcitInfo() {
	if(this->jackConnected) {
		QLCfg cfg(this->workDir);
		try {
			ExcitCfg eCfg = cfg.getExcit();
			if(eCfg.rate == this->capture->getAudioIoRate())
				this->capBtn->setEnabled(true);
			else {
				this->capBtn->setEnabled(false);
				emit forceRate(this->capture->getAudioIoRate());
			}
		} catch(QLE e) {
			emit showCritical(e.msg);
		}
	}
}

void ExcitForm::startCapture() {
	try {
		this->excitation->generate();
		this->capture->initBuffers(this->excitation->excitation(), this->playDb->value());
	} catch(QLE e) {
		emit showCritical(e.msg);
		return;
	}

	if( ! this->capture->isAudioIoConnected()) {
		emit showCritical(tr("Connect JACK ports before capturing!"));
		return;
	}

	int delay = QVariant(this->delayCombo->currentText()).toInt();
	if(this->ticker)
		delete this->ticker;
	// TODO: replace '300' with something
	this->ticker = new TickPoster(-delay, 300);
	this->ticker->move(this->pos() + QPoint(10, 10));
	this->ticker->show();

	connect(ticker, SIGNAL(zero()), this, SLOT(startJacking()));
	this->ticker->startTick();
}

void ExcitForm::startJacking() {
	CapThread* cap = new CapThread(
		this,
		this->capture,
		this->ticker
	);
	connect(cap, SIGNAL(workIsDone()), this->ticker, SLOT(stopTick()));
	connect(cap, SIGNAL(workIsDone()), this, SLOT(captureFinished()));
	cap->start();
}

void ExcitForm::captureFinished() {
	QString stdDescription = QDate::currentDate().toString(Qt::ISODate);
	stdDescription = QDate::currentDate().toString(Qt::ISODate);
	stdDescription += "-" + QTime::currentTime().toString();

	double maxLevel = QLUtl::toDb(this->capture->getMaxResponse());
	bool ok;
	QString msg = tr("Maximum capture level: ");
	msg += QVariant(maxLevel).toString() + " dB";
	msg += tr("\nGive a meaningful description for the measurement");
	msg += tr("\n(“Cancel” will ignore this measurement)");
	QString tmp = tr("Measurement description");
	QString description = QInputDialog::getText(
		this, tmp, msg, QLineEdit::Normal, stdDescription, &ok
	);
	if( ! ok)
		return;
	// save this measurement
	if( ! description.length() )
		description = stdDescription;
	GenThread* gen = new GenThread(
						 this,
						 *this->excitation,
						 *this->capture,
						 this->workDir,
						 description,
						 maxLevel,
						 this->feedback
	);
	gen->start();
}

void ExcitForm::onBackendChanged(const QString& text) {
	audioIo->selectBackend(text);

	inputCombo->clear();
	inputCombo->addItems(audioIo->inputDevices());
	inputCombo->setEnabled(inputCombo->count());

	outputCombo->clear();
	outputCombo->addItems(audioIo->outputDevices());
	outputCombo->setEnabled(outputCombo->count());
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

	return this->excitation->lastConfig().toString();
}

void ExcitForm::mapCfgToControls() {
	QLUtl::setComboToData(this->lengthCombo, this->excitation->newConfig().length);
	QLUtl::setComboToData(this->rateCombo, this->excitation->newConfig().rate);
	QLUtl::setComboToData(this->depthCombo, this->excitation->newConfig().depth);
	this->fMinCnt->setValue(this->excitation->newConfig().fMin);
	this->fMaxCnt->setValue(this->excitation->newConfig().fMax);
}

// returns true if JACK is running
bool ExcitForm::initCapture() {
	this->capture = new Capture(this->audioIo);
	this->jackConnected = true;
	emit forceRate(this->capture->getAudioIoRate());
	return !this->audioIo->backends().isEmpty();
}
