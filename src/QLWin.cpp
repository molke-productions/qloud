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

#include <QtWidgets>
#include "QLWin.h"
#include "QLE.h"
#include "QLUtl.h"
#include "TickPoster.h"
#include "CapThread.h"
#include "PlotWindow.h"
#include "QLCfg.h"
#include "ExcitCfg.h"
#include "IR.h"
#include "IRInfo.h"
#include "GenThread.h"

QLWin::QLWin(const QString* wrkDir, QWidget* parent) : QMainWindow(parent) {
	this->workDir = QDir::homePath();

	this->setWindowTitle("QLoud");
	QVBoxLayout* mainLayout = new QVBoxLayout();

	this->ticker = 0;
	this->capture = 0;
	this->jackConnected = false;

	// create WorkDir group
	QGroupBox* wrkGroup = new QGroupBox(tr("All project files are here"));
	QHBoxLayout* wrkLayout = new QHBoxLayout();

	wrkLayout->addWidget(new QLabel(tr("Select working directory")));

	QLineEdit* dirEdit = new QLineEdit();
	dirEdit->setText(QDir::homePath());
	dirEdit->setReadOnly(true);
	wrkLayout->addWidget(dirEdit, 1);
	connect(
			this,
			SIGNAL(workDirChanged(const QString&)),
			dirEdit,
			SLOT(setText(const QString&))
		   );

	QPushButton* dirBtn = new QPushButton("…");
	dirBtn->setMaximumHeight(dirEdit->sizeHint().height() + 1);
	dirBtn->setFixedWidth(QLabel("W…W").sizeHint().width());
	wrkLayout->addWidget(dirBtn);
	connect(dirBtn, SIGNAL(clicked()), this, SLOT(dirDialog()));

	wrkLayout->addSpacing(QLWin::BIG_SPACE);
	QWidget* tmp = new QLabel(tr("<b>Directory</b>"));
	tmp->setFixedWidth(QLWin::rightSize().width());
	wrkLayout->addWidget(tmp);

	wrkGroup->setLayout(wrkLayout);
	mainLayout->addWidget(wrkGroup);

	// Excitation generator has own widget will be added after restoring a state

	int excitLayoutIndex = mainLayout->indexOf(wrkGroup) + 1;

	// create Capture group
	QGroupBox* capGroup = new QGroupBox(tr("Capturing audiosystem response"));
	QVBoxLayout* capLayout = new QVBoxLayout();

	// Capture top
	QHBoxLayout* capTop = new QHBoxLayout();

	capTop->addStretch(2);
	capTop->addWidget(new QLabel(tr("Excitation:")));

	this->excitInfoLbl = new QLabel("");
	capTop->addWidget(this->excitInfoLbl);

	capTop->addSpacing(QLWin::SMALL_SPACE);
	QPushButton* jackBtn = new QPushButton(tr("Connect"));
	jackBtn->setFixedWidth(QPushButton(tr("Disconnect")).sizeHint().width());

	capTop->addSpacing(QLWin::BIG_SPACE);
	tmp = new QLabel(tr("<b>Capture</b>"));
	tmp->setFixedWidth(QLWin::rightSize().width());
	capTop->addWidget(tmp);

	capLayout->addLayout(capTop);

	// Capture bottom
	QHBoxLayout* capBottom = new QHBoxLayout();

	capBottom->addStretch(6);

	capBottom->addWidget(new QLabel(tr("Playback level [dB]")));
	this->playDb = new QDoubleSpinBox();
	this->playDb->setRange(-100, 0);
	this->playDb->setSingleStep(1);
	this->playDb->setValue(-6);
	tmp = new QLabel("W-100W");
	this->playDb->setFixedWidth(
		this->playDb->sizeHint().width() + tmp->sizeHint().width()
	);
	capBottom->addWidget(this->playDb);
	capBottom->addStretch(6);

	capBottom->addWidget(new QLabel(tr("Delay before capture [s]")));

	this->delayCombo = new QComboBox();
	this->delayCombo->setEditable(false);
	this->delayCombo->addItem("1");
	this->delayCombo->addItem("3");
	this->delayCombo->addItem("5");
	this->delayCombo->addItem("10");
	this->delayCombo->addItem("20");
	this->delayCombo->addItem("40");
	this->delayCombo->setCurrentIndex(0);
	capBottom->addWidget(this->delayCombo);

	capBottom->addSpacing(QLWin::BIG_SPACE);
	this->capBtn = new QPushButton(tr("Start recording"));
	this->capBtn->setFixedWidth(QLWin::rightSize().width());
	capBottom->addWidget(this->capBtn);
	connect(this->capBtn, SIGNAL(clicked()), this, SLOT(startCapture()));

	capLayout->addLayout(capBottom);

	capGroup->setLayout(capLayout);
	mainLayout->addWidget(capGroup, 0);

	// Create IR group
	this->createIrList();
	mainLayout->addWidget(this->irs, 1);

	// Layout finishing
	QWidget* centralWidget = new QWidget();
	centralWidget->setLayout(mainLayout);
	this->setCentralWidget(centralWidget);

	// Connect
	connect(
		this,
		SIGNAL(setStatus(const QString&)),
		this->statusBar(),
		SLOT(showMessage(const QString&))
	);
	connect(
		this,
		SIGNAL(setStatus(const QString&, int)),
		this->statusBar(),
		SLOT(showMessage(const QString&, int))
	);
	connect(
		this,
		SIGNAL(clearStatus()),
		this->statusBar(),
		SLOT(clearMessage())
	);

	connect(
		this,
		SIGNAL(workDirChanged(const QString&)),
		this,
		SLOT(updateWorkDir(const QString&))
	);

	// Restore
	this->restoreMyState(wrkDir);

	// workDir is restored, it is safe to add widgets which depends on it
	this->excit = new ExcitForm(this, this->workDir);
	mainLayout->insertWidget( excitLayoutIndex, this->excit, 0);

	this->capBtn->setEnabled(this->initCapture());
	this->excit->generated();
}

