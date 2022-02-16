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

#ifndef EXCITFORM_H
#define EXCITFORM_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>

class AudioIoManager;
class Capture;
class Excitation;
class TickPoster;

class ExcitForm : public QGroupBox {
	Q_OBJECT

public:
	ExcitForm(QWidget* feedback, QWidget* parent = 0);
	~ExcitForm();

signals:
	void cfgChanged(const QString&);
	void setStatus(const QString&);
	void setStatus(const QString&, int);
	void excitInfoChanged(const QString&);

public slots:
	void showCritical(const QString&);
	void showInfo(const QString&);
	void showStatus(const QString&);
	void showStatus(const QString&, int);
	void forceRate(int);
	void setWorkDir(const QString&);
	void generated();

private slots:
	void lengthChanged(const QString&);
	void rateChanged(const QString&);
	void depthChanged(const QString&);
	void fMinChanged(double);
	void fMaxChanged(double);
	void changeExcitInfo();

	void startCapture();
	void startJacking();
	void captureFinished();

	void onBackendChanged(const QString& text);

private:
	QWidget* feedback;
	QString workDir;

	AudioIoManager* audioIo = nullptr;
	Excitation* excitation = nullptr;
	Capture* capture = nullptr;

	QComboBox* backendCombo = nullptr;
	QComboBox* inputCombo = nullptr;
	QComboBox* outputCombo = nullptr;
	QComboBox* delayCombo = nullptr;
	QComboBox* lengthCombo = nullptr;
	QComboBox* rateCombo = nullptr;
	QComboBox* depthCombo = nullptr;
	QDoubleSpinBox* fMinCnt = nullptr;
	QDoubleSpinBox* fMaxCnt = nullptr;
	QDoubleSpinBox* playDb = nullptr;
	QPushButton* capBtn = nullptr;
	TickPoster* ticker = nullptr;

	bool jackConnected = false;

	QString getInfoString();
	void mapCfgToControls();
	bool initCapture();
};

#endif
