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

class IRPlot;
class IRPPlot;
class StepPlot;
class HarmPlot;
class SplTab;

class PlotWindow : public QWidget {
	Q_OBJECT

public:
	PlotWindow(
		const QString& dir,
		QMap<PlotWindow*, QString>* plots,
		QWidget *parent = 0
	);
	~PlotWindow();

	void setIrInfo(const IRInfo& ii);

public slots:
	void onPrintClicked();
	void onExportClicked();

	void onTabChanged(int index);

protected:
	bool print(QPrinter *printer);
	bool exportDat(const QString& filename);

private:
	QMap<PlotWindow*, QString>* plots;

	QTabWidget* tabs;
	SplTab* splTab;

	// SPL plotting
	static SplTab* buildSplTab(const QString& dir, Plotter** plot);
	// IR itself plotting
	static IRPlot* buildIRTab(const QString& dir, Plotter** plot);
	// IR power plotting
	static IRPPlot* buildIRPTab(const QString& dir, Plotter** plot);
	// Step response plotting
	static StepPlot* buildStepTab(const QString& dir, Plotter** plot);
	// Harmonics plotting
	static HarmPlot* buildHarmTab(const QString& dir, Plotter** plot);

	Plotter *splplot;
	Plotter *irplot;
	Plotter *irpplot;
	Plotter *stepplot;
	Plotter *harmplot;

	Plotter *currentplot;
	IRInfo ii;
	QString dir;
};

#endif
