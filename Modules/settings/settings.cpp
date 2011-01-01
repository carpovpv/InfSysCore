#include "settings.h"
#include "ui_settings.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

extern "C" Q_DECL_EXPORT LAP_AbstractModule *
        createModule(QWidget *parent, bool ro)
{
    return new Settings(parent, ro);
}

void Settings::install(){}
void Settings::remove(){}
void Settings::upgrade(){};

Settings::Settings(QWidget *parent, bool ro) :
    LAP_AbstractModule(parent, ro),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    moduleName = "Settings";
    version = "1.0";

    QString sql = "select id, setting_name, setting_value from settings";
    ui->tbl->clearColumns();
    ui->tbl->addColumn("id","id", tr("#"), true, false);
    ui->tbl->addColumn("setting_name","setting_name", tr("Name"), true, false);
    ui->tbl->addColumn("setting_value","setting_value",tr("Value"), false, false, "TEXTEDIT","");
    ui->tbl->setRefreshQuery(sql);
    ui->tbl->setQuery(sql + " order by 2", "settings", "id","id", false);
    ui->tbl->resizeRowsToContents();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::changeEvent(QEvent *e)
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

