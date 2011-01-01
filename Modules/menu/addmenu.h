#ifndef ADDMENU_H
#define ADDMENU_H

#include <QDialog>

namespace Ui {
    class AddMenu;
}

class AddMenu : public QDialog {
    Q_OBJECT
public:
    AddMenu(QWidget *parent = 0, const QString & menuid = 0);
    ~AddMenu();
    const QString & getId() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddMenu *ui;
    QString id;
    QString menuid;

private slots:

    void ok();
    void cancel();
};

#endif // ADDMENU_H
