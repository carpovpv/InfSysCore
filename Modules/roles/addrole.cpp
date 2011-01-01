#include "addrole.h"
#include "ui_addrole.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

AddRole::AddRole(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRole),
    role("")
{
    ui->setupUi(this);
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

void AddRole::ok()
{
    QString txt = ui->txtRole->text().trimmed();
    if(txt.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Enter, please, the role's name."));
        return;
    }
    role = "";
    QSqlQuery query;
    query.prepare("insert into roles(rolename) values(?)");
    query.addBindValue(txt);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("The system cannot add new role.\n%1").arg(query.lastError().text()));
        return;
    }

    role = txt;
    accept();
}

const QString & AddRole::getRole() const
{
    return role;
}

void AddRole::cancel()
{
    reject();
}

AddRole::~AddRole()
{
    delete ui;
}

void AddRole::changeEvent(QEvent *e)
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
