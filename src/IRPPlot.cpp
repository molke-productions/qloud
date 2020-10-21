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
#if 0
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#else
#include <QtCharts/QtCharts>
#endif
#include "FileFft.h"
#include "QLUtl.h"
#include "IR.h"
#include "WavIn.h"
#include "WavInfo.h"
#include "IRPPlot.h"

static const QColor BG_COLOR(245, 245, 232);
static const QColor MAJ_PEN_COLOR(175, 175, 152);
static const QColor MIN_PEN_COLOR(175, 175, 152);
static const QColor AMP_CURVE_COLOR(0, 0, 172);
static const QColor AMP_MARKER_COLOR(Qt::black);


IRPPlot::IRPPlot(
	const QString& aDir,
	IRInfo anIi,
	QWidget *parent
) : QChartView(parent) {
	this->dir = aDir;
	this->ii = anIi;

	this->time = 0;
	this->amps = 0;

	unsigned curveLength = this->calculate();

        this->chart = new QChart();
        this->chart->setTitle(tr("IR Power"));
        this->chart->legend()->hide();
        this->setChart(chart);
        this->setRenderHint(QPainter::Antialiasing);

        QValueAxis *XAxis = new QValueAxis(this->chart);
        XAxis->setLabelFormat("%d");
        XAxis->setTitleText(tr("Time in ms"));
        XAxis->setMax(this->time[curveLength-1]);
        XAxis->setMin(this->time[0]);
        XAxis->applyNiceNumbers();
        this->chart->addAxis(XAxis, Qt::AlignBottom);

        QValueAxis *YAxis = new QValueAxis(this->chart);
        YAxis->setTitleText(tr("Power in dB"));
        YAxis->setLabelFormat("%d");
        YAxis->setMax(20);
        YAxis->setMin(-100);
        YAxis->setTickCount(7);
        YAxis->setMinorTickCount(10);
        chart->addAxis(YAxis, Qt::AlignLeft);


        QLineSeries* ampCurve = new QLineSeries(this->chart);
        ampCurve->setPen(QPen(AMP_CURVE_COLOR));
        this->chart->addSeries(ampCurve);
        ampCurve->attachAxis(YAxis);
        ampCurve->attachAxis(XAxis);


        QList<QPointF> points;
        for (unsigned int i = 0; i < curveLength; i++) {
            points.append(QPointF(this->time[i], this->amps[i]));
        }
        ampCurve->replace(points);
}

IRPPlot::~IRPPlot() {
	if(this->time)
		delete this->time;
	if(this->amps)
		delete this->amps;
}

unsigned IRPPlot::calculate() {
	WavIn* irWav = new WavIn(this->dir + "/" + this->ii.key + IR::irFileName());
	try {
		this->amps = irWav->readDouble();
	} catch(QLE e) {
		delete irWav;
		if(this->amps) {
			delete this->amps;
			this->amps = 0;
		}
		throw QLE(e);
	}
	WavInfo* wavInfo = irWav->getWavInfo();
	delete irWav;

	for(unsigned i=0; i < wavInfo->length; i++)
		this->amps[i] = this->amps[i] * this->amps[i];

	// find peak
	unsigned peakIdx = 0;
	for(unsigned i=0; i < wavInfo->length; i++)
		if(this->amps[i] > this->amps[peakIdx])
			peakIdx = i;

	// take from (peak - 0.1sec) to (peak + 1sec)
	int left = int(peakIdx) - int(double(this->ii.rate) * 0.1 + 0.5);
	if(left < 0)
		left = 0;

	int right = int(peakIdx) + int(double(this->ii.rate) * 1.0 + 0.5);
	if(right > int(wavInfo->length) - 1)
		right = int(wavInfo->length) - 1;

	int length = right - left;

	// find peak again to define time zero
	peakIdx = 0;
	for(int i=0; i < length; i++)
		if(this->amps[left + i] > this->amps[left + peakIdx])
			peakIdx = i;

	this->time = new double[length];
	double* newAmps = new double[length];

	for(int i=0; i < length; i++) {
		this->time[i] = 1000.0 * double(i - int(peakIdx)) / wavInfo->rate;
		newAmps[i] = this->amps[left + i];
	}

	delete wavInfo;
	delete this->amps;
	this->amps = newAmps;

	QLUtl::toDbInPlace(this->amps, length, true);

        return unsigned(length);
}
