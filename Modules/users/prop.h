#ifndef PROP_H
#define PROP_H

#include <QDialog>

namespace Ui {
    class Prop;
}

class Prop : public QDialog {
    Q_OBJECT
public:
    Prop(QWidget *parent = 0, const QString &user =0);
    ~Prop();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Prop *ui;
    QString username;

    void LoadData();
private slots:

    void ok();
    void cancel();

};

#endif // PROP_H
