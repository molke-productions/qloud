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

#include "LineEditDelegate.h"

LineEditDelegate::LineEditDelegate(QObject* parent) : QItemDelegate(parent) {}

QWidget* LineEditDelegate::createEditor(
	QWidget *aParent,
	const QStyleOptionViewItem& option,
	const QModelIndex& index
) const {
	Q_UNUSED(option)
	Q_UNUSED(index)

	QLineEdit* edit = new QLineEdit(aParent);
	edit->installEventFilter(const_cast<LineEditDelegate*>(this));

	return edit;
}

void LineEditDelegate::setEditorData(
	QWidget* editor,
	const QModelIndex& index
) const {
	QString value = index.model()->data(index, Qt::DisplayRole).toString();
	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	edit->setText(value);
}

void LineEditDelegate::setModelData(
	QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index
) const {
	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	model->setData(index, edit->text());
}

void LineEditDelegate::updateEditorGeometry(
	QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& index
) const {
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}
