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

#include <cstring>
#include <cmath>
#include "Weights.h"

Weights::Weights(char const* name, int pointsAmount) {
	this->points = 0;

	if( pointsAmount < 2 )
		throw QLE("points amount must be > 2!");
	this->length = pointsAmount;

	// define constants for window
	double a, b;
	if( ! strcmp("hanning", name) ) {
		a = 0.5;
		b = 0.5;
	} else if( ! strcmp("hamming", name) ) {
		a = 0.54;
		b = 0.46;
	} else
		throw QLE("unknown window type!");

	// generate weights
	this->points = new double[pointsAmount];
	double factor = M_PI * 2.0 / (this->length - 1);
	for(int i=0; i < this->length; i++)
		this->points[i] = a - b * cos(factor * i);
}


Weights::~Weights() {
	if(this->points)
		delete this->points;
}

double Weights::getPoint(int pointNum) const {
	if(pointNum < 0 || pointNum > this->length - 1)
		throw QLE("out of index");
	return this->points[pointNum];
}
