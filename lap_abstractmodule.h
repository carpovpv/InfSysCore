#ifndef LAP_ABSTRACTMODULE_H
#define LAP_ABSTRACTMODULE_H

#include <QString>
#include <QWidget>
#include <QCloseEvent>

#include "Modules/info/lap_info.h"

/*!
  The abstract module. The core operates with modules by the pointers to the
  LAP_AbstractModule class. In a particular application  each  class derived
  from the abstract class must be in a separate dll or so file. The core loads
  this file (the file is returned from the menu entry) and creates an instance.
  Also the core catches the destroy signal of LAP_AbstractModule widget and
  free its memory. The module may have readonly access for some roles. It is
  set in Role class. The core set the readonly flag when loading the module.
  You as a developer must create some code for accounting for this flag.
 */

class LAP_AbstractModule : public QWidget
{
    Q_OBJECT

public:

    LAP_AbstractModule(QWidget * parent, bool ro = false);
    virtual ~LAP_AbstractModule();

    /*!
      The name of the module.
      */
    const QString & getModuleName() const;
    /*!
      Module's version. Not used.
      */
    const QString & getVersion() const;

    /*!
      Install.  The thing is in the  following. Each module
      uses some  tables in the  database. In this  function
      you may create the  appropriate  code  to create this
      tables and do some data modifications. So here is the
      right place to put here all DDL and DML statements to
      prepare the database to work with new module.
      */
    virtual void install() = 0;
    /*!
      Not used. Some things about autoupdate.
      */
    virtual void upgrade() = 0;
    /*!
      Not used. Delete all data that was used or created by
      this module.
      */
    virtual void remove()  = 0;

    /*!
      If the widget of this model must be application modal
      then you must return true in this function.
     */
    virtual bool isModal() const ;
    /*!
      The preffered size for the widget. After closeing each
      module, the system stores its weight and height params
      in the clients setttings.
      */
    virtual int getPrefferedWidth() const;
    virtual int getPrefferedHeight() const;

signals:

    /*!
      After the module is closed the core free all its resources.
      */
    void closed();

    void LoadDataWindow(const QString &, const QString &);

public slots:

    /*!
      LAP_Info class provides a timer in one minute. If you need
      dinamically refresh data in your module, then reimplement
      this function.
      */

    virtual void timer_refresh();

    /*!
     Communication with other windows.
     */

    virtual void LoadDataWindowRequest(const QString &){}

private:

    const LAP_AbstractModule & operator=(const LAP_AbstractModule &);
    LAP_AbstractModule(const LAP_AbstractModule &);

protected:
    QString moduleName;
    QString version;

    bool m_readonly;
    void closeEvent(QCloseEvent *);
};

#endif // LAP_ABSTRACTMODULE_H
