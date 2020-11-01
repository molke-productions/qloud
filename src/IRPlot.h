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

#ifndef IRPLOT_H
#define IRPLOT_H

#include <QtCore>
#include "Plotter.h"
#include "QLE.h"
#include "IRInfo.h"

class IRPlot: public Plotter {
	Q_OBJECT

public:
	IRPlot(const QString& dir, IRInfo ii, QWidget *parent = 0);
	~IRPlot();

private:
	QString dir;
	IRInfo ii;

	double* time;
	double* amps;

	unsigned calculate();
};

#endif
