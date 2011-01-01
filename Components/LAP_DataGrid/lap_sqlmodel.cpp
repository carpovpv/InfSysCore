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

#include <QColor>
#include <QBrush>
#include <QSqlRecord>
#include <QFont>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlField>
#include <QVector>
#include <QVariant>
#include <QDateTime>
#include <QTextCodec>

bool LAP_SQLModel::canFetchMore ( const QModelIndex & parent ) const
{
    return _isall;
}

void LAP_SQLModel::fetchMore ( const QModelIndex &)
{
    QVector <QVariant> _temp(header.count());
    int lump = 0;

    do
    {
        _temp.clear();
        for (int i=0; i< header.count(); ++i)
            _temp.push_back(_query.value(i));

        _data.push_back(_temp);
        lump++;
    }while (   lump < max_lump && _query.next() );

    _isall = _query.next();

    _row_count = _data.size();

    //maybe we need to recalculate sums?
    if (!_isall && this->_totals)
    {

        if (_data.size()>0)
        {
            QVector < double > sums(_column_count,0);
            for (int j=0; j< _data.size(); ++j)
                for (int i=1; i< _column_count; ++i)
                    if (_isCalc[i])
                    {
                        if (!_neg) sums[i] += _data[j][i].toDouble();
                        else
                        {
                            if (_data[j][i].toDouble()>=0 && _neg == 1)
                                sums[i] += _data[j][i].toDouble();
                            if (_data[j][i].toDouble()<0 && _neg == 2)
                                sums[i] += _data[j][i].toDouble();
                        }
                    }
            _temp[0]="";
            for (int i=1; i< _column_count; ++i)
            {
                if (_isCalc[i])
                    _temp[i] = sums[i];
                else
                    _temp[i] = "";
            }
            _data.push_back(_temp);
            _row_count++;
        }
    }


   // if (_grid->isAuto)

  emit layoutChanged();
  //_grid->resizeRowsToContents();

}


LAP_SQLModel::LAP_SQLModel(QWidget *parent, LAP_DataGrid *grid) :
        QAbstractTableModel(parent),
        _grid(grid),
        _row_count(0),
        _column_count(0)
{
    //empty constructor
}

Qt::ItemFlags LAP_SQLModel::flags(
    const QModelIndex &index) const
{
    int _column = index.column();
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (!_grid->LAP_Columns[_column].readOnly && _data[index.row()][0] != "")
        flags |=  Qt::ItemIsEditable;
    if (_colr != -1)
        if (_data[index.row()][_colr] != 0 && !_grid->LAP_Columns[_column].readOnly)
            flags ^= Qt::ItemIsEditable;
    if(_grid->LAP_Columns[_column].delegate.startsWith("CHECKBOX"))
    {
       flags |= Qt::ItemIsUserCheckable ;
       flags ^= Qt::ItemIsEditable;
       flags ^= Qt::ItemIsSelectable;
    }
    if(_grid->LAP_Columns[_column].readOnly)
        flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return flags;
}

void LAP_SQLModel::setRawData(int row, int column, QVariant value)
{
    if (_row_count > 0)
        if (row < _row_count && column < _column_count)
        {
            _data[row][column] = value;
            emit dataChanged(index(row,column), index(row, column));
        }
}

void LAP_SQLModel::refreshPk(QString pk, int row)
{
    QSqlQuery query;
    QString __pk;
    bool ok;
    pk.toInt(&ok, 10);

    if(!ok)
        __pk = "'" + pk + "'";
    else
        __pk = pk;

    QString refreshStmt = _grid->_refreshSql + " where " + _grid->_pk_u + " = " + __pk;
    ok = query.exec(refreshStmt);
    if (!ok)
    {
        QMessageBox::warning(0, tr("Error refreshing"), tr("Error in refreshing after update: %1\nSQL: %2").arg(refreshStmt));
        return;
    }
    query.next();
    for (int i=0;i< _column_count; ++i)
        _data[row][i] = query.value(i);
    emit dataChanged(index(row,0), index(row, _column_count));
}

