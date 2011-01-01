#include "prop.h"
#include "ui_prop.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QListWidgetItem>

Prop::Prop(QWidget *parent, const QString &user) :
    QDialog(parent),
    ui(new Ui::Prop),
    username(user)
{
    ui->setupUi(this);

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    LoadData();
}

void Prop::ok()
{
    QSqlQuery query;
    QStringList roleids, deptids;
    roleids << "-1";
    deptids << "-1";

    for(int row = 0; row < ui->lstRoles->count(); row++)
    {
             QListWidgetItem *item = ui->lstRoles->item(row);
             if(item->checkState() == Qt::Checked)
                 roleids << item->data(Qt::UserRole).toString();
    }

    for(int row = 0; row < ui->lstDept->count(); row++)
    {
             QListWidgetItem *item = ui->lstDept->item(row);
             if(item->checkState() == Qt::Checked)
                 deptids << item->data(Qt::UserRole).toString();
    }

    query.prepare("delete from userroles where usernam = ? ");
    query.addBindValue(username);
    if(!query.exec())
    {
       QMessageBox::critical(this, tr("Error"), tr("Error in deleting user's roles.\n%1").arg(query.lastError().text()));
       return;
    }

    query.clear();

    query.prepare("insert into userroles(usernam, roleid) select ?, id from roles where id  in (" + roleids.join(",") + ")");
    query.addBindValue(username);

    if(!query.exec())
    {
       QMessageBox::critical(this, tr("Error"), tr("Error in adding roles for the user.\n%1").arg(query.lastError().text()));
       return;
    }


    query.clear();
    query.prepare("delete from userdept where usernam = ? ");
    query.addBindValue(username);
    if(!query.exec())
    {
       QMessageBox::critical(this, tr("Error"), tr("Error in deleting user's departments.\n%1").arg(query.lastError().text()));
       return;
    }

    query.clear();
    query.prepare("insert into userdept(usernam, dept) select ?, id from depts where id  in (" + deptids.join(",") + ")");
    query.addBindValue(username);

    if(!query.exec())
    {
       QMessageBox::critical(this, tr("Error"), tr("Error in adding new departments for the user.\n%1").arg(query.lastError().text()));
       return;
    }


    accept();
}

void Prop::cancel()
{
    reject();
}

void Prop::LoadData()
{
    QSqlQuery query;
    query.prepare("select d.id, d.dept, 'Y' as flag from userdept u inner join depts d on d.id = u.dept where usernam = ? \
                  union select id, dept, 'N' as flag from depts where id not in (select dept from userdept where usernam = ? )\
                  order by 2");
    query.addBindValue(username);
    query.addBindValue(username);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Erorr"), tr("The system cannot determine available departments."));
        return;
    }
    int row = 0;
    while(query.next())
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(query.value(1).toString());
        newItem->setData(Qt::UserRole, query.value(0));
        newItem->setCheckState(query.value(2).toString().trimmed() == "N" ? Qt::Unchecked : Qt::Checked);
        ui->lstDept->insertItem(row++, newItem);
    }

    query.clear();
    query.prepare("select d.id, d.rolename, 'Y' as flag from userroles u inner join roles d on d.id = u.roleid where usernam = ? \
                  union select id, rolename, 'N' as flag from roles where id not in (select roleid from userroles where usernam = ?) \
                  order by 2");
    query.addBindValue(username);
    query.addBindValue(username);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("The system cannot determine available roles."));
        return;
    }
    row = 0;
    while(query.next())
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(query.value(1).toString());
        newItem->setData(Qt::UserRole, query.value(0));
        newItem->setCheckState(query.value(2).toString().trimmed() == "N" ? Qt::Unchecked : Qt::Checked);
        ui->lstRoles->insertItem(row++, newItem);
    }
}

Prop::~Prop()
{
    delete ui;
}

void Prop::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
