#ifndef __CUSTOMERS_H__
#define __CUSTOMERS_H__

#include <QDialog>
#include <QSqlError>
#include <QVector>
#include <QSqlQuery>
#include <QSqlRecord>
#include "lap_datagrid.h"
#include "ui_dictionaries.h"

#include "../../lap_abstractmodule.h"

/*!
  The modules operates with basic dictionaries of the system.
  A dictionary is a  table  in the database. In order to add
  new dict you should add several lines in the constructor.
  And create a table.
*/

class DictionaryLesCo : public LAP_AbstractModule, public Ui::Dictionary
{
	Q_OBJECT
	public:
                DictionaryLesCo(QWidget *, bool ro = false);
                void install();
                void remove();
                void upgrade();
                int getPrefferedHeight() const { return 600; }
	private:
	    struct dict_column{
                QString column;
                QString caption;
                bool hide;
	    	bool readonly;
    	};
		struct dict_table{
                        QString tableName;
                        QString caption;
                        QString refresh;
                        QString esql;
                        QString sql;
			QString pk;
                        QVector < struct dict_column> columns;
		};
		QVector <struct dict_table > Dictionary;
		int currentDict;
				
		QString createSqlInsert(QString value);
	private slots:
		void add_record();
		void delete_record();
                void populateGrid( int);
};

#endif // __CUSTOMERS_H__
