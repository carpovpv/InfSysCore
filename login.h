#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
    class Login;
}

class Login : public QDialog {
    Q_OBJECT
public:
    Login(QWidget *parent = 0, bool showConfirmation = false);
    ~Login();

    bool isLogged() const { return logged; }

protected:
    void changeEvent(QEvent *e);
     bool eventFilter(QObject *, QEvent *);

private:
    Ui::Login *ui;

    bool logged;
    bool _showConfirmation;

private slots:
    void UserName_lostFocus();

    void ok();

    void cancel();
    void upper(QString );

protected:
    void closeEvent(QCloseEvent *);

};

#endif // LOGIN_H
