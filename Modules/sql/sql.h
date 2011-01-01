#ifndef SQL_H
#define SQL_H

#include "../../lap_abstractmodule.h"

namespace Ui {
    class Sql;
}

/*!
  The module simply allows to execute any SQL statement.
  */

class Sql : public LAP_AbstractModule {
    Q_OBJECT
public:
    Sql(QWidget *parent = 0, bool ro = false);
    ~Sql();

    void install();
    void remove();
    void upgrade();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Sql *ui;

private slots:
    void execute_sql();

};

#endif // SQL_H