QLWin::~QLWin() {
	//this->saveMyState(); - this is done in closeEvent()
}

QSize QLWin::rightSize() {
	QWidget* tmp = new QPushButton(tr("Impulse response"));
	QSize size = tmp->sizeHint();
	delete tmp;
	return size;
}

void QLWin::showCritical(const QString& msg) {
	QLUtl::showCritical(this, msg);
}

void QLWin::showInfo(const QString& msg) {
	QLUtl::showInfo(this, msg);
}

void QLWin::showStatus(const QString& msg, int delay) {
	emit setStatus(msg, delay);
}

void QLWin::showStatus(const QString& msg) {
	emit setStatus(msg);
}

void QLWin::changeExcitInfo(const QString& in) {
	this->excitInfoLbl->setText(in);
	if(in == "NONE") {
		this->capBtn->setEnabled(false);
		return;
	}

	if(this->jackConnected) {
		QLCfg cfg(this->workDir);
		try {
			ExcitCfg eCfg = cfg.getExcit();
			if(eCfg.rate == this->capture->getJackRate())
				this->capBtn->setEnabled(true);
			else {
				this->capBtn->setEnabled(false);
				emit forceRate(this->capture->getJackRate());
			}
		} catch(QLE e) {
			emit showCritical(e.msg);
		}
	}
}

// protected
void QLWin::closeEvent(QCloseEvent * event) {
	this->saveMyState();
	QMainWindow::closeEvent(event);
	QApplication::exit();
}

// private
void QLWin::dirDialog() {
	QDir upDir(this->workDir);
	upDir.cdUp();
	QString s = QFileDialog::getExistingDirectory(
		this,
		"Choose project directory",
		upDir.path(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);
	if(s.length())
		emit workDirChanged(s);
}

void QLWin::saveMyState() {
	QSettings settings("Andrew_Gaydenko", "QLoud");
	settings.beginGroup("MainWindow");
	settings.setValue("workDir", this->workDir);
	settings.setValue("size", this->size());
	settings.setValue("pos", this->pos());
	settings.endGroup();
	settings.sync();
}

void QLWin::restoreMyState(const QString *wrkDir) {
	QSettings settings("Andrew_Gaydenko", "QLoud");
	settings.beginGroup("MainWindow");

	if (wrkDir)
		this->workDir = *wrkDir;
	else
		this->workDir = settings.value("workDir", QDir::homePath()).toString();

	this->resize(settings.value("size", QSize(800, 800)).toSize());
	this->move(settings.value("pos", QPoint(200, 200)).toPoint());
	settings.endGroup();

	QDir dir(this->workDir);
	if( ! dir.exists() ) {
		this->showCritical("Directory " + this->workDir + " doesn’t exist!");
		this->workDir = QDir::homePath();
	}

	emit workDirChanged(this->workDir);
}

void QLWin::createIrList() {
	this->irs = new IrsForm(this, this->workDir);
	connect(
		this,
		SIGNAL(workDirChanged(const QString&)),
		this->irs,
		SLOT(updateWorkDir(const QString&))
	);
	connect(this, SIGNAL(irAdded()), this->irs, SLOT(updateIrList()));
	connect(
		this->irs,
		SIGNAL(showCritical(const QString&)),
		SLOT(showCritical(const QString&))
	);
	connect(this->irs,
		SIGNAL(setStatus(const QString&)),
		SLOT(showStatus(const QString&))
	);
	connect(
		this->irs,
		SIGNAL(setStatus(const QString&, int)),
		SLOT(showStatus(const QString&, int))
	);
}

void QLWin::updateWorkDir(const QString& newDir) {
	this->workDir = newDir;
	emit setStatus("Working dir is: " + this->workDir, 2000);
	emit irAdded();
	if( ! this->jackConnected)
		return;
	if(this->capture)
		delete this->capture;
	this->capBtn->setEnabled(this->initCapture());
	if(this->jackConnected)
		emit forceRate(this->capture->getJackRate());
}

// returns true if JACK is running
bool QLWin::initCapture() {
	try {
		this->capture = new Capture(this->workDir);
		this->capture->openJack();
		this->jackConnected = true;
		emit forceRate(this->capture->getJackRate());
		return true;
	} catch(QLE e) {
		QString s = tr("Failed connecting with JACK server. The error is:\n\n");
		s += e.msg;
		s += tr("\n\nCapturing will be disabled.");
		s += tr("\nTo enable start JACK and restart the application");
		this->showCritical(s);
	}
	return false;
}

void QLWin::startCapture() {
	try {
		this->capture->initBuffers();
	} catch(QLE e) {
		emit showCritical(e.msg);
		return;
	}

	if( ! this->capture->jackIsConnected()) {
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

void QLWin::startJacking() {
	CapThread* cap = new CapThread(
		this,
		this->capture,
		this->ticker,
		this->playDb->value()
	);
	connect(cap, SIGNAL(workIsDone()), this->ticker, SLOT(stopTick()));
	connect(cap, SIGNAL(workIsDone()), this, SLOT(captureFinished()));
	cap->start();
}

void QLWin::captureFinished() {
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
		this, this->workDir, description, maxLevel, this
	);
	gen->start();
}

void QLWin::irCalculated() {
	// TODO - if nothing more, connect directly
	emit irAdded();
}
