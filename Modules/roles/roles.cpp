#include "roles.h"
#include "ui_roles.h"

#include <QMessageBox>
#include <QSqlQuery>

#include "addrole.h"

Roles::Roles(QWidget *parent, bool ro):
        LAP_AbstractModule(parent, ro),
        ui(new Ui::Roles)
{
    ui->setupUi(this);
    moduleName = "LAP_Roles";
    version = "1.0.0";

    connect(ui->btnAddRole, SIGNAL(clicked()), this, SLOT(add_role()));
    connect(ui->btnDelRole, SIGNAL(clicked()), this, SLOT(del_role()));
    connect(ui->cmbMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(menu_changed(int)));
    connect(ui->tblRoles, SIGNAL(rowChange(int)), this, SLOT(role_changed(int)));

    QSqlQuery query;
    query.exec("select id, menu from menucategory order by sorter");
    while(query.next())
        ui->cmbMenu->addItem(query.value(1).toString(), query.value(0));

    LoadRoles();
}

int Roles::getPrefferedWidth() const
{
    return 800;
}

int Roles::getPrefferedHeight() const
{
    return 500;
}

void Roles::add_role()
{
    AddRole * add = new AddRole(this);
    if(add->exec())
    {
        QString role = add->getRole();
        QSqlQuery query;
        query.exec("insert into modulerole(moduleid, roleid) \
                   select  mid, rid from ( \
                           select r.id as rid, m.id as mid from roles r cross join modules m ) m \
                   where not exists(select id from modulerole where moduleid = mid and roleid = rid) and rid is not null and mid is not null");

        LoadRoles();
        int row = ui->tblRoles->getRowByField("rolename", role);
        ui->tblRoles->selectRow(row);
    }
    delete add;
}

void Roles::del_role()
{
    if(ui->tblRoles->model() &&
        ui->tblRoles->model()->rowCount() > 0)
    {
        int row = ui->tblRoles->getCurrentRow();
        QString id = ui->tblRoles->getCurrentData("id").toString();
        QMessageBox::StandardButton ret = QMessageBox::question(this, \
                           tr("Warning"), tr("Are you sure you'd like to delete the role selected?"),\
                           QMessageBox::Yes | QMessageBox::No);
           if (ret == QMessageBox::Yes)
           {
               QSqlQuery query;
               if(!query.exec("delete from roles where id = " + id))
               {
                   QMessageBox::critical(this, tr("Error"), tr("Error deleting the role.\n%1").arg(query.lastError().databaseText()));
                   return;
               }
               LoadRoles();
           }
    }
}

void Roles::menu_changed(int r)
{
    QString menuid = ui->cmbMenu->itemData(r).toString();
    if(ui->tblRoles->model() && ui->tblRoles->model()->rowCount())
    {
        QString roleid = ui->tblRoles->getCurrentData("id").toString();
        LoadModules(roleid, menuid);
    }
}

void Roles::role_changed(int row)
{
    QString role = ui->tblRoles->getData(row, "ID").toString();
    QString menuid = ui->cmbMenu->itemData(ui->cmbMenu->currentIndex()).toString();
    LoadModules(role, menuid);
}


void Roles::LoadModules(const QString &roleid, const QString & menuid )
{
    QString sql = "select r.id, r.available, r.ro, m.caption from modules m inner join modulerole r on r.moduleid = m.id ";
    QString where = " where r.roleid = " + roleid + " and m.menuid =" + menuid;

    ui->tblModules->clearColumns();
    ui->tblModules->addColumn("id", "r.id", tr("ID"), true, false);
    ui->tblModules->addColumn("available", "r.available", tr("Available"), false, false, "CHECKBOXA");
    ui->tblModules->addColumn("ro", "r.ro", tr("Read only"), false, false, "CHECKBOXA");

    ui->tblModules->addColumn("caption", "m.caption", tr("Caption"), true, false, "TEXTEDIT");


    ui->tblModules->setRefreshQuery(sql);
    ui->tblModules->setQuery(sql + where + " order by 3", "modulerole", "id", "r.id", false);

    ui->tblModules->horizontalHeader()->setStretchLastSection(true);
    ui->tblModules->resizeRowsToContents();

    //ui->tblModules->setColumnWidth(0,26);
    //ui->tblModules->setColumnWidth(1,26);

    ui->tblModules->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblModules->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tblModules->resizeColumnsToContents();
}

void Roles::LoadRoles()
{
    QString sql = "select id, rolename from roles ";
    ui->tblRoles->clearColumns();
    ui->tblRoles->addColumn("id","id", tr("ID"), true, false);
    ui->tblRoles->addColumn("rolename","rolename", tr("Role's name"), false, false, "TEXTEDIT","");
    ui->tblRoles->setRefreshQuery(sql);
    ui->tblRoles->setQuery(sql + " order by 2", "roles","id","id", false);

    ui->tblRoles->horizontalHeader()->setStretchLastSection(true);
    ui->tblRoles->resizeRowsToContents();

    ui->tblRoles->setColumnWidth(0,26);

    ui->tblRoles->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblRoles->setSelectionBehavior(QAbstractItemView::SelectRows);

    if(ui->tblRoles->model() && ui->tblRoles->model()->rowCount())
        role_changed(0);
}

Roles::~Roles()
{
    delete ui;
}

extern "C" Q_DECL_EXPORT LAP_AbstractModule *
            createModule(QWidget *parent, bool ro)
{
     return new Roles(parent, ro);
}

void Roles::install()
{

}

void Roles::remove()
{

}

void Roles::upgrade()
{

}
