#ifndef LAP_Info_H
#define LAP_Info_H

#include <QObject>
#include <QDateTime>
#include <QSettings>

class QSqlDatabase;
class QSqlQuery;
class QMessageBox;
class QTimer;

#include "../../globals.h"

/*!
  The basic signleton that holds the database context, timer, convinent function
  for generating unique id and several function  to  operate the  settings. When
  the application loads  this  class  creates  first. It tries to connect to the
  database, set timer. If you need to  obtain the  current user's  name  or dept
  then you must  get a pointer  to his class and execute the corresponding proc.
  After using the class the freeLAP_Info() function must be executed.

  */

class Q_DECL_EXPORT  LAP_Info : public QObject
{
Q_OBJECT
public:
    static LAP_Info * getLAP_Info();
    void freeLAP_Info();

    /*! User defined function to create a new ID.
      */
    const QString  GetId(const QString & table);

    bool isDb() const { return dbstatus;}
    /*!
      Current user.
      */
    const QString & getCurrentUser() const { return user; }
    /*!
      Current role.
    */
    const QString & getCurrentRole() const { return role; }
    /*!
      Timestamp when a user begin to work with a propgam.
    */
    const QDateTime &getStartDateTime() const { return dtStart; }
    /*!
      Home directory.
    */
    const QString & getHomePath() const { return HomePath; }
    /*!
      Current department.
    */
    const QString & getCurrentDept() const { return dept; }

    void setLoginInfo(const QString & username,
                      const QString & dept,
                      const QString & role,
                      const QString & deptcode,
                      const QString & userid);
    /*!
      Get the value of a property from the settings table in the database.
      */
    const QString  GetProperty(const QString & prop);
    /*!
      Get the value of a property from the client's settings (ini files or registry).
      */
    const QVariant GetClientSetting(const QString & prop, const QVariant &def);
    /*!
      Set the value of a proeprty in the client's settings.
      */
    void SetClientSetting(const QString & prop, const QVariant & value);

    bool isOk() const;
    const QString & getError() const;
    const QString getDbInfo() const;
    QString stringDecrypt(QString strInput, QString strPassword);
    QString stringEncrypt(QString strInput, QString strPassword);
    bool reconnect();

private:

    static LAP_Info *info;
    int counter;
    QTimer *timer;

    LAP_Info( QObject *parent = 0);
    virtual ~LAP_Info();

    QString user, role, dept, deptcode, userId;

    QDateTime dtStart;
    bool dbstatus;
    QString logid;
    QString HomePath;

    QSettings *settings;
    bool ok;

    QString driver;
    QString login;
    QString password;
    QString server;
    QString database;

    QString err;

signals:
    void timer_elapsed();

public slots:
    void timer_event();

};

#endif // LAP_Info_H
