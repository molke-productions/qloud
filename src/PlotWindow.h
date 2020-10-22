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


#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QtWidgets>
#include <QPrinter>
#include "QLE.h"
#include "IRInfo.h"
#include "Plotter.h"

class PlotWindow : public QWidget {
	Q_OBJECT

public:
	PlotWindow(
		const QString& dir,
		const IRInfo& ii,
		QMap<PlotWindow*, QString>* plots,
		QWidget *parent = 0
	);
	~PlotWindow();

public slots:
    void onPrintClicked();
    void onTabChanged(int index);

protected:
    bool print(QPrinter *printer);

private:
	QMap<PlotWindow*, QString>* plots;

	// SPL plotting
    QWidget* getSplTab(const QString& dir, const IRInfo& ii, QChartView** plot);
	// IR itself plotting
    QWidget* getIRTab(const QString& dir, const IRInfo& ii, QChartView** plot);
	// IR power plotting
    QWidget* getIRPTab(const QString& dir, const IRInfo& ii, QChartView** plot);
	// Step Response plotting
    QWidget* getStepTab(const QString& dir, const IRInfo& ii, QChartView** plot);
	// Harmonics plotting
    QWidget* getHarmTab(const QString& dir, const IRInfo& ii, QChartView** plot);

    QChartView *splplot;
    QChartView *irplot;
    QChartView *irpplot;
    QChartView *stepplot;
    QChartView *harmplot;

    QChartView *currentplot;
};

#endif
