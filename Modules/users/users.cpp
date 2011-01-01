#include "users.h"
#include "ui_users.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QCryptographicHash>
#include "adduser.h"
#include "prop.h"

extern "C" Q_DECL_EXPORT LAP_AbstractModule *
            createModule(QWidget *parent, bool ro)
{
     return new Users(parent,ro);
}


void Users::install()
{

}

void Users::upgrade()
{

}

void Users::remove()
{

}

int Users::getPrefferedWidth() const
{
    return 900;
}

Users::Users(QWidget *parent, bool ro)
    : LAP_AbstractModule(parent, ro),
    ui(new Ui::Users)
{
    ui->setupUi(this);

    ui->cmbStatus->addItem(tr("All"),"-1");

    QSqlQuery query;
    query.exec("select status, statusname from statuses order by 2");
    while(query.next())
        ui->cmbStatus->addItem(query.value(1).toString(), query.value(0));

    LoadData();

    moduleName = "LAP_Users";
    connect(ui->cmbStatus, SIGNAL(currentIndexChanged(int)), this, SLOT(LoadData()));

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(add_user()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(del_user()));
    connect(ui->btnProperties, SIGNAL(clicked()), this, SLOT(prop_user()));
    connect(ui->btnPas, SIGNAL(clicked()), this, SLOT(change_password()));
}

void Users::change_password()
{
    if(!(ui->tbl->model() && ui->tbl->model()->rowCount() > 0))
        return;

    QString name = ui->tbl->getCurrentData("u.usernam").toString();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter a new password"),
                                         tr("New user's password:"), QLineEdit::Normal,
                                              "", &ok);
    if (ok && !text.isEmpty())
    {
        QByteArray hash = QCryptographicHash::hash(text.toAscii(),
                                                   QCryptographicHash::Md5);

        QString hd = "";
        for(int i=0; i< hash.length(); ++i)
            hd += QString::number(hash[i]);

        QSqlQuery query;
        query.prepare("update users set \"PASSWORD\" = ? where usernam = ?");
        query.addBindValue(hd);
        query.addBindValue(name);
        if(!query.exec())
        {
            QMessageBox::critical(this, tr("Error"), tr("Changing the password caused an error.\n%1").arg(query.lastError().text()));
        }
    }
}

void Users::prop_user()
{
    if(!(ui->tbl->model() && ui->tbl->model()->rowCount() > 0))
        return;

    QString name = ui->tbl->getCurrentData("u.usernam").toString();

    Prop * p = new Prop(this, name);
    p->exec();
    delete p;
}

void Users::del_user()
{
    if(!(ui->tbl->model() && ui->tbl->model()->rowCount() > 0 )) return;

    QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Confirmation"),\
                                                        tr("Are you sure you'd like to delete the user?"), \
                                                        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::No)
        return ;

    QString id = ui->tbl->getCurrentData("ID").toString();
    if(id == "-1") return;

    QSqlQuery query;
    if(!query.exec("delete from users where id = " + id))
    {
        QMessageBox::warning(this, tr("Error"),tr("Error occured while deleting the user.\n%1").arg(query.lastError().text()));
        return;
    }

    LoadData();
}

void Users::add_user()
{
    AddUser * add = new AddUser(this);
    if(add->exec())
    {
        QString usernam = add->getUsername();
        ui->cmbStatus->setCurrentIndex(0);
        LoadData();
        int row = ui->tbl->getRowByField("u.usernam", usernam);
        ui->tbl->selectRow(row);
    }
    delete add;
}

void Users::LoadData()
{

    QString where = ui->cmbStatus->itemData(ui->cmbStatus->currentIndex()).toString();
    if (where == "-1") where = "";
    else where = " where u.status = '" + where + "' ";

    QString sql = "select u.id, u.usernam, s.statusname, u.fullname from users u inner join statuses s on s.status = u.status  ";
    ui->tbl->clearColumns();
    ui->tbl->addColumn("id", "u.id", tr("ID"), true, false);
    ui->tbl->addColumn("usernam","u.usernam", tr("Username"), true, false, "TEXTEDIT");
    ui->tbl->addColumn("status", "s.statusname", tr("Status"), false, false, "COMBOBOX","select status, statusname from statuses order by 2");
    ui->tbl->addColumn("fullname","u.fullname", tr("Signature"), false, false, "TEXTEDIT","");

    ui->tbl->setRefreshQuery(sql);
    ui->tbl->setQuery(sql + where + " order by 2", "users", "id","u.id", false);


    ui->tbl->horizontalHeader()->setStretchLastSection(true);
    ui->tbl->resizeRowsToContents();

    ui->tbl->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
}

Users::~Users()
{
    delete ui;
}
