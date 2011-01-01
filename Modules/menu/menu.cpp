#include "menu.h"
#include "ui_menu.h"

#include "addcategory.h"
#include "addmenu.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QFileDialog>
#include <QBuffer>

extern "C" Q_DECL_EXPORT LAP_AbstractModule *  createModule(QWidget *parent, bool ro)
{
    return new Menu(parent,ro);
}

void Menu::install()
{

}

void Menu::remove()
{

}

void Menu::upgrade()
{

}

Menu::Menu(QWidget *parent, bool ro) :
    LAP_AbstractModule(parent, ro),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    moduleName = "LAP_Menu";

    LoadCategory();

    connect(ui->btnAddCategory, SIGNAL(clicked()), this, SLOT(add_category()));
    connect(ui->btnDeleteCategory, SIGNAL(clicked()), this, SLOT(del_category()));
    connect(ui->btnIcon, SIGNAL(clicked()), this, SLOT(change_icon()));
    connect(ui->tblMenuCategory, SIGNAL(rowChange(int)), this, SLOT(LoadModules(int)));
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(add_module()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(del_module()));
    connect(ui->btnIconModule, SIGNAL(clicked()), this, SLOT(change_icon_module()));

}

void Menu::add_module()
{
    if(ui->tblMenuCategory->model() && ui->tblMenuCategory->model()->rowCount())
    {
        QString id = ui->tblMenuCategory->getCurrentData("id").toString();
        int row = ui->tblMenuCategory->getCurrentRow();
        AddMenu * add = new AddMenu(this, id);
        if(add->exec())
        {
            QString nid = add->getId();
            LoadModules(row);
            int r = ui->tblMenu->getRowByField("id","nid");
            ui->tblMenu->selectRow(r);
        }
        delete add;
    }
    else
        QMessageBox::warning(this, tr("Warning"), tr("Please, select menu's category."));
}

void Menu::del_module()
{
    if(ui->tblMenu->model() &&
        ui->tblMenu->model()->rowCount() > 0)
    {
        int row = ui->tblMenuCategory->getCurrentRow();
        QString id = ui->tblMenu->getCurrentData("id").toString();
        QMessageBox::StandardButton ret = QMessageBox::question(this, \
                           tr("Question"), tr("Are you sure you'd like to delete the menu selected?"),\
                           QMessageBox::Yes | QMessageBox::No);
           if (ret == QMessageBox::Yes)
           {
               QSqlQuery query;
               if(!query.exec("delete from modules where id = " + id))
               {
                   QMessageBox::critical(this, tr("Error"), tr("Error deleting the menu.\n%1").arg(query.lastError().databaseText()));
                   return;
               }
               LoadModules(row);
           }
    }
}

void Menu::change_icon_module()
{
    if(ui->tblMenu->model() &&
              ui->tblMenu->model()->rowCount())
    {
        QString id = ui->tblMenu->getCurrentData("id").toString();
        int row = ui->tblMenu->getCurrentRow();

        QString fileName = QFileDialog::getOpenFileName(this, tr("Select an icon"),
                                                        "",
                                                        tr("Images (*.png *.xpm *.jpg)"));
        if(fileName.isEmpty())
            return;

        QPixmap pixmap(fileName);

        QSqlQuery query;
        query.prepare("update modules set picture = ? where id = ?");

        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");

        query.addBindValue(bytes);
        query.addBindValue(id);

        if(!query.exec())
        {
                QMessageBox::critical(this, tr("Error"), query.lastError().text());
        }
        else
        {
            ui->tblMenu->refreshPk(id, row);
        }

    }
}

void Menu::change_icon()
{
    if(ui->tblMenuCategory->model() &&
              ui->tblMenuCategory->model()->rowCount())
    {
        QString id = ui->tblMenuCategory->getCurrentData("id").toString();
        int row = ui->tblMenuCategory->getCurrentRow();

        QString fileName = QFileDialog::getOpenFileName(this, tr("Select an icon"),
                                                        "",
                                                        tr("Images (*.png *.xpm *.jpg)"));
        if(fileName.isEmpty())
            return;

        QPixmap pixmap(fileName);

        QSqlQuery query;
        query.prepare("update menucategory set picture = ? where id = ?");

        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");

        query.addBindValue(bytes);
        query.addBindValue(id);

        if(!query.exec())
        {
                QMessageBox::critical(this, tr("Error"), query.lastError().text());
        }
        else
        {
            ui->tblMenuCategory->refreshPk(id, row);
        }

    }
}

