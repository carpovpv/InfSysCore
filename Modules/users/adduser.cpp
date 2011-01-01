#include "adduser.h"
#include "ui_adduser.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QCryptographicHash>

AddUser::AddUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);
    QSqlQuery query;
    int row = 0;
    query.exec("select id, dept from depts order by 2");
    while(query.next())
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(query.value(1).toString());
        newItem->setData(Qt::UserRole, query.value(0));
        newItem->setCheckState(Qt::Unchecked);
        ui->lstDepts->insertItem(row++, newItem);
    }
    ui->lstDepts->setSelectionMode(QAbstractItemView::ExtendedSelection);

    query.clear();

    row = 0;
    query.exec("select id, rolename from roles order by 2");
    while(query.next())
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(query.value(1).toString());
        newItem->setData(Qt::UserRole, query.value(0));
        newItem->setCheckState(Qt::Unchecked);
        ui->lstRoles->insertItem(row++, newItem);
    }
    ui->lstRoles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));

}

void AddUser::ok()
{
    QString login = ui->txtLogin->text().trimmed().toUpper();
    QString fullname = ui->txtFullname->text().trimmed();
    QString passwd = ui->txtPassword->text().trimmed();
    if(login.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please, enter a user's name."));
        return;
    }
    if(fullname.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please, enter the signature of a user."));
        return;
    }
    if(passwd.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please, enter a password for the user."));
        return;
    }

    QStringList roles, depts;
    roles << "-1";
    depts << "-1";

    for(int row = 0; row < ui->lstRoles->count(); row++)
    {
             QListWidgetItem *item = ui->lstRoles->item(row);
             if(item->checkState() == Qt::Checked)
                 roles << item->data(Qt::UserRole).toString();
    }

    for(int row = 0; row < ui->lstDepts->count(); row++)
    {
             QListWidgetItem *item = ui->lstDepts->item(row);
             if(item->checkState() == Qt::Checked)
                 depts << item->data(Qt::UserRole).toString();
    }
    if(depts.size() == 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please, select user's departments."));
        return;
    }

    QByteArray hash = QCryptographicHash::hash(passwd.toAscii(),
                                               QCryptographicHash::Md5);

    QString hd = "";
    for(int i=0; i< hash.length(); ++i)
        hd += QString::number(hash[i]);

    QSqlQuery query;
    query.prepare("insert into users(usernam, fullname, \"PASSWORD\", status) values(?,?,?,?)");
    query.addBindValue(login);
    query.addBindValue(fullname);
    query.addBindValue(hd);
    query.addBindValue("A");
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("Error in adding a user.\n%1").arg(query.lastError().text()));
        return;
    }

    query.clear();

    query.prepare("insert into userdept(usernam, dept) select ?, id from depts where id in (" + depts.join(",") + ") ");
    query.addBindValue(login);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("Error in adding user's departments.\n%1").arg(query.lastError().text()));
        return;
    }

    query.clear();

    query.prepare("insert into userroles(usernam, roleid) select ?, id from roles where id  in (" + roles.join(",") + ")");
    query.addBindValue(login);

    if(!query.exec())
    {
       QMessageBox::critical(this, tr("Error"), tr("Error in adding new user's roles.\n%1").arg(query.lastError().text()));
       return;
    }

    usernam = login;
    accept();

}

const QString & AddUser::getUsername() const
{
    return usernam;
}

void AddUser::cancel()
{
    reject();
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::changeEvent(QEvent *e)
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
