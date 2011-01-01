#ifndef ROLES_H
#define ROLES_H

#include <QWidget>
#include "Roles_global.h"

namespace Ui
{
    class Roles;
}

/*!
  The module allows to create new roles and select those modules that must be avaiulable under it.
  */

class Roles : public LAP_AbstractModule
{
    Q_OBJECT
public:
    Roles(QWidget *, bool ro = false);
    ~Roles();

    void install();
    void remove();
    void upgrade();

    int getPrefferedWidth() const;
    int getPrefferedHeight() const;

private:
    Ui::Roles *ui;

private slots:

    void add_role();
    void del_role();
    void menu_changed(int);
    void role_changed(int);

    void LoadRoles();
    void LoadModules(const QString &, const QString &);

};

#endif // ROLES_H
