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

#include "IrsModel.h"
#include "IrsView.h"
#include "QLCfg.h"

IrsForm::IrsForm(
	QWidget* aFeedback,
	QString aWorkDir,
	QWidget* parent
) : QGroupBox(tr("Measurements set"), parent) {
	this->feedback = aFeedback;
	this->workDir = aWorkDir;

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

	this->plotWindow = new PlotWindow(this->workDir, &this->plots);

	QHBoxLayout* hLay = new QHBoxLayout();
	hLay->addWidget(this->view, 1);
	hLay->addWidget(this->plotWindow, 3);
	connect(this->view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &IrsForm::updatePlot);

	setLayout(hLay);
}

IrsForm::~IrsForm() {}

void IrsForm::updatePlot() {
	QModelIndex index = this->view->currentIndex();
	if( (! index.isValid()) || (this->model->rowCount() < 1) ) {
		emit setStatus(tr("Valid selection not found"), 2000);
		return;
	}
	QString irKey = this->model->data(index, Qt::UserRole).toString();

	try {
		QLCfg cfg(this->workDir);
		plotWindow->setIrInfo(cfg.getIr(irKey));
	} catch(QLE e) {
		emit showCritical(e.msg);
	}

	emit setStatus(model->infoString(index.row()));
}

void IrsForm::updateWorkDir(const QString& newDir) {
	this->workDir = newDir;
	emit workDirChanged(newDir);
}

void IrsForm::updateIrList() {
	emit workDirChanged(this->workDir);
}
