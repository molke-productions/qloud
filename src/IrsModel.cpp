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

#include "QLCfg.h"
#include "QLE.h"
#include "QLUtl.h"
#include "IrsModel.h"
#include "IR.h"

IrsModel::IrsModel(
	const QString& aWorkDir,
	QMap<PlotWindow*, QString>* aPlots,
	QObject* parent
) : QAbstractTableModel(parent) {
	this->workDir = aWorkDir;
	this->plots = aPlots;

	QLCfg cfg(this->workDir);
	this->list = cfg.getIrs();
}

IrsModel::~IrsModel() {}

int IrsModel::rowCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	try {
		QLCfg cfg(this->workDir);
		return cfg.getIrs().size();
	} catch(QLE e) {
		emit showCritical(e.msg);
	}
	return 0;
}

int IrsModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return 3;
}

QVariant IrsModel::data(const QModelIndex& index, int role) const {
	if( ! index.isValid())
		return QVariant();

	if(role == Qt::UserRole) {
		IRInfo ii = this->list.at(index.row());
		return ii.key;
	}

	if(role == Qt::DisplayRole) {
		IRInfo ii = this->list.at(index.row());
		switch(index.column()) {
			case 0: return ii.info;
			case 1: return ii.format();
			case 2: return ii.maxLevelAsString();
		}
	}

	return QVariant();
}

QVariant IrsModel::headerData(
	int section,
	Qt::Orientation orientation,
	int role
) const {
	if(role != Qt::DisplayRole)
		return QAbstractTableModel::headerData(section, orientation, role);
	if(orientation != Qt::Horizontal)
		return QAbstractTableModel::headerData(section, orientation, role);

	switch(section) {
		case 0: return tr("Description");
		case 1: return tr("Used excitation");
		case 2: return tr("Max. level [dB]");
	}
	return QVariant();
}

bool IrsModel::setData(
	const QModelIndex& index,
	const QVariant& value,
	int role
) {
	Q_UNUSED(role);

	if( ! index.isValid())
		return false;
	if(index.column() != 0) // 'info' only is editable
		return false;

	IRInfo ii = this->list.at(index.row());
	ii.info = value.toString();
	this->list.replace(index.row(), ii);
	try {
		QLCfg cfg(this->workDir);
		cfg.replaceIr(ii);
	} catch(QLE e) {
		emit showCritical(e.msg);
		return false;
	}
	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags IrsModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flag = QAbstractTableModel::flags(index);
	if(index.column() == 0)
		flag |= Qt::ItemIsEditable;
	return flag;
}

bool IrsModel::removeRows(int row, int count, const QModelIndex& parent) {
	Q_UNUSED(parent)

	// only single row selection is allowed
	if(count != 1)
		return false;

	// be sure there aren't opened plots with this measure
	IRInfo ii = this->list.at(row);
	foreach(QString plotDigest, this->plots->values()) {
		if(plotDigest == this->workDir + "@" + ii.key) {
			emit showCritical(
				tr("Close plot window with this measurement before deleting")
			);
			return false;
		}
	}

	// delete from own container and qloud.xml
	this->list.removeAt(row);
	try {
		QLCfg cfg(this->workDir);
		cfg.removeIr(ii.key);
	} catch(QLE e) {
		emit showCritical(e.msg);
		return false;
	}

	// delete associated files
	QString excitPath = this->workDir + "/" + ii.key + IR::irFileName();
	QString trimPath = this->workDir + "/" + ii.key + IR::trimmedIrFileName();
	try {
		QFile excitFile(excitPath);
		if(excitFile.exists())
			if( ! excitFile.remove()) {
				emit showCritical(
					tr("Failed removing ") + excitFile.fileName()
				);
				return false;
			}
		QLUtl::checkFileError(excitFile);

		QFile trimFile(trimPath);
		if(trimFile.exists())
			if( ! trimFile.remove()) {
				emit showCritical(
					tr("Failed removing ") + trimFile.fileName()
				);
				return false;
			}
		QLUtl::checkFileError(trimFile);
	} catch(QLE e) {
		emit showCritical(e.msg);
		return false;
	}

	beginResetModel();
	endResetModel();

	QString status = "\"";
	status += ii.info;
	status += tr("\" measurement is deleted");
	emit showStatus(status, 3000);
	return true;
}

void IrsModel::irListChanged() {
	try {
		QLCfg cfg(this->workDir);
		this->list = cfg.getIrs();
	} catch(QLE e) {
		emit showCritical(e.msg);
		return;
	}

	beginResetModel();
	endResetModel();
}

void IrsModel::updateWorkDir(const QString& newDir) {
	this->workDir = newDir;
	this->irListChanged();
}
