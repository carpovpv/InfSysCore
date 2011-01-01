#ifndef ADDROLE_H
#define ADDROLE_H

#include <QDialog>

namespace Ui {
    class AddRole;
}

class AddRole : public QDialog {
    Q_OBJECT
public:
    AddRole(QWidget *parent = 0);
    ~AddRole();

    const QString & getRole() const;

protected:
    void changeEvent(QEvent *e);


private:
    Ui::AddRole *ui;
    QString role;
private slots:

    void ok();
    void cancel();

};

#endif // ADDROLE_H
