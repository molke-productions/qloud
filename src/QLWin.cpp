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

#include "QLWin.h"

#include "ExcitForm.h"
#include "IrsForm.h"
#include "QLUtl.h"

QLWin::QLWin(const QString* wrkDir, QWidget* parent) : QMainWindow(parent) {
	this->workDir = QDir::homePath();

	this->setWindowTitle("QLoud");
	QVBoxLayout* mainLayout = new QVBoxLayout();

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

	wrkGroup->setLayout(wrkLayout);
	mainLayout->addWidget(wrkGroup);

	// Excitation generator has own widget will be added after restoring a state
	int excitLayoutIndex = mainLayout->indexOf(wrkGroup) + 1;

	// Create IR group
	this->createIrList();
	mainLayout->addWidget(this->irs, 1);

	// Layout finishing
	mainLayout->setSpacing(24);
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
	this->excit = new ExcitForm(this);
	this->excit->setWorkDir(this->workDir);
	mainLayout->insertWidget(excitLayoutIndex, this->excit, 0);
	connect(
		this,
		SIGNAL(workDirChanged(const QString&)),
		this->excit,
		SLOT(setWorkDir(const QString&))
	);

	this->excit->generated();
}

QLWin::~QLWin() {
	//this->saveMyState(); - this is done in closeEvent()
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
}

void QLWin::irCalculated() {
	// TODO - if nothing more, connect directly
	emit irAdded();
}
