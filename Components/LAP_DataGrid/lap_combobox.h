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


#ifndef __LAP_COMBOBOX_H__
#define __LAP_COMBOBOX_H__

#include <QItemDelegate>
#include <QSqlQuery>
#include <QVector>

class LAP_ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    LAP_ComboBoxDelegate(QObject *parent = 0, QString sql="");

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QString _sql;
    QVector< QVector<QVariant> > _data;
};


#endif // __LAP_COMBOBOX_H__
