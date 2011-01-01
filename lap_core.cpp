#include "lap_core.h"
#include "ui_lap_core.h"

#include <QMessageBox>
#include <QLabel>
#include <QBoxLayout>
#include <QLibrary>

#include <QSqlQuery>
#include <QSqlError>

#include "Modules/info/lap_info.h"
#include "login.h"
#include "about.h"

#include "lap_abstractmodule.h"

#include "include/iisTaskPanel"

typedef LAP_AbstractModule *  (*createModule)(QWidget *, bool);

LAP_Core::LAP_Core(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LAP_Core)
{
    ui->setupUi(this);

    info = LAP_Info::getLAP_Info();

    if(!info->isDb())
    {
        QMessageBox::critical(this, tr("Error"),tr("The system cannot connect to the database.\nPlease, contact to the HelpDesk."));
        info->freeLAP_Info();
        exit(0);
    }

    QString shutdown = info->GetProperty("shutdown");
    if(shutdown == "Y")
    {
        QMessageBox::critical(this, tr("Error"),tr("The database is shutdown for technical reasons."));
        info->freeLAP_Info();
        exit(0);
    }

    connect(info, SIGNAL(timer_elapsed()), this, SLOT(timeout()));

    stDb = new QLabel("<b>" + info->getDbInfo() + "</b>", this);
    ui->statusBar->addWidget(stDb);

    setWindowState(windowState() ^ Qt::WindowMaximized);

    ui->tasks->setVisible(false);
    //ui->mdi->setBackground(QBrush(QColor(0xfbf398)));

    show();

    Login login(this, true);
    if(login.exec())
    {
        if(!login.isLogged())
        {
            info->freeLAP_Info();
            exit(0);
        }
    }
    else
    {
        info->freeLAP_Info();
        exit(0);
    }

    //Мы в Лимсе

    stUser = new QLabel(tr("User: <b>%1</b>, Department: %2, Role: %3")\
                          .arg(info->getCurrentUser())\
                          .arg(info->getCurrentDept())\
                          .arg(info->getCurrentRole()) , this);
    ui->statusBar->addWidget(stUser,1);

    stInfo = new QLabel(tr(""), this);
    ui->statusBar->addWidget(stInfo);

    QString org = info->GetProperty("organisation");
    this->setWindowTitle(org);

    ctask = NULL;
    spacer = NULL;

    load_menu();

    this->installEventFilter(this);
    shutcounts = 0;
    lbl = NULL;

}

void LAP_Core::timeout()
{
    QString shutdown = info->GetProperty("shutdown");
    if(shutdown == "Y")
    {
        if(shutcounts > 0)
        {
            exit(0);
        }
        shutcounts++;

        minutes = 59;
        lbl = new QLabel(tr("<font color=red><h1> Через %1 с. The system will have be shutdown,<br> please save your work.</h1></font>").arg(QString::number(minutes)),this);

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timer_seconds()));
        timer->start(1000);

        lbl->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        lbl->move(50,50);

        lbl->show();

    }
}

void LAP_Core::timer_seconds()
{
    if(lbl)
    {
        minutes--;
        lbl->setText(tr("<font color=red><h1> In %1 s. the system wil close itself,<br> please save your work.</h1></font>").arg(QString::number(minutes)));
    }
    if(minutes <= 0)
    {
        delete lbl;
        shutcounts = 0;
        lbl = NULL;
    }
}

bool LAP_Core::eventFilter(QObject * obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
           QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
           if( keyEvent->key()  == Qt::Key_F1)
           {
               About * about = new About(this);
               about->exec();
               delete about;
           }
    }
    return QMainWindow::eventFilter(obj, event);
}

void LAP_Core::change_user()
{
    QString user = info->getCurrentUser();

    Login login(this);
    if(login.exec())
    {
        save_applications(user);
        stUser->setText(tr("User: <b>%1</b>, Department: %2, Role: %3")\
                              .arg(info->getCurrentUser())\
                              .arg(info->getCurrentDept())\
                              .arg(info->getCurrentRole()));



        load_menu();

        QSqlQuery query;
        query.exec("update loguser set date_end = current_timestamp where usernam = '" + user + "'");
    }


}

