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

#include <QtCharts/QtCharts>

#include "IR.h"
#include "FileFft.h"
#include "Plotter.h"
#include "QLUtl.h"
#include "QLCfg.h"

static const QColor BG_COLOR(245, 245, 232);
static const QColor MAJ_PEN_COLOR(175, 175, 152);
static const QColor MIN_PEN_COLOR(175, 175, 152);
static const QColor AMP_CURVE_COLOR(0, 0, 172);
static const QColor PHASE_CURVE_COLOR(0, 150, 0);

Plotter::Plotter(
	const QString& aDir,
	IRInfo anIi,
	QWidget *parent
) : QChartView(parent) {
	this->setAttribute(Qt::WA_DeleteOnClose);

	this->dir = aDir;
	this->ii = anIi;

	this->freqs = 0;
	this->amps = 0;
	this->phase = 0;
	this->phaseCurve = 0;

	this->ir = new IR(dir, ii.key);

	chart = new QChart();
	chart->setTitle(tr("Frequency Response"));
	chart->legend()->hide();
	this->setChart(chart);
	this->setRenderHint(QPainter::Antialiasing);

	XAxis = new QLogValueAxis(this->chart);
	((QLogValueAxis*) XAxis)->setBase(10.0);
	((QLogValueAxis*) XAxis)->setLabelFormat("%d");
	XAxis->setTitleText(tr("Frequency in Hz"));
	XAxis->setRange(10, 100000);
	((QLogValueAxis *) XAxis)->setMinorTickCount(8);
	chart->addAxis(XAxis, Qt::AlignBottom);

	YAxis = new QValueAxis(this->chart);
	YAxis->setTitleText(tr("Amplitude in dB"));
	((QValueAxis *) YAxis)->setLabelFormat("%d");
	YAxis->setMax(20);
	YAxis->setMin(-100);
	((QValueAxis *) YAxis)->setTickCount(7);
	((QValueAxis *) YAxis)->setMinorTickCount(10);
	chart->addAxis(YAxis, Qt::AlignLeft);

	if(QLCfg::USE_PAHSE) {
		YPAxis = new QValueAxis(this->chart);
		YPAxis->setTitleText(tr("Phase in degrees"));
		((QValueAxis *) YPAxis)->setLabelFormat("%d");
		YPAxis->setMax(180);
		YPAxis->setMin(-180);
		chart->addAxis(YPAxis, Qt::AlignRight);
	}

	// curves
	this->ampCurve = new QLineSeries(this->chart);
	this->chart->addSeries(this->ampCurve);

	this->ampCurve->setPen(QPen(AMP_CURVE_COLOR));
	this->ampCurve->attachAxis(XAxis);
	this->ampCurve->attachAxis(YAxis);

	if(QLCfg::USE_PAHSE) {
		this->phaseCurve = new QLineSeries(this->chart);
		this->phaseCurve->setPen(QPen(PHASE_CURVE_COLOR));
	}

	this->smoothFactor = Plotter::DEFAULT_SMOOTH; // 1/6 octave
	this->winLength = 0.5; // 500 ms for right window
	this->recalculate();

	this->setRubberBand(QChartView::HorizontalRubberBand);
}

Plotter::~Plotter() {
	if(this->freqs)
		delete this->freqs;
	if(this->amps)
		delete this->amps;
	if(this->phase)
		delete this->phase;
	if(this->ir)
		delete ir;
	if(this->phaseCurve)
		delete this->phaseCurve; // may be detached
}

double Plotter::getMaxTrimLength() {
	return this->ir->getMaxTrimLength();
}

void Plotter::setSmooth(double aSmoothFactor) {
	this->smoothFactor = aSmoothFactor;
	this->recalculate();
}

void Plotter::setWinLength(double msecs) {
	this->winLength = msecs / 1000.0;
	this->recalculate();
}

void Plotter::enablePhase(int state) {
	if( ! QLCfg::USE_PAHSE)
		return;

	if(state) {
		this->chart->addSeries(phaseCurve);
		this->phaseCurve->attachAxis(this->XAxis);
		this->phaseCurve->attachAxis(this->YPAxis);
	} else {
		this->chart->removeSeries(phaseCurve);
	}
}

// private
void Plotter::recalculate() {
	this->ir->trim(this->winLength);

	FileFft* ff = new FileFft(
		this->dir + "/" + this->ii.key + IR::trimmedIrFileName(), this->ii
	);

	if(this->freqs)
		delete this->freqs;
	this->freqs = ff->getFreqs();

	if(this->amps)
		delete this->amps;
	this->amps = ff->getAmps(1.0 / this->smoothFactor);

	if(QLCfg::USE_PAHSE) {
		if(this->phase)
			delete this->phase;
		this->phase = ff->getPhase(1.0 / this->smoothFactor);
	}

	delete ff;

	QList<QPointF> points;
	for (int i = 0; i < FileFft::POINTS_AMOUNT; ++i) {
		points.append(QPointF(this->freqs[i], this->amps[i]));
	}
	this->ampCurve->replace(points);

	if(QLCfg::USE_PAHSE) {
		QList<QPointF> ppoints;
		for (int i = 0; i < FileFft::POINTS_AMOUNT; ++i) {
			ppoints.append(QPointF(this->freqs[i], this->phase[i]));
		}
		this->phaseCurve->replace(ppoints);
	}
}
