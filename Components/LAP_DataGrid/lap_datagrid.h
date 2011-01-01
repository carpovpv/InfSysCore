#ifndef LAP_DATAGRID_H
#define LAP_DATAGRID_H

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

#include <QTableView>
#include <QWidget>
#include <QMap>
#include <QSqlRecord>
#include <QKeyEvent>
#include <QSqlError>
#include <QVector>
#include <QSqlQuery>
#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <QStandardItemModel>
#include <QMessageBox>

//user-defined components

#include "lap_combobox.h"
#include "lap_lineedit.h"
#include "lap_dateedit.h"

//Unfortunately we cannot use multiple inheritance here, so we must create two class
//one for the table and one for the model

class LAP_SQLModel;

class Q_DECL_EXPORT LAP_Function{
public:
    LAP_Function(){}
    virtual ~LAP_Function(){}
    virtual bool estimate(const QVariant & oldValue = 0, const QVariant & newValue = 0, const QModelIndex &index = QModelIndex())
    {
        return false;
    }
};

/*!
  LAP_DataGrid class provides a simple to use combination of QTableView
  and QAbstractTableModel. It is known that in order to create a view
  one must first create a model for it. But when you create a big number
  of user widgets for operating with tables you prefer to use simple
  delegates and formatting. And use one class instead of two is promising.
  The LAP_DataGrid class provides this functionality. In the Designer you
  simple create a QTableView, change in to LAP_DataGrid and in code manually
  define you query and columns and delegates for them. Let's see an example.

  Suppose you have a query

  select r.id, r.available, r.ro, m.caption from modules m inner join modulerole r on r.moduleid = m.id

  Here one join statement, of course the id column must be not editable,
  available column is better to show with a checkbox delegate, whereas
  caption column must be edited by QLineEdit class.

  It can be done so:

  QString sql = "select r.id, r.available, r.ro, m.caption from modules m inner join modulerole r on r.moduleid = m.id ";

  QString where = " where r.roleid = " + roleid + " and m.menuid =" + menuid;

  ui->tblModules->clearColumns();

  ui->tblModules->addColumn("id", "r.id", tr("ID"), true, false);

  ui->tblModules->addColumn("available", "r.available", tr("Available"), false, false, "CHECKBOXA");

  ui->tblModules->addColumn("ro", "r.ro", tr("Read only"), false, false, "CHECKBOXA");

  ui->tblModules->addColumn("caption", "m.caption", tr("Caption"), true, false, "TEXTEIDT");

  ui->tblModules->setRefreshQuery(sql);

  ui->tblModules->setQuery(sql + where + " order by 3", "modulerole", "id", "r.id", false);

  ui->tblModules->horizontalHeader()->setStretchLastSection(true);

  ui->tblModules->resizeRowsToContents();


  The addColumn() functions determines how the column could be treated. Its name from the query,
  caption in a table, and a name in the database table. The LAP_DataGrid class automatically
  updates the underling data in tables based on the setttings in setQuery function. The grid
  supports the pagination. So when your query return 10000 record only first 900 will be
  displayed first, others will be loaded when user scrolls to the end the grid.

 */

class Q_DECL_EXPORT LAP_DataGrid : public QTableView
{
    Q_OBJECT
    friend class LAP_SQLModel;
public:

    LAP_DataGrid(QWidget *);

    QVariant getData(unsigned int row, unsigned int column);
    QVariant getData(unsigned int row, QString column);
    QVariant getCurrentData(unsigned int column);
    QVariant getCurrentData(QString);

    //find first row where the filed's value is equal to value
    int getRowByField(QString field, QString value);

    int getCurrentRow();

    //refresh a row in a table with the data from the refreshSql statement
    //with pk as id
    void refreshPk(QString pk, int row);

    //set a query for the table
    bool setQuery(
        QString sql,   	//sql statement
        QString table, 	//main table. /this table will be updated
        QString pk, 		//primary key in that table
        QString pku,          //alias for the pk in sql query
        bool totals = false,  //need totals
        int neg = 0,          //if new == 0 then sum all positive
        //else if neq == 1 then sum positive and negative values
        int column = -1,       //If some rows can be readonly you may use a special column
                     //in a sql statement. Its values will be 0 for those rows
                     //which must be RO and 1 which must not.
        int fontsize = 8
    );

