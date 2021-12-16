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

#include "IrsForm.h"
#include "QLWin.h"
#include "QLUtl.h"
#include "QLCfg.h"
#include "PlotWindow.h"
#include "IrsView.h"

IrsForm::IrsForm(
	QWidget* aFeedback,
	QString aWorkDir,
	QWidget* parent
) : QWidget(parent) {
	this->feedback = aFeedback;
	this->workDir = aWorkDir;

	QVBoxLayout* mainLayout = new QVBoxLayout();
	QGroupBox* group = new QGroupBox(tr("Measurements set"));
	QHBoxLayout* hLay = new QHBoxLayout();

	this->model = new IrsModel(this->workDir, &this->plots);
	connect(
		this,
		SIGNAL(workDirChanged(const QString&)),
		this->model,
		SLOT(updateWorkDir(const QString&))
	);
	connect(
		this->model,
		SIGNAL(showCritical(const QString&)),
		this->feedback,
		SLOT(showCritical(const QString&))
	);
	connect(
		this->model,
		SIGNAL(showStatus(const QString&, int)),
		this->feedback,
		SLOT(showStatus(const QString&, int))
	);

	this->view = new IrsView(this->model);
	connect(
		this->view,
		SIGNAL(showCritical(const QString&)),
		this->feedback,
		SLOT(showCritical(const QString&))
	);

	hLay->addWidget(this->view, 1);
	hLay->addSpacing(QLWin::BIG_SPACE);

	QVBoxLayout* vLay = new QVBoxLayout();
	vLay->addStretch(1);

	QWidget* lbl = new QLabel(tr("<b>Response</b>"));
	lbl->setFixedWidth(QLWin::rightSize().width());
	vLay->addWidget(lbl);

	this->btnPlot = new QPushButton(tr("Plot"));
	this->btnPlot->setFixedWidth(QLWin::rightSize().width());
	vLay->addWidget(this->btnPlot);

	hLay->addLayout(vLay);

	group->setLayout(hLay);
	mainLayout->addWidget(group);
	this->setLayout(mainLayout);
	this->layout()->setContentsMargins(0,0,0,0);

	connect(this->btnPlot, SIGNAL(clicked()), this, SLOT(newPlot()));
}

IrsForm::~IrsForm() {}

void IrsForm::newPlot() {
	QModelIndex index = this->view->currentIndex();
	if( (! index.isValid()) || (this->model->rowCount() < 1) ) {
		emit setStatus(tr("Valid selection not found"), 2000);
		return;
	}
	QString irKey = this->model->data(index, Qt::UserRole).toString();

	try {
		QLCfg cfg(this->workDir);
		PlotWindow* w = new PlotWindow(
			this->workDir,
			cfg.getIr(irKey),
			&this->plots
		);
		w->show();
	} catch(QLE e) {
		emit showCritical(e.msg);
	}
}

void IrsForm::updateWorkDir(const QString& newDir) {
	this->workDir = newDir;
	emit workDirChanged(newDir);
}

void IrsForm::updateIrList() {
	emit workDirChanged(this->workDir);
}
