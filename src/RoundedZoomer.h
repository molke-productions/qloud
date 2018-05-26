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

#ifndef ROUNDEDZOOMER_H
#define ROUNDEDZOOMER_H

#include <QtWidgets>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>

class RoundedZoomer : public QwtPlotZoomer {
	Q_OBJECT

public:
	RoundedZoomer(int xAxis, int yAxis, QWidget*, bool doReplot=true);
	void setRound(int xDig, int yDig);

protected:
	QwtText trackerTextF(const QPointF& pos) const;

private:
	int xDig;
	int yDig;

	QString getFormat(int dig) const;
};

#endif
