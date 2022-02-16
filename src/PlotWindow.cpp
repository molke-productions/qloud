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
#include "SplTab.h"

PlotWindow::PlotWindow(
	const QString& dir,
	QMap<PlotWindow*, QString>* aPlots,
	QWidget *parent
) : QWidget(parent) {
	this->dir = dir;
	this->plots = aPlots;

	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setContextMenuPolicy(Qt::NoContextMenu);

	// laying out
	auto* mainLayout = new QHBoxLayout();
	mainLayout->setContentsMargins(2,2,2,2);

	this->tabs = new QTabWidget();
	tabs->setTabPosition(QTabWidget::North);
	splTab = this->buildSplTab(dir, &splplot);
	tabs->addTab(splTab, tr("SPL [dB/Hz]"));
	tabs->addTab(this->buildIRTab(dir, &irplot), tr("IR [amp/ms]"));
	tabs->addTab(this->buildIRPTab(dir, &irpplot), tr("IR power [dB/ms]"));
	tabs->addTab(this->buildStepTab(dir, &stepplot), tr("Step response [amp/ms]"));
	tabs->addTab(this->buildHarmTab(dir, &harmplot), tr("Harmonics [dB/Hz]"));
	this->currentplot = splplot;
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
	mainLayout->addWidget(tabs);

	auto* actions = new QVBoxLayout();
	actions->setAlignment(Qt::AlignBottom);

	QPushButton* exportDat = new QPushButton();
	exportDat->setText(tr("Export"));
	exportDat->setMaximumWidth(exportDat->sizeHint().width());
	connect(exportDat, SIGNAL(clicked()), this, SLOT(onExportClicked()));
	actions->addWidget(exportDat, 1, Qt::AlignRight);

	QPushButton* print = new QPushButton();
	print->setText(tr("Print"));
	connect(print, SIGNAL(clicked()), this, SLOT(onPrintClicked()));
	actions->addWidget(print, 0);

	mainLayout->addLayout(actions, 0);
	this->setLayout(mainLayout);
}

PlotWindow::~PlotWindow() {
}

void PlotWindow::setIrInfo(const IRInfo& ii) {
	this->plots->insert(this, dir + "@" + ii.key);

	QString tit(ii.info + " (" + ii.format());
	tit += ", ";
	tit += ii.maxLevelAsString();
	tit += " dB)";
	this->setWindowTitle(tit);

	splplot->setIrInfo(ii);
	irplot->setIrInfo(ii);
	irpplot->setIrInfo(ii);
	stepplot->setIrInfo(ii);
	harmplot->setIrInfo(ii);

	splTab->init();
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

SplTab* PlotWindow::buildSplTab(
	const QString& dir,
	Plotter **ref
) {
	return new SplTab(dir, ref);
}

IRPlot* PlotWindow::buildIRTab(
	const QString& dir,
	Plotter **ref
) {
	IRPlot* irPlot = new IRPlot(dir);
	*ref = static_cast<Plotter*>(irPlot);
	return irPlot;
}

IRPPlot* PlotWindow::buildIRPTab(
	const QString& dir,
	Plotter **ref
) {
	IRPPlot* irpPlot = new IRPPlot(dir);
	*ref = static_cast<Plotter*>(irpPlot);
	return irpPlot;
}

StepPlot* PlotWindow::buildStepTab(
	const QString& dir,
	Plotter **ref
) {
	StepPlot* stepPlot = new StepPlot(dir);
	*ref = static_cast<Plotter*>(stepPlot);
	return stepPlot;
}

HarmPlot* PlotWindow::buildHarmTab(
	const QString& dir,
	Plotter **ref
) {
	HarmPlot* harmPlot = new HarmPlot(dir);
	*ref = static_cast<Plotter*>(harmPlot);
	return harmPlot;
}
