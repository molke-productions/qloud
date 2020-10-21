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

#include "PlotWindow.h"
#include "Plotter.h"
#include "QLUtl.h"
#include "QLCfg.h"
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
	mainLayout->setMargin(2);
	QTabWidget* tab = new QTabWidget();
	tab->setTabPosition(QTabWidget::North);
	tab->addTab(this->getSplTab(dir, ii), "SPL [dB/Hz]");
	tab->addTab(this->getIRTab(dir, ii), "IR [amp/ms]");
	tab->addTab(this->getIRPTab(dir, ii), "IR power [dB/ms]");
	tab->addTab(this->getStepTab(dir, ii), "Step response [amp/ms]");
	tab->addTab(this->getHarmTab(dir, ii), "Harmonics [dB/Hz]");
	mainLayout->addWidget(tab);
	this->setLayout(mainLayout);
}

PlotWindow::~PlotWindow() {
	this->plots->remove(this);
}

QWidget* PlotWindow::getSplTab(
	const QString& dir,
	const IRInfo& ii
) {
	Plotter* plotter = new Plotter(dir, ii);
        //this->zoomizePlotter(plotter, 1, 3);
	if(QLCfg::USE_PAHSE) {
#if 0
		// as we have additional curve/scale (phase), add a zoomer
		RoundedZoomer* zoomer2 = new RoundedZoomer(
			QwtPlot::xTop,
			QwtPlot::yRight,
			plotter->canvas()
		);
		zoomer2->setRound(1, 3);
		zoomer2->setTrackerMode(QwtPicker::AlwaysOff);
		zoomer2->setRubberBand(QwtPicker::NoRubberBand);
		zoomer2->setMousePattern(
			QwtEventPattern::MouseSelect2,
			Qt::RightButton,
			Qt::ControlModifier
		);
		zoomer2->setMousePattern(
			QwtEventPattern::MouseSelect3,
			Qt::RightButton
		);
		zoomer2->setEnabled(true);
		zoomer2->zoom(0);
#endif
	}

	QWidget* splWidget = new QWidget();
	QVBoxLayout* splLayout = new QVBoxLayout();

	// buttons and counters
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addSpacing(15);

	QLabel* hlp = new QLabel("<b>?</b>");
	QString tip = "Mouse using:\n";
	tip += "Left button drag – zoom in\n";
	tip += "Right button click – go back in zoom history\n";
	tip += "Shift + middle button click – go forward in zoom history\n";
	tip += "Ctrl + right button click – go to zoom history start\n";
	tip += "Middle button drag – move window";
	hlp->setToolTip(tip);
	topLayout->addWidget(hlp);

	topLayout->addStretch(1);

	topLayout->addSpacing(15);
	topLayout->addWidget(new QLabel("Octave smoothing, 1/x"));
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
	topLayout->addWidget(new QLabel("Window [ms]"));
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

	if(QLCfg::USE_PAHSE) {
		topLayout->addSpacing(15);
		QCheckBox* phaseCheck = new QCheckBox("Phase");
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
	splLayout->setMargin(1);
	splWidget->setLayout(splLayout);
	return splWidget;
}

QWidget* PlotWindow::getIRTab(
	const QString& dir,
	const IRInfo& ii
) {
        QWidget* irPlot = new IRPlot(dir, ii);
        //this->zoomizePlotter(irPlot, 2, 3);
	return irPlot;
}

QWidget* PlotWindow::getIRPTab(
	const QString& dir,
	const IRInfo& ii
) {
        QWidget* irpPlot = new IRPPlot(dir, ii);
        //this->zoomizePlotter(irpPlot, 2, 3);
	return irpPlot;
}

QWidget* PlotWindow::getStepTab(
	const QString& dir,
	const IRInfo& ii
) {
        QWidget* stepPlot = new StepPlot(dir, ii);
        //this->zoomizePlotter(stepPlot, 2, 3);
	return stepPlot;
}

QWidget* PlotWindow::getHarmTab(
	const QString& dir,
	const IRInfo& ii
) {
        QWidget* harmPlot = new HarmPlot(dir, ii);
        //this->zoomizePlotter(harmPlot, 1, 3);
	return harmPlot;
}

#if 0
void PlotWindow::zoomizePlotter(
	QwtPlot* plotter,
	int roundX,
	int roundY
) {
	RoundedZoomer* zoomer1 = new RoundedZoomer(
		QwtPlot::xBottom,
		QwtPlot::yLeft,
                plotter->canvas()
	);
	zoomer1->setRound(roundX, roundY);
	zoomer1->setRubberBand(QwtPicker::RectRubberBand);
	zoomer1->setRubberBandPen(QColor(192, 0, 0));
	zoomer1->setTrackerMode(QwtPicker::AlwaysOn);
	zoomer1->setTrackerPen(QColor(0, 0, 0));
	zoomer1->setMousePattern(
		QwtEventPattern::MouseSelect2,
		Qt::RightButton,
		Qt::ControlModifier
	);
	zoomer1->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer1->setEnabled(true);
	zoomer1->zoom(0);
}
#endif

