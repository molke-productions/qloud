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

#ifndef STEPPLOT_H
#define STEPPLOT_H

#include <QtCore>
#include <QtCharts/QtCharts>
#include "QLE.h"
#include "IRInfo.h"
#include "Plotter.h"

class StepPlot: public Plotter {
	Q_OBJECT

public:
	StepPlot(const QString& dir, IRInfo ii, QWidget *parent = 0);
	~StepPlot();

private:
	QString dir;
	IRInfo ii;

	double* time;
	double* amps;

	unsigned calculate();
};

#endif
