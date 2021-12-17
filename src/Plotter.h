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


#ifndef PLOTTER_H
#define PLOTTER_H

#include <QtWidgets>
#include <QtCharts/QtCharts>
#include "QLE.h"
#include "IR.h"
#include "IRInfo.h"

class Plotter: public QChartView {
	Q_OBJECT

public:
	static constexpr double DEFAULT_SMOOTH = 6.0; // 1/6 octave

	Plotter(QWidget *parent = 0);
	~Plotter();

	void setTitle(const QString& title);
	QString getTitle();

	void appendSeries(QLineSeries* series,
			QAbstractAxis* xaxis, Qt::Alignment xalign, const QString &xunit,
			QAbstractAxis* yaxis, Qt::Alignment yalign, const QString &yunit);
	void removeSeries(QLineSeries* series, QAbstractAxis* yattached);

	virtual bool exportSeries(const QString &filename);

	QChart *chart;
	QList<QLineSeries*> list;

protected:
	bool viewportEvent(QEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	double curveYfromX(double x, QLineSeries* series);
	QList<QString> xUnits;
	QList<QString> yUnits;
	QGraphicsLineItem *vLine;
};

#endif