void LAP_Core::save_applications(const QString & usernam)
{
    QStringList d;

    QList<QMdiSubWindow *> lst = ui->mdi->subWindowList();
    QStringList apps;

      for(int i =0; i< lst.count(); ++i)
      {
          QMdiSubWindow * win = lst[i];
          bool ismax = win->isMaximized();
          bool ismin = win->isMinimized();

          int x = win->x();
          int y = win->y();

          int width = win->width();
          int height = win->height();

          d = QStringList();
          struct lap_info l = wins[win];

          QString name = l.moduleName;
          apps << l.file;

          if(!name.isEmpty())
                d << QString::number(ismax) << QString::number(ismin) << QString::number(x) << QString::number(y) << QString::number(width) << QString::number(height);

          info->SetClientSetting("MDI/wins/" + name+ ":" + usernam, d);

          delete win;

        }

    info->SetClientSetting("MDI/apps/"+usernam, apps);
}

void LAP_Core::load_menu()
{

    QString role = info->getCurrentRole();

    QStringList apps = info->GetClientSetting("MDI/apps/"+info->getCurrentUser(), QStringList()).toStringList();

    if(ctask == NULL)
    {
        ctask = new iisTaskBox(QIcon(":images/images/lock.png").pixmap(1), tr("Session"), true, this);
        iisIconLabel * act_change_user = new iisIconLabel(QIcon(":images/images/close.png"), tr("Change user"), ctask);
        connect(act_change_user, SIGNAL(activated()), this, SLOT(change_user()));
        ctask->addIconLabel(act_change_user);
        ui->tasks->addWidget(ctask);
    }
    if(spacer != NULL)
    {
          delete spacer;
    }

    for(int i=0; i< menuentries.size(); ++i)
        delete menuentries[i];
    menuentries.clear();
    ros.clear();

    QSqlQuery query;
    query.exec("select menu, picture, id from menucategory where id in (select mod.menuid from modules mod inner join modulerole m on m.moduleid = mod.id inner join roles r on r.id = m.roleid where r.rolename ='" +role+ "' and m.available ='Y' ) order by sorter");
    while(query.next())
    {

        QImage img = QImage::fromData(query.value(1).toByteArray());
        iisTaskBox * task = new iisTaskBox(QPixmap::fromImage(img), query.value(0).toString(), true, this);
        menuentries.push_back(task);

        QSqlQuery actions;
        actions.exec("select mod.caption, mod.filepath, mod.picture, m.ro from modules mod inner join modulerole m on m.moduleid = mod.id inner join roles r on r.id = m.roleid where r.rolename ='" + role + "' and m.available ='Y' and mod.menuid = " + query.value(2).toString() + " order by mod.sorter");
        while(actions.next())
        {
            QImage ima = QImage::fromData(actions.value(2).toByteArray());
            iisIconLabel *act = new iisIconLabel(QPixmap::fromImage(ima), actions.value(0).toString(), task);
            task->addIconLabel(act);

            modules[act] = actions.value(1).toString();
            ros[act] = actions.value(3).toString() == "Y" ? true : false;
            connect(act, SIGNAL(activated()), this, SLOT(module_activated()));

            if(apps.indexOf(modules[act]) != -1) {
                LoadModule(modules[act],act);
            }
        }
        ui->tasks->addWidget(task);

    }

    QVBoxLayout * vb = dynamic_cast<QVBoxLayout *>(ui->tasks->layout());
    const int box_width = 200;

    spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vb->addWidget(spacer);

    ui->tasks->setMinimumWidth(box_width);

    ui->tasks->setScheme(iisWinXPTaskPanelScheme::defaultScheme());
    ui->tasks->setVisible(true);

}

void LAP_Core::closeEvent(QCloseEvent *evt)
{
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Would you like to exit the system?"),QMessageBox::Yes | QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        evt->accept();
        return;
    }
    evt->ignore();
}

void LAP_Core::module_activated()
{
    const QObject * obj = sender();
    QMdiSubWindow * win = loaded[obj];
    if(win == NULL)
    {
        QString file = modules[sender()];
        if(!file.isEmpty())
            LoadModule(file, obj);
    }
    else
    {
        win->show();
        win->raise();
        win->activateWindow();
        win->setFocus();
    }
}

