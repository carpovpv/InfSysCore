#include "login.h"
#include "ui_login.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QSqlError>
#include <QValidator>

#include "Modules/info/lap_info.h"

Login::Login(QWidget *parent, bool showConfirmation) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    _showConfirmation = showConfirmation;

    connect(ui->txtUserName,SIGNAL(textEdited(QString)), this, SLOT(upper(QString)));

    connect(ui->txtUserName, SIGNAL(editingFinished()), this, SLOT(UserName_lostFocus()));

    ui->cmbDept->setEnabled(false);
    ui->cmbRole->setEnabled(false);

    ui->txtPassword->setEchoMode(QLineEdit::Password);

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    logged = false;

    setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint);
    setMaximumSize(size());
    setMinimumSize(size());

    ui->txtUserName->setFocus();

    connect(ui->txtUserName, SIGNAL(editingFinished()), ui->txtPassword, SLOT(setFocus()));
    connect(ui->txtPassword, SIGNAL(editingFinished()), ui->cmbRole, SLOT(setFocus()));

    ui->cmbRole->installEventFilter(this);
    ui->btnOk->installEventFilter(this);
}

bool Login::eventFilter(QObject *obj, QEvent *evt)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(evt);
    if (evt->type() == QEvent::KeyPress)
    {
        int key = keyEvent->key();
        if(obj == ui->cmbRole)
        {
             if( key == Qt::Key_Return || key == Qt::Key_Enter)
                 ok();
         }
         else if(obj == ui->btnOk)
             ok();

    }
    return QDialog::eventFilter(obj, evt);
}

void Login::upper(QString str)
{
    ui->txtUserName->setText(str.toUpper());
}

void Login::ok()
{
    QString usernam = ui->txtUserName->text().trimmed().toUpper();
    QString password = ui->txtPassword->text().trimmed();

    QByteArray hash = QCryptographicHash::hash(password.toAscii(),
                                               QCryptographicHash::Md5);

    QString hd = "";
    for(int i=0; i< hash.length(); ++i)
        hd += QString::number(hash[i]);

    QSqlQuery query;

    if(!(ui->cmbDept->count() && ui->cmbRole->count()))
    {
        QMessageBox::warning(this, tr("Not all data was entered"), tr("You must supply your login, password, department and role."));
        return;
    }

    query.clear();
    query.exec("select id from users where status ='A' and usernam ='" + usernam + "' and \"PASSWORD\" ='" + hd +"'");
    if(query.next())
    {
        logged = true;

        QString userid = query.value(0).toString();
        query.clear();
        query.exec("select deptcode from depts where dept = '" + ui->cmbDept->currentText() +"'" );
        query.next();

        QString _deptcode = query.value(0).toString();

        LAP_Info *info = LAP_Info::getLAP_Info();
        info->setLoginInfo(usernam,
                           ui->cmbDept->currentText(),
                           ui->cmbRole->currentText(),
                           _deptcode,
                           userid );
        info->freeLAP_Info();
        accept();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("The system doesn't find such a user."));
        return;
    }

}

void Login::cancel()
{
     if(_showConfirmation)
     {
         QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Confirmation"),\
                                                         tr("Would you like to exit the system?"), \
                                                         QMessageBox::Yes | QMessageBox::No);
         if (ret == QMessageBox::Yes)
        {
           logged = false;
            reject();
         }
    }
    else
     {
        logged = false;
         reject();
     }
}


void Login::UserName_lostFocus()
{
    QString usernam = ui->txtUserName->text().trimmed().toUpper();

    if(usernam.isEmpty())
        return;

    ui->cmbDept->clear();
    ui->cmbRole->clear();

    QSqlQuery query;
    query.exec("select distinct u.dept, d.dept from userdept u inner join depts d on d.id = u.dept where u.usernam ='" + usernam + "'");
    while(query.next())
        ui->cmbDept->addItem(query.value(1).toString(), query.value(0));

    ui->cmbDept->setEnabled(ui->cmbDept->count());

    query.clear();
    query.exec("select distinct u.roleid, r.rolename from userroles u inner join roles r on r.id = u.roleid where u.usernam = '" + usernam + "'");
    while(query.next())
        ui->cmbRole->addItem(query.value(1).toString(), query.value(0));

    ui->cmbRole->setEnabled(ui->cmbRole->count());

}

Login::~Login()
{
    delete ui;
}

void Login::changeEvent(QEvent *e)
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

void Login::closeEvent(QCloseEvent *event)
{
    event->ignore();
    cancel();
}
