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
#include <QtCharts/QtCharts>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QPainter>

#include "PlotWindow.h"
#include "Plotter.h"
#include "QLUtl.h"
#include "QLCfg.h"
#include "SplPlot.h"
#include "IRPlot.h"
#include "IRPPlot.h"
#include "StepPlot.h"
#include "HarmPlot.h"

PlotWindow::PlotWindow(
	const QString& dir,
	const IRInfo& ii,
	QMap<PlotWindow*, QString>* aPlots,
	QWidget *parent
) : QWidget(parent) {
	this->dir = dir;
	this->ii = ii;
	this->plots = aPlots;
	this->plots->insert(this, dir + "@" + ii.key);

	this->setAttribute(Qt::WA_DeleteOnClose);
	QString tit(ii.info + " (" + ii.format());
	tit += ", ";
	tit += ii.maxLevelAsString();
	tit += " dB)";
	this->setWindowTitle(tit);
	this->setContextMenuPolicy(Qt::NoContextMenu);

	// laying out
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(2,2,2,2);

	QTabWidget* tab = new QTabWidget();
	tab->setTabPosition(QTabWidget::North);
	tab->addTab(this->getSplTab(dir, ii, &splplot), tr("SPL [dB/Hz]"));
	this->currentplot = splplot;
	tab->addTab(this->getIRTab(dir, ii, &irplot), tr("IR [amp/ms]"));
	tab->addTab(this->getIRPTab(dir, ii, &irpplot), tr("IR power [dB/ms]"));
	tab->addTab(
		this->getStepTab(dir, ii, &stepplot),
		tr("Step response [amp/ms]")
	);
	tab->addTab(this->getHarmTab(dir, ii, &harmplot), tr("Harmonics [dB/Hz]"));
	connect(tab, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
	mainLayout->addWidget(tab);

	QHBoxLayout* actions = new QHBoxLayout();

	QPushButton* exportDat = new QPushButton();
	exportDat->setText(tr("Export"));
	exportDat->setMaximumWidth(exportDat->sizeHint().width());
	connect(exportDat, SIGNAL(clicked()), this, SLOT(onExportClicked()));
	actions->addWidget(exportDat, 1, Qt::AlignRight);

	QPushButton* print = new QPushButton();
	print->setText(tr("Print"));
	print->setMaximumWidth(print->sizeHint().width());
	connect(print, SIGNAL(clicked()), this, SLOT(onPrintClicked()));
	actions->addWidget(print, 0);

	mainLayout->addLayout(actions, 0);
	this->setLayout(mainLayout);
}

PlotWindow::~PlotWindow() {
	this->plots->remove(this);
}

void PlotWindow::onTabChanged(int index) {
	switch (index) {
		case 0: currentplot = splplot; break;
		case 1: currentplot = irplot; break;
		case 2: currentplot = irpplot; break;
		case 3: currentplot = stepplot; break;
		case 4: currentplot = harmplot; break;
		default: currentplot = nullptr;
	}
}

void PlotWindow::onPrintClicked() {
	if (!currentplot)
		return;

	QPrinter printer;
	printer.setCreator("QLoud");
	printer.setDocName("qloud");
	printer.setPageOrientation(QPageLayout::Landscape);
	QPrintDialog dialog(&printer, this);
	if (dialog.exec() == QDialog::Accepted) {
		if (printer.isValid()) {
			if (!print(&printer)) {
				QPrinterInfo info(printer);
				qWarning() << "printerinfo definition null:"
					<< info.isNull();
				qWarning() << "printerinfo state error:"
					<< (info.state() == QPrinter::Error);
			}
		} else
			qWarning() << "invalid printer object";
	}
}

bool PlotWindow::print(QPrinter* printer) {
	QPainter painter;
	if (!painter.begin(printer))
		return false;

	QRect page = printer->pageLayout().paintRectPixels(printer->resolution());
	currentplot->render(
		&painter,
		QRectF(page.left(), page.top(), page.width(), page.height())
	);
	painter.end();
	return true;
}

void PlotWindow::onExportClicked() {
	if (!currentplot)
		return;

	QString home = QStandardPaths::writableLocation(
		QStandardPaths::HomeLocation
	);
	QString f = QString("QLoud-%1").arg(currentplot->getTitle());
	f.replace(" ", "");
	QString fileName = QFileDialog::getSaveFileName(
		this,
		tr("Export"),
		home + QDir::separator() + f + ".dat",
		tr("*.dat")
	);
	exportDat(fileName);
}

bool PlotWindow::exportDat(const QString& filename) {
	return currentplot->exportSeries(filename);
}

QWidget* PlotWindow::getSplTab(
	const QString& dir,
	const IRInfo& ii,
	Plotter **ref
) {
	SplPlot* plotter = new SplPlot(dir, ii);

	QWidget* splWidget = new QWidget();
	QVBoxLayout* splLayout = new QVBoxLayout();

	// buttons and counters
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addSpacing(15);

	QLabel* hlp = new QLabel("<b>?</b>");
	QString tip = tr("Mouse using:\n");
	tip += tr("Left button drag – zoom in\n");
	tip += tr("Right button click – zoom out");
	hlp->setToolTip(tip);
	topLayout->addWidget(hlp);

	topLayout->addStretch(1);

	topLayout->addSpacing(15);
	topLayout->addWidget(new QLabel(tr("Octave smoothing, 1/x")));
	QDoubleSpinBox* cntSmooth = new QDoubleSpinBox();
	cntSmooth->setRange(0.25, 256.0);
	cntSmooth->setSingleStep(0.25);
	cntSmooth->setValue(Plotter::DEFAULT_SMOOTH); // 1/6 octave
	QWidget* tmp = new QLabel("W9999.99W");
	cntSmooth->setFixedWidth(
		cntSmooth->sizeHint().width() + tmp->sizeHint().width()
	);
	delete tmp;
	topLayout->addWidget(cntSmooth, 0);
	connect(
		cntSmooth,
		SIGNAL(valueChanged(double)),
		plotter,
		SLOT(setSmooth(double))
	);

	topLayout->addSpacing(15);
	topLayout->addWidget(new QLabel(tr("Window [ms]")));
	QDoubleSpinBox* cntWindow = new QDoubleSpinBox();
	// s to ms, right window width
	double maxMilliSecs = plotter->getMaxTrimLength() * 1000.0;
	cntWindow->setRange(1.0, maxMilliSecs);
	cntWindow->setSingleStep(1);
	cntWindow->setValue(500);
	tmp = new QLabel("W25999.0W");
	cntWindow->setFixedWidth(
		cntWindow->sizeHint().width() + tmp->sizeHint().width()
	);
	delete tmp;
	topLayout->addWidget(cntWindow);
	connect(
		cntWindow,
		SIGNAL(valueChanged(double)),
		plotter,
		SLOT(setWinLength(double))
	);

	if(QLCfg::USE_PHASE) {
		topLayout->addSpacing(15);
		QCheckBox* phaseCheck = new QCheckBox(tr("Phase"));
		phaseCheck->setChecked(false);
		topLayout->addWidget(phaseCheck, 0);
		connect(
			phaseCheck,
			SIGNAL(stateChanged(int)),
			plotter,
			SLOT(enablePhase(int))
		);
	}

	topLayout->addSpacing(15);
	splLayout->addLayout(topLayout, 0);

	// plotter itself
	splLayout->addWidget(plotter, 1);

	// layout final
	splLayout->setContentsMargins(1,1,1,1);
	splWidget->setLayout(splLayout);

	*ref = static_cast<Plotter*>(plotter);
	return splWidget;
}

QWidget* PlotWindow::getIRTab(
	const QString& dir,
	const IRInfo& ii,
	Plotter **ref
) {
	QWidget* irPlot = new IRPlot(dir, ii);
	*ref = static_cast<Plotter*>(irPlot);
	return irPlot;
}

QWidget* PlotWindow::getIRPTab(
	const QString& dir,
	const IRInfo& ii,
	Plotter **ref
) {
	QWidget* irpPlot = new IRPPlot(dir, ii);
	*ref = static_cast<Plotter*>(irpPlot);
	return irpPlot;
}

QWidget* PlotWindow::getStepTab(
	const QString& dir,
	const IRInfo& ii,
	Plotter **ref
) {
	QWidget* stepPlot = new StepPlot(dir, ii);
	*ref = static_cast<Plotter*>(stepPlot);
	return stepPlot;
}

QWidget* PlotWindow::getHarmTab(
	const QString& dir,
	const IRInfo& ii,
	Plotter **ref
) {
	QWidget* harmPlot = new HarmPlot(dir, ii);
	*ref = static_cast<Plotter*>(harmPlot);
	return harmPlot;
}
