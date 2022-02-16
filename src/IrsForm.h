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

#ifndef IRSFORM_H
#define IRSFORM_H

#include <QGroupBox>
#include <QPushButton>
#include <QTableView>

class IrsModel;
class PlotWindow;

class IrsForm : public QGroupBox {
	Q_OBJECT

public:
	IrsForm(QWidget* feedback, QString workDir, QWidget* parent = 0);
	~IrsForm();

signals:
	void setStatus(const QString&);
	void setStatus(const QString&, int);
	void workDirChanged(const QString&);
	void showCritical(const QString&);

private slots:
	void updatePlot();
	void updateWorkDir(const QString&);
	void updateIrList();

private:
	QWidget* feedback;
	QString workDir;
	IrsModel* model;
	QTableView* view;
	QPushButton* btnPlot;
	PlotWindow* plotWindow;
	QMap<PlotWindow*,QString> plots;
};

#endif
