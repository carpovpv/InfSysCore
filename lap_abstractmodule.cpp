
#include "lap_abstractmodule.h"

#include <QCloseEvent>
#include <QMessageBox>

bool LAP_AbstractModule::isModal() const
{
    return false;
}

LAP_AbstractModule::LAP_AbstractModule(QWidget * parent, bool ro)
    : QWidget(parent)
{
    m_readonly = ro;
}

void LAP_AbstractModule::timer_refresh()
{

}

void LAP_AbstractModule::closeEvent(QCloseEvent * evt)
{
    emit closed();
    evt->accept();
}

LAP_AbstractModule::~LAP_AbstractModule()
{

}

const QString & LAP_AbstractModule::getModuleName() const
{
    return moduleName;
}

int LAP_AbstractModule::getPrefferedHeight() const
{
    return 200;
}

int LAP_AbstractModule::getPrefferedWidth() const
{
    return 500;
}
