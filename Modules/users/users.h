#ifndef USERS_H
#define USERS_H

#include <QWidget>

#include "Users_global.h"
#include "../../lap_abstractmodule.h"

extern "C" Q_DECL_EXPORT LAP_AbstractModule *
            createModule(QWidget *, bool ro);

namespace Ui
{
    class Users;
}

/*!
  User managment. This module allows to add, to delete and to modify user's information.
 */

class Users : public LAP_AbstractModule
{
    Q_OBJECT
public:
    Users(QWidget * parent, bool ro = false);
    ~Users();

    void install();
    void upgrade();
    void remove();

    int getPrefferedWidth() const;

private:

    Ui::Users * ui;

private slots:

    void LoadData();
    void add_user();
    void del_user();
    void prop_user();
    void change_password();

};

#endif // USERS_H
