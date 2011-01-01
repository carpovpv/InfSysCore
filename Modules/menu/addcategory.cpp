#include "addcategory.h"
#include "ui_addcategory.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

AddCategory::AddCategory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCategory),
    id("-1")
{

    ui->setupUi(this);

    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));

}

const QString & AddCategory::getId()
{
    return id;
}

void AddCategory::ok()
{
    id = "-1";

    QString cat = ui->txt->text().trimmed();
    if(cat.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter the category."));
        return;
    }
    QSqlQuery query;
    query.prepare("insert into menucategory(menu) values(?)");
    query.addBindValue(cat);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("Error adding the new category.\n%1").arg(query.lastError().text()));
        return;
    }
    id = query.lastInsertId().toString();

    accept();

}

void AddCategory::cancel()
{
    reject();
}

AddCategory::~AddCategory()
{
    delete ui;
}

void AddCategory::changeEvent(QEvent *e)
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
