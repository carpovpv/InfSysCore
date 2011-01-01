#ifndef LAP_CORE_H
#define LAP_CORE_H

#include <QMainWindow>
#include <QVector>
#include <QMap>

namespace Ui {
    class LAP_Core;
}

class LAP_DateEdit;
class LAP_Info;
class QLabel;
class iisTaskBox;
class iisIconLabel;
class QMdiSubWindow;
class LAP_AbstractModule;


struct lap_module
{
    const QObject * obj;
    LAP_AbstractModule * module;
};

struct lap_info
{
    QString moduleName;
    QString file;
};

/*!
    The basic  class  with  MDI  interface. At the  left side the
    menu is generated according to the role  and the availability
    of dynamic linked modules. The main  widget is MDI area where
    all loaded windows are  placed. Any information system manage
    some  data  by  several users. One  of them can do one thing,
    other can  do  another  thing. For example,  several  workers
    from  warehouse may create  requests that some  goods are all
    sold and they must be bought. Economist review this  requests
    according to the current policy  of  a firm. Some of them  he
    approves, some - rejectes. In this example we have two groups
    of users. The users of the first group can create  a  request
    and modify  it, but they  cannot confirm it. The confirmation
    is done by the users from the  second role.  In  applications
    there  are  a  lot of roles. Each application or a  module is
    available or not under a particular role. When a  user logins
    he selects the department and the  role. The  departments and
    roles are set to the user  int Users class. The roles may  be
    created manually by Roles class. Once the  role created it is
    needed to select wich modules (or applications) are available
    for users logged  into the system by this role.  Modules  are
    created by  inheritance  from  LAP_AbstractModule class. Each
    module must be in a separate  dll (so) file. The link to this
    file with the icon and caption for the application is created
    in Menu class. So  the  basic  core  may  work with different
    projects. Only  the settings are differ from one  project  to
    another. If you wish to add new functionality to  a  project,
    but you know that you have created the module before in other
    project than you can simply send a binary dll or (so) file to
    the customer.
  */

class LAP_Core : public QMainWindow {
    Q_OBJECT
public:
    LAP_Core(QWidget *parent = 0);
    ~LAP_Core();

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *, QEvent *);

private:

    Ui::LAP_Core *ui;
    LAP_DateEdit * dt;
    LAP_Info * info;

    QLabel *stDb;
    QLabel *stUser;
    QLabel *stInfo;
    iisTaskBox * ctask;
    QWidget * spacer;

    QVector <iisTaskBox *> menuentries;

    QMap <const QObject *, QString > modules;
    QMap <const QObject *, bool > ros;

    QMap <const QObject *, QMdiSubWindow *> loaded;
    QMap <const QObject *, struct lap_module> widgets;
    QMap <QMdiSubWindow *, struct lap_info > wins;

    void save_applications(const QString & user);

    QMdiSubWindow * getMdiWindow(const QString &);

    int shutcounts;
    int minutes;
    QLabel * lbl;

private slots:

    /*!
      Load the menu according to the settings. For each role
      the menu is defined in Roles class.
      */
    void load_menu();

    /*!
      Change the user and reload the menu.
      */

    void change_user();

    /*!
      If the module is loaded the core just raise its windiw, otherwise
      the so module  is  loaded  and  the module is instantiated by the
      create function.

      */
    void module_activated();

    /*!
      Delete module. Save its coordinates for future use.
      */
    void module_destroyed();

    /*!
      Loade the module.
     */
    void LoadModule(const QString &, const QObject *);

    /*!
      Send the request to reload the data in a module.
      */
    void SendLoadDataWindow(const QString & modulename, const QString & sql);

    void timeout();
    void timer_seconds();

protected:

   /*!
     Close the session. All loaded modules will be saved with their coordinates
     in order to launch them in next program's session.
     */
    void closeEvent(QCloseEvent *);

};

#endif // LAP_CORE_H
