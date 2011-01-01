#ifndef ADDCATEGORY_H
#define ADDCATEGORY_H

#include <QDialog>

namespace Ui {
    class AddCategory;
}

class AddCategory : public QDialog {
    Q_OBJECT
public:
    AddCategory(QWidget *parent = 0);
    ~AddCategory();
    const QString & getId();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddCategory *ui;
    QString id;

private slots:

    void ok();
    void cancel();

};

#endif // ADDCATEGORY_H