bool LAP_SQLModel::setData(const QModelIndex &_index, const QVariant &value, int /* role */)
{
    if (value.toString().isEmpty()) return false;
        query.clear();

        QVariant oldValue = _data[_index.row()][_index.column()];
        if (value == oldValue) return false;

        if (_grid->function && !_grid->function->estimate(oldValue, value, _index))
            return false;

        QString update = "update " + _grid->_table + " set " + _grid->LAP_Columns[_index.column()].name + " = ";
        QVariant __id = _data[_index.row()][mhead[_grid->_pk]];
        QString where = " where " +  _grid->_pk + " = " + (__id.type() == QVariant::String ? ("'" + __id.toString() + "'") : __id.toString()) ;
        QString _where = " where " +  _grid->_pk_u + " = " + (__id.type() == QVariant::String ? ("'" + __id.toString() + "'") : __id.toString()) ;

        int dtype = value.type();
        QVariant updv = value;

        if (_grid->LAP_Columns[_index.column()].delegate.startsWith("CHECKBOX"))
        {
          if(value == Qt::Checked) updv = QVariant("Y");
          else updv= QVariant("N");
        }

        update += "? ";
        update += where;

        query.prepare(update);
        query.addBindValue(updv);

        bool ok = query.exec();
        if (!ok)
        {
            QMessageBox::warning(0, tr("Error updating"), tr("Error in update query: %1\nSQL: %2").arg(update).arg(query.lastError().text()));
            return false;
        }
        query.clear();

        QString refreshStmt = _grid->_refreshSql + _where;
        //QMessageBox::warning(0, "dsf", refreshStmt);
        ok = query.exec(refreshStmt);
        if (!ok)
        {
            QMessageBox::warning(0, tr("Error refreshing"), tr("Error in refreshing after update: %1\nSQL: %2").arg(refreshStmt));
            return 0;
        }
        query.next();
        for (int i=0;i< _column_count; ++i)
            _data[_index.row()][i] = query.value(i);

        if (_totals)
        {
            recalculate();
            emit dataChanged(index(_row_count-1,0), index(_row_count-1, _column_count));
            return true;
        }

        emit dataChanged(index(_index.row(),0), index(_index.row(), _column_count));
        return true;
    }

void LAP_SQLModel::recalculate()
{

    QVector < double > sums(_column_count,0);
    for (int j=0; j< _data.size()-1; ++j)
        for (int i=1; i< _column_count; ++i)
            if (_isCalc[i])
            {
                if (_neg == 0) sums[i] += _data[j][i].toDouble();
                else
                {
                    if (_data[j][i].toDouble()>=0 && _neg == 1)
                        sums[i] += _data[j][i].toDouble();
                    if (_data[j][i].toDouble()<0 && _neg == 2)
                        sums[i] += _data[j][i].toDouble();

                }
            }
    _data[_row_count-1][0]="";
    for (int i=1; i< _column_count; ++i)
    {
        if (_isCalc[i])
            _data[_row_count-1][i] = sums[i];
        else
            _data[_row_count -1][i] = "";
    }

}

void LAP_SQLModel::sort ( int column, Qt::SortOrder order )
{
    QString orderSql = _grid->_baseSql.simplified();
    int pos = orderSql.indexOf("order by",0, Qt::CaseInsensitive);
    if (pos != 0)
    {
        orderSql = orderSql.left(pos);
        orderSql += " order by " + QString::number(column+1) + (order == Qt::AscendingOrder ? " asc" : " desc" );
    }
    setQuery(orderSql, _totals,_neg, _colr);
    dataChanged(index(0,0), index(_row_count, _column_count));

}

