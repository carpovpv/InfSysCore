#ifndef ADDUSER_H
#define ADDUSER_H

#include <QDialog>

namespace Ui {
    class AddUser;
}

class AddUser : public QDialog {
    Q_OBJECT
public:
    AddUser(QWidget *parent = 0);
    ~AddUser();
    const QString & getUsername() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddUser *ui;
    QString usernam;

private slots:

    void ok();
    void cancel();
};

#endif // ADDUSER_H
