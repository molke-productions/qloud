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

#ifndef IRSMODEL_H
#define IRSMODEL_H

#include <QtWidgets>
#include "QLE.h"
#include "IRInfo.h"
#include "PlotWindow.h"

class IrsModel : public QAbstractTableModel {
	Q_OBJECT

public:
	IrsModel(
		const QString& aWorkDir,
		QMap<PlotWindow*,QString>* plots,
		QObject* parent = 0
	);
	~IrsModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool setData(
		const QModelIndex& index,
		const QVariant& value,
		int role = Qt::EditRole
	);

	QVariant headerData(
		int section,
		Qt::Orientation orientation,
		int role = Qt::DisplayRole
	) const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

	bool removeRows(
		int row,
		int count,
		const QModelIndex& parent=QModelIndex()
	);

public slots:
	void irListChanged();
	void updateWorkDir(const QString&);

signals:
	void showCritical(const QString&) const;
	void showStatus(const QString&, int) const;

private:
	QString workDir;
	QList<IRInfo> list;
	QMap<PlotWindow*,QString>* plots;
};

#endif