QVariant LAP_SQLModel::data(const QModelIndex & _index, int role ) const
{
    //QTextCodec *codec =QTextCodec::codecForName("CP1251");

    int _row = _index.row();
    int _col = _index.column();
    QString _color = "";
    QVariant _val = _data[_row][_col];
    //if(_val.type() == QVariant::String)
    //    _val = codec->toUnicode(_val.toByteArray());
    QStringList dummy = _data[_row][_col].toString().split("|");
    if (dummy.count()>1)
    {
        _color = dummy[1].simplified();
        _val = dummy[0];
    }

    if (role == Qt::CheckStateRole && _grid->LAP_Columns[_col].delegate.startsWith("CHECKBOX"))
    {
        if(_val == "Y")
            return Qt::Checked;
        else return Qt::Unchecked;
    }

    if (role == Qt::BackgroundRole )
    {
        if (_data[_row][0] =="" ) return QBrush(QColor(210,210,255));
        if (_color != "" ) return QBrush(QColor(_color));
    }
    if (role == Qt::ForegroundRole)
        return QColor(0x536ec6);
    if (role==Qt::DisplayRole && _grid->LAP_Columns[_col].delegate != "IMAGE" )
    {
        if(_grid->LAP_Columns[_col].delegate == "CHECKBOX")
        {
            if(_grid->LAP_Columns[_col].isc)
                return _val == "Y" ? tr("Да") : tr("Нет");
            return QVariant();
        }
        if(_grid->LAP_Columns[_col].delegate == "CHECKBOXA")
        {
           return QVariant();
        }

        if(_grid->LAP_Columns[_col].currency)
        {

                    QString pre = QString::number(_val.toDouble(),'f',2);
                    const int l = pre.length();
                    int lpos = pre.lastIndexOf('.');
                    int g =0;
                    for(int i=lpos; i >0; i--)
                    {
                        if(g==3)
                        {
                            if(!(i==1 && pre[0]=='-'))
                            pre.insert(i," ");
                            g=0;
                        }
                        g++;
                    }
                    return QString(pre.replace(".",","));
        }
        if (_row >= _row_count || _col >= _column_count) return QVariant();
        //if (_data[_row][_col] == 0 ) return QVariant("");
        if(_val.type() == QVariant::DateTime) return _val.toDateTime().toString("dd.MM.yyyy HH:mm");
        if(_val.type() == QVariant::Date) return _val.toDate().toString("dd.MM.yyyy");

        return _val;
    }
    if (role == Qt::FontRole && _data[_row][0] =="")
    {
        QFont font("MS Shell Dlg 2", _grid->_fontsize);
        font.setBold(true);
        return QVariant(font);
    }
    if(role == Qt::TextAlignmentRole && _grid->LAP_Columns[_col].currency)
                   return QVariant(Qt::AlignRight | Qt::AlignVCenter);

    if(role == Qt::TextAlignmentRole && (_data[_row][_col].type() == QVariant::Double || _data[_row][_col].type() == QVariant::Int || _grid->LAP_Columns[_col].name == "CODE" || _grid->LAP_Columns[_col].name == "ID" ) )
                   return QVariant(Qt::AlignCenter);

    if (role == Qt::ToolTipRole )
        return "<p>" + this->index(_row,_col).data().toString() + "</p>";

    if(role == Qt::DecorationRole && _grid->LAP_Columns[_col].delegate == "IMAGE")
    {
          QByteArray str = _data[_row][_col].toByteArray();
          QImage img = QImage::fromData(str);
          if(img.isNull()) return QVariant();
          return img.scaledToHeight(20);;
    }
    return QVariant();
}

QSqlError LAP_SQLModel::lastError()
{
    return query.lastError();
}

bool LAP_SQLModel::setQuery(QString sqlQuery, bool totals, int neg, int colr)
{

    _query.clear();
    //_query.setForwardOnly(true);

    for (int i=0; i< _data.size(); ++i)
        _data[i].clear();
    _data.clear();
    _row_count = _column_count = 0;
    mhead.clear();

    bool ok = _query.exec(sqlQuery);
    if (!ok)
    {
        error = _query.lastError();         
        return false;
    }

    QSqlRecord _record = _query.record();
    header.clear();
    _isCalc.clear();
    _isCalc.resize(_record.count());
    _neg = neg;

    _colr = colr;

    for (int i=0; i< _record.count(); ++i)
    {
        header << _grid->LAP_Columns[i].caption;
        mhead[_grid->LAP_Columns[i].name] = i;
        if (_grid->LAP_Columns[i].isc && (_record.field(i).type() == QVariant::Int || _record.field(i).type() == QVariant::Double))
            _isCalc[i] = true;
    }

    QVector <QVariant> _temp(_record.count());
    int lump = 0;
    while ( lump < max_lump && _query.next() )
    {
        for (int i=0; i< header.count(); ++i)
            _temp[i] = _query.value(i);
        _data.push_back(_temp);
        lump++;
    }

    _isall = _query.next();

    _column_count = _record.count();
    _totals=totals;

    if (totals && !_isall)
    {
        if (_data.size()>0)
        {

            QVector < double > sums(_column_count,0);
            for (int j=0; j< _data.size(); ++j)
                for (int i=1; i< _column_count; ++i)
                    if (_isCalc[i])
                    {
                        if (_neg == 0) sums[i] += _data[j][i].toDouble();
                        else
                        {
                            if (_data[j][i].toDouble()>=0 && _neg == 1)
                                sums[i] += _data[j][i].toDouble();
                            if (_data[j][i].toDouble()<0 && _neg == 2)
                                sums[i] += _data[j][i].toDouble();
                        }
                    }
            _temp[0]="";
            for (int i=1; i< _column_count; ++i)
            {
                if (_isCalc[i])
                    _temp[i] = sums[i];
                else
                    _temp[i] = "";
            }
            _data.push_back(_temp);
        }
    }

    _row_count = _data.size();

    reset();
    return true;
}

QVariant LAP_SQLModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && header.count() > section)
        return QVariant(header.at(section));
    else
        return QAbstractTableModel::headerData(section, orientation, role);
}