void Menu::del_category()
{
    if(ui->tblMenuCategory->model() &&
        ui->tblMenuCategory->model()->rowCount() > 0)
    {
        QString id = ui->tblMenuCategory->getCurrentData("id").toString();
        QMessageBox::StandardButton ret = QMessageBox::question(this, \
                           tr("Question"), tr("Are you sure you'd like to delete the category selected?"),\
                           QMessageBox::Yes | QMessageBox::No);
           if (ret == QMessageBox::Yes)
           {
               QSqlQuery query;
               if(!query.exec("delete from menucategory where id = " + id))
               {
                   QMessageBox::critical(this, tr("Eror"), tr("Error deleteing the category.\n%1").arg(query.lastError().databaseText()));
                   return;
               }
               LoadCategory();
           }


    }
}

void Menu::add_category()
{
    AddCategory * add = new AddCategory(this);
    if(add->exec())
    {
        QString id = add->getId();
        if(id != "-1")
        {
            LoadCategory();
            int row = ui->tblMenuCategory->getRowByField("id", id);
            ui->tblMenuCategory->selectRow(row);

        }
    }
    delete add;
}

void Menu::LoadCategory()
{
    QString sql = "select id, picture, sorter, menu from menucategory ";
    ui->tblMenuCategory->clearColumns();
    ui->tblMenuCategory->addColumn("id", "id", tr("ID"), true, false);
    ui->tblMenuCategory->addColumn("picture", "picture", tr("..."), true, false,"IMAGE");
    ui->tblMenuCategory->addColumn("sorter", "sorter", tr("Sorter"), false, false,"TEXTEDIT","\\d{1,5}");
    ui->tblMenuCategory->addColumn("menu", "menu", tr("Menu"), false, false, "TEXTEDIT","");
    ui->tblMenuCategory->setRefreshQuery(sql);
    ui->tblMenuCategory->setQuery(sql + " order by sorter", "menucategory", "id", "id", false);

    ui->tblMenuCategory->horizontalHeader()->setStretchLastSection(true);
    ui->tblMenuCategory->setColumnWidth(0, 26);
    ui->tblMenuCategory->setColumnWidth(1, 26);
    ui->tblMenuCategory->setColumnWidth(2, 55);

    ui->tblMenuCategory->resizeRowsToContents();

    ui->tblMenuCategory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblMenuCategory->setSelectionBehavior(QAbstractItemView::SelectRows);

    if(ui->tblMenuCategory->model() && ui->tblMenuCategory->model()->rowCount())
        LoadModules(0);
}

void Menu::LoadModules(int r)
{
    QString id = ui->tblMenuCategory->getData(r,"id").toString();
    QString sql = "select id, picture, sorter, caption, filepath from modules ";
    ui->tblMenu->clearColumns();
    ui->tblMenu->addColumn("id","id",tr("ID"), true, false);
    ui->tblMenu->addColumn("picture","picture",tr("..."), true, false,"IMAGE");
    ui->tblMenu->addColumn("sorter","sorter",tr("Сортер"), false, false, "TEXTEDIT", "\\d{1,5}");
    ui->tblMenu->addColumn("caption","caption",tr("Название"), false, false,"TEXTEDIT");
    ui->tblMenu->addColumn("filepath","filepath",tr("Путь"), false, false,"TEXTEDIT");

    ui->tblMenu->setRefreshQuery(sql);
    ui->tblMenu->setQuery(sql + " where menuid = " + id + " order by sorter",
                          "modules",
                          "id",
                          "id",
                          false);
    ui->tblMenu->setColumnWidth(0, 26);
    ui->tblMenu->setColumnWidth(1, 26);
    ui->tblMenu->setColumnWidth(2, 55);

    ui->tblMenu->resizeRowsToContents();
    ui->tblMenu->horizontalHeader()->setStretchLastSection(true);

    ui->tblMenu->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblMenu->setSelectionBehavior(QAbstractItemView::SelectRows);

}

Menu::~Menu()
{
    delete ui;
}

void Menu::changeEvent(QEvent *e)
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
