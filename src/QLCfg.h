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

#ifndef QLCFG_H
#define QLCFG_H

#include <QtCore>
#include <QtXml/QDomDocument>

#include "QLE.h"
#include "IRInfo.h"
#include "ExcitCfg.h"

class QLCfg {

public:
	static const bool USE_PAHSE = true;
	static const int INDENT = 4;

	QLCfg(QString aPath);
	~QLCfg();

	void appendIr(IRInfo info);
	void removeIr(QString key);
	void replaceIr(IRInfo info);
	QString nextIrKey();
	IRInfo getIr(const QString& key);
	QList<IRInfo> getIrs();

	void setExcit(const ExcitCfg&);
	ExcitCfg getExcit();

private:
	QString path;

	QString fileName();
	void initFile();
	QDomDocument read();
	void write(const QDomDocument&);
	QString irKeyToString(int key);
	bool fileExists();
};

#endif
