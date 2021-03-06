#include "lap_info.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QTimer>

/*!
 This class hold the database's context, timers, system settings and etc.
  */

LAP_Info::LAP_Info(QObject *parent) :
    QObject(parent)
{
    settings =new QSettings(AppFirm, AppName);

    QString PASSWORD_CODE=stringEncrypt(PASSWORD, "LIMS");

    if(!settings->contains("DRIVER"))SetClientSetting("DRIVER",DRIVER);
    if(!settings->contains("SERVER"))SetClientSetting("SERVER",SERVER);
    if(!settings->contains("DATABASE"))SetClientSetting("DATABASE",DATABASE);
    if(!settings->contains("LOGIN"))SetClientSetting("LOGIN",LOGIN);
    if(!settings->contains("PASSWORD"))SetClientSetting("PASSWORD",PASSWORD_CODE);

    driver =LAP_Info::GetClientSetting("DRIVER",DRIVER).toString();
    server =LAP_Info::GetClientSetting("SERVER",SERVER).toString();
    database =LAP_Info::GetClientSetting("DATABASE",DATABASE).toString();
    login =LAP_Info::GetClientSetting("LOGIN",LOGIN).toString();

    QString password_code =LAP_Info::GetClientSetting("PASSWORD",PASSWORD_CODE).toString();
    password =stringDecrypt(password_code, "LIMS");
    ok = false;

    reconnect();


    QDir dir;
    if(!dir.exists(::HomePath))
        dir.mkpath(::HomePath);

    HomePath = ::HomePath;

    timer = new QTimer(this);
    timer->setInterval(1000 * 60);
    timer->start();

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_event()));

}

const QString LAP_Info::GetId(const QString &table)
{
    QSqlQuery query;
    query.exec("select GEN_ID(gen_"+table+"_id,1) from rdb$database");
    if(query.next())
    {
        return QString(query.value(0).toString());
    }
    return "-1";
}

void LAP_Info::timer_event()
{
    emit timer_elapsed();
}

LAP_Info * LAP_Info::getLAP_Info()
{
    if(!info)
    {

        info = new LAP_Info();
        info->counter = 1;
        if(!info->isOk())
            info->dbstatus = false;

        else
        {
            info->dbstatus = true;
        }
    }
    else
    {

        info->counter++;
    }
    return info;
}

void LAP_Info::setLoginInfo(const QString & _user, const QString & _dept, const QString & _role, const QString & _deptcode, const QString & _userId)
{
    info->role = _role;
    info->user = _user;
    info->dept = _dept;
    info->deptcode = _deptcode;
    info->userId = _userId;
    info->dtStart = QDateTime::currentDateTime();

    info->logid = "-1";

    QSqlQuery query;
    query.prepare("insert into loguser(usernam, date_start) values(?, current_timestamp)");
    query.addBindValue(_user);
    if(query.exec())
    {
        query.clear();
        query.prepare("select max(id) from loguser where usernam = ?");
        query.addBindValue(_user);

        if(query.exec() && query.next())
            info->logid = query.value(0).toString();


    }

}

LAP_Info::~LAP_Info()
{
    if(info->logid != "-1")
    {
        QSqlQuery query;
        query.exec("update loguser set date_end = current_timestamp where id = " + info->logid);
    }
    if(ok)
    {
        QSqlDatabase::removeDatabase(driver);
    }
    delete settings;

}

void LAP_Info::freeLAP_Info()
{
    info->counter--;
    if(info->counter==0)
    {
        delete this;
    }
}

const QString LAP_Info::getDbInfo() const
{
    return QString(driver + ":" + server + "/" + database);
}

const QString & LAP_Info::getError() const
{
    return err;
}

bool LAP_Info::reconnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);

    db.setHostName(server);
    db.setDatabaseName(database);
    db.setUserName(login);
    db.setPassword(password);

    ok = db.open();
    err = db.lastError().driverText();

    return ok;
}

bool LAP_Info::isOk() const
{
    return ok;
}

const QString LAP_Info::GetProperty(const QString &prop)
{
    QSqlQuery query;
    if(!query.exec("select setting_value from settings where setting_name ='" + prop + "'"))
        return QString();

    query.next();
    return QString(query.value(0).toString());
}

/*!
    Return the value of a property saved on the client..
  */

const QVariant LAP_Info::GetClientSetting(const QString & prop, const QVariant & def)
{
    return settings->value(prop,def);
}

/*!
    Set the value for property on the client.
  */

void LAP_Info::SetClientSetting(const QString & prop, const QVariant & value)
{
    settings->setValue(prop,value);
}
LAP_Info *LAP_Info::info = NULL;

QString LAP_Info::stringEncrypt(QString strInput, QString strPassword)
{
    int i,j;
    QString strOutput="";
    QByteArray baInput    = strInput.toLocal8Bit();
    QByteArray baPassword = strPassword.toLocal8Bit();
    for (i=0; i < baInput.size(); i++)
    {   for (j=0; j < baPassword.size(); j++)
        {
            baInput[i] = baInput[i] ^ (baPassword[j] + (i*j));
        }
        strOutput += QString("%1").arg((int)((unsigned char)baInput[i]),2,16,QChar('0'));
    }
    return strOutput;
}

QString LAP_Info::stringDecrypt(QString strInput, QString strPassword)
{
    int i,j;
    QByteArray baInput    = QByteArray::fromHex(strInput.toLocal8Bit());
    QByteArray baPassword = strPassword.toLocal8Bit();
    for (i=0; i < baInput.size(); i++)
    {   for (j=0; j < baPassword.size(); j++)
        {
            baInput[i] = baInput[i] ^ (baPassword[j] + (i*j));
        }
    }
    return QString::fromLocal8Bit(baInput.data());
}
