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

#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>

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
) : QwtPlot(parent) {
	this->setAttribute(Qt::WA_DeleteOnClose);

	this->dir = aDir;
	this->ii = anIi;

	this->freqs = 0;
	this->amps = 0;
	this->phase = 0;
	this->phaseCurve = 0;

	this->ir = new IR(dir, ii.key);

	this->setAutoReplot(false);
	this->setCanvasBackground(BG_COLOR);

	this->setAxisScale(QwtPlot::yLeft, -80.0, 20.0);
	this->setAxisMaxMajor(QwtPlot::yLeft, 7);
	this->setAxisMaxMinor(QwtPlot::yLeft, 10);

	if(QLCfg::USE_PAHSE) {
		this->enableAxis(QwtPlot::yRight);
		this->setAxisScale(QwtPlot::yRight, -180.0, 180.0);
	}

	this->setAxisMaxMajor(QwtPlot::xBottom, 6);
	this->setAxisMaxMinor(QwtPlot::xBottom, 10);
	QwtLogScaleEngine* logEngine = new QwtLogScaleEngine(10.0);
	this->setAxisScaleEngine(QwtPlot::xBottom, logEngine);

	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->setMajorPen(QPen(MAJ_PEN_COLOR, 0, Qt::DotLine));
	grid->setMinorPen(QPen(MIN_PEN_COLOR, 0 , Qt::DotLine));
	grid->attach(this);

	// curves
	this->ampCurve = new QwtPlotCurve("Amplitude");
	this->ampCurve->setPen(QPen(AMP_CURVE_COLOR));
	this->ampCurve->setYAxis(QwtPlot::yLeft);
	this->ampCurve->attach(this);

	if(QLCfg::USE_PAHSE) {
		this->phaseCurve = new QwtPlotCurve("Phase");
		this->phaseCurve->setPen(QPen(PHASE_CURVE_COLOR));
		this->phaseCurve->setYAxis(QwtPlot::yRight);
	}

	QwtPlotPanner* panner = new QwtPlotPanner(this->canvas());
	panner->setMouseButton(Qt::MidButton);
	panner->setEnabled(true);

	this->smoothFactor = Plotter::DEFAULT_SMOOTH; // 1/6 octave
	this->winLength = 0.5; // 500 ms for right window
	this->recalculate();
	this->setAutoReplot(true);
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

	if(state)
		this->phaseCurve->attach(this);
	else
		this->phaseCurve->detach();
	this->replot();
}

// private
void Plotter::recalculate() {
	this->setAutoReplot(false);

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

	this->ampCurve->setSamples(
		this->freqs,
		this->amps,
		FileFft::POINTS_AMOUNT
	);
	if(QLCfg::USE_PAHSE)
		this->phaseCurve->setSamples(
			this->freqs,
			this->phase,
			FileFft::POINTS_AMOUNT
		);

	this->setAutoReplot(true);
	this->replot();
}
