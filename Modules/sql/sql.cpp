#include "sql.h"
#include "ui_sql.h"

#include <QSqlQueryModel>

extern "C" Q_DECL_EXPORT LAP_AbstractModule *
        createModule(QWidget *parent, bool ro)
{
    return new Sql(parent, ro);
}

void Sql::install(){}
void Sql::remove(){}
void Sql::upgrade(){};

Sql::Sql(QWidget *parent, bool ro) :
    LAP_AbstractModule(parent, ro),
    ui(new Ui::Sql)
{
    ui->setupUi(this);
    moduleName = "Sql Editor";
    connect(ui->btnExecute, SIGNAL(clicked()), this, SLOT(execute_sql()));
    connect(ui->txtSql, SIGNAL(returnPressed()), this, SLOT(execute_sql()));

    ui->txtSql->setFocus();
}

void Sql::execute_sql()
{
    QString sql = ui->txtSql->text();
    if(sql.isEmpty())
        return;

    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(sql);

    ui->tblData->setModel(model);

}

Sql::~Sql()
{
    delete ui;
}

void Sql::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
