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

#ifndef QLWIN_H
#define QLWIN_H

#include <QtWidgets>
#include <QDoubleSpinBox>
#include "ExcitForm.h"
#include "Capture.h"
#include "TickPoster.h"
#include "IrsForm.h"

class QLWin : public QMainWindow {
	Q_OBJECT

public:
	static const int BIG_SPACE = 44;
	static const int SMALL_SPACE = 15;
	static QSize rightSize();

	QLWin(QWidget* parent);
	~QLWin();

signals:
	void setStatus(const QString&);
	void setStatus(const QString&, int);
	void clearStatus();
	void workDirChanged(const QString&);
	void jackingStopped();
	void forceRate(int);
	void irAdded();

public slots:
	void showCritical(const QString&);
	void showInfo(const QString&);
	void showStatus(const QString&);
	void showStatus(const QString&, int);
	void changeExcitInfo(const QString&);

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void dirDialog();
	void updateWorkDir(const QString&);

	void startCapture();
	void startJacking();
	void captureFinished();
	void irCalculated();

private:
	QString workDir;
	QComboBox* delayCombo;

	ExcitForm* excit;
	IrsForm* irs;
	Capture* capture;
	TickPoster* ticker;
	QLabel* excitInfoLbl;
    QDoubleSpinBox* playDb;
	QPushButton* capBtn;

	bool jackConnected;

	void saveMyState();
	void restoreMyState();
	void createIrList();
	bool initCapture();
};

#endif
