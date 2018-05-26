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


#include "TickPoster.h"

TickPoster::TickPoster(int aMin, int aMax, QWidget* parent) : QWidget(parent) {
	this->max = aMax;
	this->current = aMin;

	QFont defaultFont = QApplication::font(this);
	this->setFixedWidth(400);

	QPalette pal;
	pal.setColor( QPalette::Window, QColor(255, 255, 243));
	pal.setColor( QPalette::WindowText, QColor(0, 119, 0));
	this->setPalette(pal);

	QFont font;
	font.setPointSize(120);
	font.setBold(true);
	this->setFont(font);

	QVBoxLayout* vbl = new QVBoxLayout();

	this->number = new QLabel();
	this->number->setAlignment(Qt::AlignHCenter);
	this->number->setText("...");

	vbl->addWidget(this->number, 1);

	QPushButton* closeBtn = new QPushButton("Stop");
	closeBtn->setFont(defaultFont);
	connect(closeBtn, SIGNAL(clicked()), this, SLOT(stopTick()));

	this->setLayout(vbl);

	this->timer = new QTimer(this);

	connect(this->timer, SIGNAL(timeout()), this, SLOT(updateTick()));
	connect(this, SIGNAL(tickStopped()), this->timer, SLOT(stop()));
}

void TickPoster::startTick() {
	this->timer->start(1000);
	emit tickStarted();
	this->show();
}

void TickPoster::stopTick() {
	emit tickStopped();
	this->close();
}

void TickPoster::updateTick() {
	this->number->setNum(this->current);
	this->repaint();
	if( ! this->current) {
		emit zero();
		QPalette pal;
		pal.setColor( QPalette::Window, QColor(255, 255, 243));
		pal.setColor( QPalette::WindowText, QColor(211, 0, 0));
		this->setPalette(pal);
	}
	this->current++;
	if(this->current > this->max) {
		emit tickStopped();
		this->close();
	}
}
