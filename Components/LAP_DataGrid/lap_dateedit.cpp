
/*
    Copyright (C) 2009  Carpov P.V. carpovpv@mail.ru
    LAP_DataGrid a class providing flexible behaviour of standard
    QTableView class.

    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "lap_dateedit.h"
#include <QDateEdit>

// place your code here

LAP_DateEditDelegate::LAP_DateEditDelegate(QObject *parent)
        : QItemDelegate(parent)
{
}

QWidget * LAP_DateEditDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &/* option */,
        const QModelIndex &/* index */) const
{
    QDateEdit *editor = new QDateEdit(parent);
    return editor;
}

void LAP_DateEditDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString v = index.model()->data(index, Qt::DisplayRole).toString();
    QDate value = QDate::fromString(v,"dd.MM.yyyy");
    QDateEdit *dtm = static_cast<QDateEdit*>(editor);
    dtm->setDate(value);
}

void LAP_DateEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex &index) const
{
    QDateEdit *dtm = static_cast<QDateEdit*>(editor);
    QVariant value = dtm->date();
    model->setData(index, value, Qt::EditRole);
}

void LAP_DateEditDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}


