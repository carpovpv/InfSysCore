
#include "dictionary.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>

extern "C" Q_DECL_EXPORT LAP_AbstractModule *  createModule(QWidget *parent, bool ro)
{
    return new DictionaryLesCo(parent, ro);
}


void DictionaryLesCo::install()
{
}

void DictionaryLesCo::remove()
{

}

void DictionaryLesCo::upgrade()
{

}

DictionaryLesCo::DictionaryLesCo(QWidget *parent, bool ro) :  LAP_AbstractModule(parent, ro)
{
	 setupUi(this);
         moduleName = "Dictionary";

	 Dictionary.clear();	 
	 
	 struct dict_column col;
	 struct dict_table table;
	 
        //roles
         table.columns.clear();
         table.tableName = "roles";
         table.esql = "select id, rolename from roles order by id";
         table.refresh = "select id, rolename from roles ";

         table.pk = "id";
         table.caption = tr("Users' roles");
         table.sql = "insert into roles (rolename) values (";

         col.column = "ID";
         col.caption = tr("ID");
         col.readonly =  true;
         col.hide = true;
         table.columns.push_back(col);

         col.column = "ROLENAME";
         col.caption = tr("Role name");
         col.readonly =  false;
         col.hide = false;
         table.columns.push_back(col);

         Dictionary.push_back(table);

        //depts
         table.columns.clear();
         table.tableName = "depts";
         table.esql = "select id, dept from depts order by 2";
         table.refresh = "select id, dept from depts ";

         table.pk = "id";
         table.caption = tr("Departments");
         table.sql = "insert into depts(dept) values (";

         col.column = "ID";
         col.caption = tr("ID");
         col.readonly =  true;
         col.hide = true;
         table.columns.push_back(col);

         col.column = "dept";
         col.caption = tr("Department");
         col.readonly =  false;
         col.hide = false;
         table.columns.push_back(col);


         Dictionary.push_back(table);

         //statuses
         table.columns.clear();
         table.tableName = "statuses";
         table.esql = "select id,status, statusname, statuscolor from statuses order by 2";
         table.refresh = "select id,status, statusname, statuscolor from statuses ";

         table.pk = "id";
         table.caption = tr("Statuses");
         table.sql = "insert into statuses(statusname) values (";

         col.column = "ID";
         col.caption = tr("ID");
         col.readonly =  true;
         col.hide = true;
         table.columns.push_back(col);

         col.column = "status";
         col.caption = tr("Code");
         col.readonly =  false;
         col.hide = false;
         table.columns.push_back(col);

         col.column = "statusname";
         col.caption = tr("Status");
         col.readonly =  false;
         col.hide = false;
         table.columns.push_back(col);

         col.column = "statuscolor";
         col.caption = tr("Color (HTML)");
         col.readonly =  false;
         col.hide = false;
         table.columns.push_back(col);

         Dictionary.push_back(table);

         for(int i =0; i< Dictionary.size(); ++i)
                lstDictionary->addItem(Dictionary[i].caption);

	 connect(lstDictionary, SIGNAL(currentRowChanged(int)), this, SLOT(populateGrid(int))); 

         //lstDictionary->sortItems();

         show();
         lstDictionary->setCurrentRow(0);
	 
         connect(btnAdd, SIGNAL(released()), this, SLOT(add_record()));
         connect(btnDelete, SIGNAL(clicked()), this, SLOT(delete_record()));
 }

void DictionaryLesCo::populateGrid( int id)
{
	 currentDict = id;
	 tblDictionary->clearColumns();
	 for( int i =0; i < Dictionary[id].columns.count(); ++i)
	 	tblDictionary->addColumn(Dictionary[id].columns[i].column, 
	 							 Dictionary[id].columns[i].column,
	 							 Dictionary[id].columns[i].caption,
	 							 Dictionary[id].columns[i].readonly,
	 							 Dictionary[id].columns[i].hide,
	 							 "TEXTEDIT",
	 							 "");
	 tblDictionary->setRefreshQuery(Dictionary[id].refresh);
	 tblDictionary->setQuery(Dictionary[id].esql,
	 						 Dictionary[id].tableName,
	 						 Dictionary[id].pk, Dictionary[id].pk,false);
     
   if(tblDictionary->model() && tblDictionary->model()->columnCount() > 2)
       tblDictionary->setColumnWidth(3, tblDictionary->columnWidth(3)+40);

   tblDictionary->setColumnWidth(0, 50);
   if(tblDictionary->model()->columnCount() > 2)
          tblDictionary->setColumnWidth(1, 50);

   tblDictionary->horizontalHeader()->setStretchLastSection(true);
   tblDictionary->resizeRowsToContents();

}

QString DictionaryLesCo::createSqlInsert(QString value)
{
	return QString(Dictionary[currentDict].sql + "'" + value + "')");
}

void DictionaryLesCo::add_record()
{
	
     bool ok;
     QString text = QInputDialog::getText(this, tr("Please, input the value:"),
                                          Dictionary[currentDict].caption, QLineEdit::Normal,
                                          "", &ok);
     if (!(ok && !text.isEmpty()))
     	return;

     text = text.trimmed();
     
     QSqlQuery query;
     QString sql = createSqlInsert(text);
     ok = query.exec(sql);
     if(ok)
     {
     	populateGrid(currentDict);
     	tblDictionary->selectRow(tblDictionary->model()->rowCount()-1);
     }
     else
         QMessageBox::warning(this, tr("Error in insert statement."), \
                tr("Error in insert statement.\nError text: %1").\
     	 	arg(query.lastError().text()));
 }
	
void DictionaryLesCo::delete_record()
{
        if(!tblDictionary->model()) return;

	QString table = Dictionary[currentDict].tableName;
	
	QItemSelectionModel * mdl = tblDictionary->selectionModel();
	QModelIndexList indexes = mdl->selectedRows();
		
	for(int i=0; i< indexes.size(); ++i)
	{
		int row  = indexes[i].row();
		if(row >=0)
		{
			QMessageBox::StandardButton ret = QMessageBox::question(this, \
                                        tr("Question"), tr("Are you sure you would like to delete the element selected?"),\
					QMessageBox::Yes | QMessageBox::No);
			if (ret == QMessageBox::Yes)
			{
				QString id = tblDictionary->model()->index(row, 0).data().toString();
				QSqlQuery query;
				bool ok= query.exec("delete from " +\
												 Dictionary[currentDict].tableName + \
												 " where id = " + id );
                                if(!ok)
                                QMessageBox::warning(this, tr("Error"), \
                                                                        tr("Error deleteing the element from the dictionary.\nSQL: %1").\
   	    									arg(query.lastError().text()));
                        }
                  }
            }
            populateGrid(currentDict);
}
