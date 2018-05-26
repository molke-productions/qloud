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

#include "RoundedZoomer.h"
#include "QLUtl.h"

RoundedZoomer::RoundedZoomer(int xAxis, int yAxis, QWidget* canvas, bool doReplot) :
QwtPlotZoomer(xAxis, yAxis, canvas, doReplot) {
	this->xDig = 2;
	this->yDig = 2;
}

QwtText RoundedZoomer::trackerTextF(const QPointF& pos) const {
	switch(this->rubberBand()) {
		case QwtPicker::HLineRubberBand: {
				QString format = this->getFormat(this->yDig);
				return QString().sprintf(format.toStdString().c_str(), pos.y());
			}
		case QwtPicker::VLineRubberBand: {
				QString format = this->getFormat(this->xDig);
				return QString().sprintf(qPrintable(format), pos.x());
			}
		default: {
				QString format = this->getFormat(this->xDig);
				format += ", ";
				format += this->getFormat(this->yDig);
				return QString().sprintf(qPrintable(format), pos.x(), pos.y());
			}
	}
	return QwtText();
}


void RoundedZoomer::setRound(int anXDig, int anYDig) {
	this->xDig = anXDig;
	this->yDig = anYDig;
}


QString RoundedZoomer::getFormat(int dig) const {
	if(dig < 0)
		dig = 0;
	QString f = "%.";
	f += QVariant(dig).toString();
	f += "f";
	return f;
}

