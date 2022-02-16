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

#ifndef EXCITATION_H
#define EXCITATION_H

#include <vector>
#include <QtCore>
#include "QLCfg.h"
#include "QLE.h"
#include "ExcitCfg.h"

class Excitation : public QObject {
	Q_OBJECT

public:
	Excitation(QObject* parent = nullptr);
	virtual ~Excitation();

	void generate();

	static QString excitationFileName() {
		return QString("excitation.wav");
	}
	static QString filterFileName() {
		return QString("filter.wav");
	}

	void setWorkDir(const QString&);
	ExcitCfg& newConfig();
	const ExcitCfg& lastConfig() const;
	const std::vector<double>& excitation() const;
	const std::vector<double>& filter() const;

private:
	void writeWavFile(const ExcitCfg& cfg, const std::vector<double>& data, const QString& dirPath);

	QLCfg* m_qlCfg = nullptr;
	ExcitCfg m_newConfig;
	ExcitCfg m_lastConfig;
	std::vector<double> m_excitation;
	std::vector<double> m_filter;
};

#endif
