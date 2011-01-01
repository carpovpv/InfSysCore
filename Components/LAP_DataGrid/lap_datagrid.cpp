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

#include "lap_datagrid.h"
#include "lap_checkbox.h"

#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>

LAP_DataGrid::LAP_DataGrid(QWidget *parent) : QTableView(parent)
{
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    _model = new LAP_SQLModel(this, this);
    isAuto = false;
    connect(_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(changed(QModelIndex, QModelIndex)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(clicked(const QModelIndex&)) );
    connect(this, SIGNAL(clicked(const QModelIndex&)), this,\
            SLOT(row_activated(const QModelIndex&)));
    connect(this, SIGNAL(pressed(const QModelIndex&)), this,\
            SLOT(row_activated(const QModelIndex&)));

    function = NULL;
}

void LAP_DataGrid::setBeforeUpdate(LAP_Function * func)
{
    function = func;
}

void LAP_DataGrid::clicked(const QModelIndex & ind)
{
    if(ind.row() >= 0)
    {
        emit doubleClick(ind.row());
    }
}

void LAP_DataGrid::refreshPk(QString pk, int row)
{
    _model->refreshPk(pk, row);
}

void LAP_DataGrid::changed(QModelIndex index, QModelIndex)
{
    emit dataChanged(index.row());
}

int LAP_DataGrid::getRowByField(QString field, QString value)
{
    int col = columns[field.toUpper()];
    for (int i =0; i< _model->rowCount(); ++i)
    {
        if (_model->index(i,col).data().toString() == value)
            return i;
    }
    return -1;
}

void LAP_DataGrid::setRawData(int row, int column, QVariant value)
{
    _model->setRawData(row, column, value);
}

void LAP_DataGrid::clearColumns()
{
    LAP_Columns.clear();
}

void LAP_DataGrid::addColumn(QString name,
                             QString nameq,
                             QString caption,
                             bool readOnly,
                             bool hide,
                             QString delegate,
                             QString sql,
                             bool isc, bool currency)
{

    struct LAP_Column column;
    column.name = name.toUpper();
    column.nameq = nameq.toUpper();
    column.caption = caption;
    column.readOnly = readOnly;
    column.hide = hide;
    column.delegate =  delegate;
    column.sql = sql;
    column.isc = isc;
    column.currency = currency;

    LAP_Columns.push_back(column);
}

bool LAP_DataGrid::setQuery(QString sql,
                            QString table,
                            QString pk,
                            QString pku,
                            bool totals,
                            int neg,
                            int col, int fontsize)
{

    bool ok = _model->setQuery(sql, totals, neg, col);
    if (!ok)
        return false;

    _table = table;
    _pk = pk;
    _pk_u = pku;
    _baseSql = sql;
    _fontsize = fontsize;

    setModel(_model);

    for (int i=0; i< LAP_Columns.size(); ++i)
    {
        QString delegate = LAP_Columns[i].delegate;
        QString _sql = LAP_Columns[i].sql;

        if (delegate == "COMBOBOX")
            setItemDelegateForColumn(i, new LAP_ComboBoxDelegate(this, _sql));
        else if (delegate == "DATEEDIT")
            setItemDelegateForColumn(i, new LAP_DateEditDelegate(this));
        else if (delegate == "TEXTEDIT")
            setItemDelegateForColumn(i, new LAP_LineEditDelegate(this,_sql));
        else if (delegate == "CHECKBOXA")
            setItemDelegateForColumn(i, new LAP_CheckBoxDelegate(this));

        if (LAP_Columns[i].hide)
            hideColumn(i);
    }
    return ok;
}

void LAP_DataGrid::setRefreshQuery(QString sql)
{
    _refreshSql=sql;
}

void LAP_DataGrid::setModel ( LAP_SQLModel * model )
{
    QTableView::setModel(model);

    for (int i=0; i< LAP_Columns.count(); ++i)
        columns[LAP_Columns[i].nameq] = i;

    if (model->rowCount())
    {
        setCurrentIndex(model->index(0,0));
        emit rowChange(0);
    }

    //function dosent work sometimes. official bug
    //this->resizeRowsToContents();
}

void LAP_DataGrid::keyPressEvent(QKeyEvent *key)
{
    int _row = currentIndex().row();
    switch (key->key())
    {
    case Qt::Key_Up:
    {
        if (_row > 0)
            emit rowChange(_row - 1);
        break;
    }
    case Qt::Key_Down:
    {
        if (_row < model()->rowCount()-1)
            emit rowChange(_row + 1);
        break;
    }
    case Qt::Key_PageUp:
    case Qt::Key_PageDown :
        return;
    case Qt::Key_F5:

        if(this->model() && this->model()->rowCount()>0)
    {
    QClipboard *clipboard = QApplication::clipboard();
    QString data = "";
        for(int i=0; i< model()->rowCount(); ++i)
    {
       for(int j=0; j< model()->columnCount(); ++j)
           if(!isColumnHidden(j))
               data += model()->index(i,j).data().toString() + "\t";
    data += "\n";
    }
    clipboard->setText(data);
}
            break;

    }
    QTableView::keyPressEvent(key);
}

void LAP_DataGrid::row_activated(const QModelIndex &index)
{
    emit rowChange(index.row());
}

QVariant LAP_DataGrid::getData(unsigned int row, unsigned int column)
{
    if (row< model()->rowCount() && column < LAP_Columns.count())
        return model()->index(row, column).data();
    return QVariant("");
}

QVariant LAP_DataGrid::getData(unsigned int row, QString column)
{
    return getData(row,columns[column.toUpper()]);
}

int LAP_DataGrid::getCurrentRow()
{
    QItemSelectionModel * mdl = selectionModel();
    QModelIndexList indexes = mdl->selectedRows();
    if (indexes.size())
        return indexes[0].row();
    return -1;
}

QVariant LAP_DataGrid::getCurrentData(unsigned int column)
{
    QItemSelectionModel * mdl = selectionModel();
    QModelIndexList indexes = mdl->selectedRows();
    if (indexes.size())
    {
        return getData(indexes[0].row(),column);
    }
    return QVariant("");
}

QVariant LAP_DataGrid::getCurrentData(QString column)
{
    return getCurrentData(columns[column.toUpper()]);
}
