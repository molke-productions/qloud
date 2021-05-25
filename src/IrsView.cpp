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

#include "IrsView.h"
#include "LineEditDelegate.h"
#include "QLUtl.h"
#include "QLE.h"
#include "QLCfg.h"

IrsView::IrsView(
	QAbstractTableModel* model,
	QWidget* parent
) : QTableView(parent) {
	this->setModel(model);
	this->setAlternatingRowColors(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);

	LineEditDelegate* delegate = new LineEditDelegate();
	this->setItemDelegate(delegate);

	this->setContextMenuPolicy(Qt::DefaultContextMenu);

	QLabel* tmp = new QLabel("W88 s, 192000 Hz/32 bit, 99999–99999 Hz");
	int columnWidth = tmp->sizeHint().width();
	delete tmp;
	this->setColumnWidth(0, columnWidth);
	this->setColumnWidth(1, columnWidth);
	tmp = new QLabel("WW Max. level [dB]");
	columnWidth = tmp->sizeHint().width();
	delete tmp;
	this->setColumnWidth(2, columnWidth);

	this->menu = new QMenu();
	QAction* delAction = this->menu->addAction(tr("Delete measurement"));
	connect(delAction, SIGNAL(triggered()), this, SLOT(deleteMeasure()));
}

void IrsView::contextMenuEvent(QContextMenuEvent* event) {
	QTableView::contextMenuEvent(event);
	this->menu->popup(event->globalPos());
}

void IrsView::deleteMeasure() {
	QModelIndex index = this->currentIndex();
	if( ! index.isValid() )
		return;

	QModelIndex infoIndex = this->model()->index(index.row(), 0);
	QString info = this->model()->data(infoIndex, Qt::DisplayRole).toString();

	QString msg = tr("Measurement description:\n\n");
	msg += info;
	msg += tr("\n\nAre you sure you want to delete this measurement?");
	int doDelete = QMessageBox::question(
		this,
		tr("Delete measurement permanently"),
		msg,
		QMessageBox::Yes,
		QMessageBox::Cancel
	);
	if(doDelete != QMessageBox::Yes)
		return;
	// ignore return value
	this->model()->removeRow(index.row());
}