    /*!
        Add a column to the datagrid. Note if the number of columns in sql query
        is more than explicitly defined by addColumn function, your program will
        crash.
      */
    void addColumn(
        QString name,		//name of the column in a table
        QString nameq, 		//name of the column in a sql statement
        QString caption,	//caption for the column
        bool readOnly = true,	//readonly flag
        bool hide = false,	//don't show this column
        QString delegate = "",	//which delegate class to use
        //possible values are: COMBOBOX, TEXTEDIT, DATEEDIT, CHECKBOXA
        //parameter for a delegate
        //in case of COMBOBOX a query to populate it with data
        //in case of TEXTEDIT a regexp to validate user input
        QString sql_for_delegate = "",
        bool isc = false,        //aggregate column
        bool currency = false
    );

    /*! Delete all information about columns in the table*/
    void clearColumns();

    /*! Triggers can modify data in a record. So we changed one field, but triggers may change othe field.
      This method is needed for updating all record and refreshing it in a table. */
    void setRefreshQuery(QString);

    /*! Sometimes we need to set raw data. Without updating database. Useful for avg, sums and so on.*/
    void setRawData(int row, int column, QVariant value);

    /*! Call-back to the function*/
    void setBeforeUpdate(class LAP_Function *);

    bool isAuto;

signals:
    /*! User clicks on another row.*/
    void rowChange(int row);

    /*! User opens an editor and modify data in a cell.*/
    void dataChanged(int);

    /*! Double click.*/
    void doubleClick(int);

private:
    /* The data*/
    QMap < QString , unsigned int> columns;

    QSqlRecord _header;
    QString _table;           //main table
    QString _pk, _pk_u;       //pk and its alias
    QString _refreshSql;      //refreshSql
    QString _baseSql;         //base select

    struct LAP_Column {
        QString name;     //the name of the corresponding field of the database
        QString nameq;    //the same field as it is named in the query
        QString caption;  //caption to be displayed in the table
        bool readOnly;    //readonly
        bool hide;        //don't show the column
        QString delegate; //what delegate must be used fro the column
        //available are TEXTEDIT, COMBOBOX, DATEEDIT
        QString sql;      //parameter for a delegate.
        //For COMBOBOX you may pass sql query to populate it with values.
        //For TEXTEDIT - regular expression to validate the input data
        bool isc;
        bool currency;
    };
    QVector <struct LAP_Column> LAP_Columns;
    LAP_SQLModel *_model;
    void setModel (LAP_SQLModel * model);
    int _fontsize;

    LAP_Function * function;

private slots:
    void row_activated(const QModelIndex&);
    void changed(QModelIndex, QModelIndex);
    void clicked( const QModelIndex &);
protected:
    void keyPressEvent(QKeyEvent *);
};

//our model class
class Q_DECL_EXPORT LAP_SQLModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LAP_SQLModel(QWidget *, LAP_DataGrid *);

    QVariant data(const QModelIndex&, int) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int /* role */);
    int rowCount(const QModelIndex& parent = QModelIndex()) const {
        return _row_count;
    }
    int columnCount(const QModelIndex& parent = QModelIndex()) const {
        return _column_count;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setQuery(QString sqlQuery, bool totals = false, int neg = 0, int colr = -1);
    QSqlError lastError();

    //sort will be performed through the SQL server
    void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
    void setRawData(int ,int, QVariant);
    void refreshPk(QString pk, int row);
private:
    LAP_DataGrid * _grid;

    QVector< QVector< QVariant > > _data; //raw data
    long _row_count;
    long int _column_count;

    //use paging in 300 rows.
    static const int max_lump = 900;
    bool _isall;

    QSqlQuery query, _query;
    QSqlError error;
    QStringList header;
    QMap<QString, int> mhead;

    bool _totals;
    int _neg;
    int _colr;

    QVector <bool> _isCalc;
    //recalculate all sums
    void recalculate();

    bool canFetchMore ( const QModelIndex & parent = QModelIndex() ) const;
    void fetchMore ( const QModelIndex & parent = QModelIndex());
};


#endif // __LAP_DATAGRID_H__

