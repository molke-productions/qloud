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

#include "Plotter.h"
#include "HarmPlot.h"
#include "Harmonics.h"
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
) : Plotter(parent) {
	this->setAttribute(Qt::WA_DeleteOnClose);

	this->dir = aDir;
	this->ii = anIi;

	this->data = (HarmData**) new char[sizeof(HarmData*) * (MAX_HARM - 1)];

	setTitle(tr("Harmonic Distortion"));

	QLogValueAxis *XAxis = new QLogValueAxis(this->chart);
	XAxis->setBase(10.0);
	XAxis->setLabelFormat("%d");
	XAxis->setTitleText(tr("Frequency in Hz"));
	XAxis->setRange(10, 10000);
	XAxis->setMinorTickCount(8);

	QValueAxis *YAxis = new QValueAxis(this->chart);
	YAxis->setTitleText(tr("Distortion in dB"));
	YAxis->setLabelFormat("%d");
	YAxis->setMax(20);
	YAxis->setMin(-120);
	YAxis->setTickCount(8);
	YAxis->setMinorTickCount(10);

	this->addCurves(XAxis, YAxis);
}

HarmPlot::~HarmPlot() {
	for(int i = 0; i < MAX_HARM - 2; i++) {
		HarmData* data = this->data[i];
		if( data )
			delete data;
	}
	delete this->data;
}

void HarmPlot::addCurves(QAbstractAxis *x, QAbstractAxis *y) {
	Harmonics* harmonics = new Harmonics(this->dir, this->ii);
	for(int i = 0; i <= MAX_HARM - 2; i++) {
		this->data[i] = harmonics->getHarm(i+2); // begin from second harmonic
		if( ! this->data[i])
			continue; // have not more harmonic in IR

		QString name = "Harmonic";
		name += QString::number(i);
		QLineSeries* curve = new QLineSeries(this->chart);
		curve->setPen(QPen(HARM_COLORS[i]));
		if (i == 0)
			appendSeries(curve, x, Qt::AlignBottom, "Hz", y, Qt::AlignLeft, "dB");
		else
			appendSeries(
				curve,
				nullptr, Qt::AlignBottom, "Hz",
				nullptr, Qt::AlignLeft, "dB"
			);

		QList<QPointF> points;
		for(int j = 0; j < this->data[i]->length; j++) {
			points.append(
				QPointF(this->data[i]->freqs[j], this->data[i]->values[j])
			);
		}
		curve->replace(points);
	}
	delete harmonics;
}
