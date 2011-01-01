#ifndef Settings_H
#define Settings_H

#include "lap_abstractmodule.h"

namespace Ui {
    class Settings;
}

/*!
  Basic settings for the program.
  */

class Settings : public LAP_AbstractModule {
    Q_OBJECT
public:
    Settings(QWidget *parent = 0, bool ro = false);
    ~Settings();

    void install();
    void remove();
    void upgrade();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Settings *ui;

};

#endif // Settings_H
