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

#include "HarmPlot.h"
#include "Harmonics.h"

static const QColor BG_COLOR(245, 245, 232);
static const QColor MAJ_PEN_COLOR(175, 175, 152);
static const QColor MIN_PEN_COLOR(175, 175, 152);
static const QColor CURVE_COLOR(0, 0, 172);
static const QColor HARM_COLORS[] = {
	QColor(0, 172, 0),
	QColor(192, 0, 0),
	QColor(172, 255, 172),
	QColor(255, 152, 152)
};

static const int MAX_HARM = 5;

HarmPlot::HarmPlot(
	const QString& aDir,
	IRInfo anIi,
	QWidget *parent
) : QwtPlot(parent) {
	this->setAttribute(Qt::WA_DeleteOnClose);

	this->dir = aDir;
	this->ii = anIi;

	this->data = (HarmData**) new char[sizeof(HarmData*) * (MAX_HARM - 1)];

	this->setAutoReplot(false);
	this->setCanvasBackground(BG_COLOR);

	this->setAxisScale(QwtPlot::yLeft, -120.0, 20.0);
	this->setAxisMaxMajor(QwtPlot::yLeft, 7);
	this->setAxisMaxMinor(QwtPlot::yLeft, 10);

	this->setAxisMaxMajor(QwtPlot::xBottom, 6);
	this->setAxisMaxMinor(QwtPlot::xBottom, 10);
	QwtLogScaleEngine* logEngine = new QwtLogScaleEngine(10.0);
	this->setAxisScaleEngine(QwtPlot::xBottom, logEngine);

	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->setMajorPen(QPen(MAJ_PEN_COLOR, 0, Qt::DotLine));
	grid->setMinorPen(QPen(MIN_PEN_COLOR, 0 , Qt::DotLine));
	grid->attach(this);

	this->addCurves();

	QwtPlotPanner* panner = new QwtPlotPanner(this->canvas());
	panner->setMouseButton(Qt::MidButton);
	panner->setEnabled(true);

	this->setAutoReplot(true);

}


HarmPlot::~HarmPlot() {
	for( int i = 0; i < MAX_HARM - 2; i++) {
		HarmData* data = this->data[i];
		if( data )
			delete data;
	}
	delete this->data;
}


void HarmPlot::addCurves() {
	Harmonics* harmonics = new Harmonics(this->dir, this->ii);
	for(int i=0; i <= MAX_HARM - 2; i++) {
		this->data[i] = harmonics->getHarm(i+2); // begin from second harmonic
		if( ! this->data[i])
			continue; // have not more harmonic in IR

		QString name = "Harmonic";
		name += i;
		QwtPlotCurve* curve = new QwtPlotCurve(name);
		curve->setPen(QPen(HARM_COLORS[i]));
		curve->setYAxis(QwtPlot::yLeft);
		curve->attach(this);
		curve->setSamples(
			this->data[i]->freqs,
			this->data[i]->values,
			this->data[i]->length
		);
	}
	delete harmonics;
}
