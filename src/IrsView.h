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

#ifndef IRSVIEW_H
#define IRSVIEW_H

#include <QtWidgets>

class IrsView : public QTableView {
	Q_OBJECT

public:
	IrsView(QAbstractTableModel* model, QWidget* parent=0);
	QList<QAction*> actions() const;

signals:
	void showCritical(const QString&);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

private slots:
	void deleteMeasure();

private:
	QMenu* menu;
};

#endif