QMdiSubWindow * LAP_Core::getMdiWindow(const QString &modulename)
{
    QMdiSubWindow * sub = NULL;

    QMapIterator<QMdiSubWindow *, struct lap_info> i(wins);
     while (i.hasNext()) {
         i.next();
         if(i.value().moduleName == modulename)
         {
             sub = i.key();
             break;
         }
     }

     return sub;
}

void LAP_Core::SendLoadDataWindow(const QString &modulename, const QString &sql)
{
    QMdiSubWindow * sub = getMdiWindow(modulename);
    if(sub != NULL)
    {
        sub->show();
        sub->raise();
        sub->activateWindow();
        sub->setFocus();

        LAP_AbstractModule * mod = dynamic_cast<LAP_AbstractModule *> (sub->widget());
        if(mod)
            mod->LoadDataWindowRequest(sql);
    }
}

void LAP_Core::LoadModule(const QString & file, const QObject *obj)
{
    //грузим модули
    QLibrary * lib = new QLibrary(file);
    createModule create = (createModule) lib->resolve("createModule");

    if (create)
    {

        LAP_AbstractModule * sub = create(this, ros[obj]);

        QString module = sub->getModuleName();

        if(sub->isModal())
        {
            sub->setWindowFlags(Qt::Dialog);
            sub->setWindowModality(Qt::WindowModal);
            sub->setAttribute(Qt::WA_DeleteOnClose);
            sub->setWindowIcon(QIcon(":images/images/download.png"));

            const QRect rect = QApplication::desktop()->screenGeometry();
            int x = (rect.width() - sub->width()) / 2;
            int y = (rect.height() - sub->height()) / 2;

            sub->move(x,y);

            sub->show();
        }
        else
        {
            QMdiSubWindow * win = ui->mdi->addSubWindow(sub);

            sub->install();

            sub->show();
            sub->activateWindow();
            sub->raise();

            int w = sub->getPrefferedWidth();
            int h = sub->getPrefferedHeight();

            loaded[obj] = win;

            struct lap_module mod;
            mod.obj = obj;
            mod.module = sub;

            widgets[ dynamic_cast<QObject*>(sub)] = mod;
            struct lap_info l;
            l.file = file;
            l.moduleName = module;

            wins[win] = l;

            connect(sub, SIGNAL(closed()), this, SLOT(module_destroyed()));
            connect(info, SIGNAL(timer_elapsed()), sub, SLOT(timer_refresh()));
            connect(sub, SIGNAL(LoadDataWindow(QString,QString)), this, SLOT(SendLoadDataWindow(QString,QString)));

            //находим текущую геометрию.

            int x = 0;
            int y =0;

            QStringList lst = info->GetClientSetting("MDI/wins/" + module + ":" + info->getCurrentUser(), QStringList()).toStringList();
            if(lst.size() >= 6)
            {
               bool ismax = lst[0].toInt();
               bool ismin = lst[1].toInt();

               x = lst[2].toInt();
               y = lst[3].toInt();
               w = lst[4].toInt();
               h = lst[5].toInt();

             }

             win->move(x,y);
             win->resize(w,h);
         }

        delete lib;
    }
}

void LAP_Core::module_destroyed()
{
    QObject * obj = sender();
    if(widgets.contains(obj))
    {
       lap_module mod= widgets[obj];
       delete mod.module;
       QMdiSubWindow * win = loaded[mod.obj];
       //сохранеям позицию окна

       bool ismax = win->isMaximized();
       bool ismin = win->isMinimized();

       int x = win->x();
       int y = win->y();

       int width = win->width();
       int height = win->height();

       struct lap_info l = wins[win];
       QString name = l.moduleName;

       if(!name.isEmpty())
       {
             QStringList d;
             d << QString::number(ismax) << QString::number(ismin) << QString::number(x) << QString::number(y) << QString::number(width) << QString::number(height);
             info->SetClientSetting("MDI/wins/" + name + ":" + info->getCurrentUser(), d);
       }
       wins.remove(win);
       loaded.remove(mod.obj);

    }
}

LAP_Core::~LAP_Core()
{

    info->SetClientSetting("MainWindow/state", saveState());
    info->SetClientSetting("MainWindow/geometry", saveGeometry());

    save_applications(info->getCurrentUser());

    delete ui;
    info->freeLAP_Info();

}

void LAP_Core::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
