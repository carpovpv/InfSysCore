#ifndef MENU_H
#define MENU_H

#include <QWidget>

#include "../../lap_abstractmodule.h"

namespace Ui {
    class Menu;
}

/*!
  The module allows to create a menu, it operates with modules, their captions, icons and locations.
  */

class Menu : public LAP_AbstractModule {
    Q_OBJECT
public:
    Menu(QWidget *parent = 0, bool ro = false);
    ~Menu();

    void install();
    void remove();
    void upgrade();

protected:
    void changeEvent(QEvent *e);

private:

    Ui::Menu *ui;
    void LoadCategory();

private slots:

    void add_category();
    void del_category();

    void add_module();
    void del_module();
    void change_icon_module();

    void change_icon();

    void LoadModules(int r);

};

#endif // MENU_H
