#include "addmenu.h"
#include "ui_addmenu.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

AddMenu::AddMenu(QWidget *parent, const QString &_menuid) :
    QDialog(parent),
    ui(new Ui::AddMenu),
    id("-1"),
    menuid(_menuid)
{
    ui->setupUi(this);
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

void AddMenu::ok()
{
    id = "-1";
    QString txt = ui->lineEdit->text().trimmed();
    if(txt.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter menu's caption."));
        return;
    }
    QSqlQuery query;
    query.prepare("insert into modules(caption, menuid) values(?,?)");
    query.addBindValue(txt);
    query.addBindValue(menuid);
    if(!query.exec())
    {
        QMessageBox::critical(this, tr("Error"), tr("Error adding new menu entry.").arg(query.lastError().text()));
        return;
    }

    id = query.lastInsertId().toString();

    query.clear();
    query.exec("insert into modulerole(moduleid, roleid) \
                    select  mid, rid from ( \
                            select r.id as rid, m.id as mid from roles r cross join modules m ) m \
                    where not exists(select id from modulerole where moduleid = mid and roleid = rid) and rid is not null and mid is not null");

    accept();

}

const QString & AddMenu::getId() const
{
    return id;
}

void AddMenu::cancel()
{
    reject();
}

AddMenu::~AddMenu()
{
    delete ui;
}

void AddMenu::changeEvent(QEvent *e)
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
