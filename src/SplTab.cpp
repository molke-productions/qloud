/*
	Copyright (C) 2022 Manuel Weichselbaumer <mincequi@web.de>

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

#include "SplTab.h"

#include "QLCfg.h"
#include "SplPlot.h"

SplTab::SplTab(const QString& dir, Plotter** ref, QWidget *parent)
	: QWidget{parent} {
	plotter = new SplPlot(dir);

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
	QSpinBox* cntSmooth = new QSpinBox();
	cntSmooth->setRange(1, 24);
	cntSmooth->setSingleStep(1);
	cntSmooth->setValue(Plotter::DEFAULT_SMOOTH); // 1/6 octave
	QLabel tmp("999");
	cntSmooth->setFixedWidth(
		cntSmooth->sizeHint().width() + tmp.sizeHint().width()
	);
	topLayout->addWidget(cntSmooth, 0);
	connect(cntSmooth, &QSpinBox::valueChanged, plotter, &SplPlot::setSmooth);

	topLayout->addSpacing(15);
	topLayout->addWidget(new QLabel(tr("Window [ms]")));
	cntWindow = new QDoubleSpinBox();
	cntWindow->setDecimals(1);
	cntWindow->setSingleStep(1);
	cntWindow->setValue(500);
	tmp.setText("W25999.0W");
	cntWindow->setFixedWidth(
		cntWindow->sizeHint().width() + tmp.sizeHint().width()
	);
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
	setLayout(splLayout);

	*ref = static_cast<Plotter*>(plotter);
}

void SplTab::init() {
	// s to ms, right window width
	double maxMilliSecs = plotter->getMaxTrimLength() * 1000.0;
	cntWindow->setRange(1.0, maxMilliSecs);
}
