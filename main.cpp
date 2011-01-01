
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTranslator>

#include "lap_core.h"
#include "style03.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    a.setStyle( new Style03() );

    QTranslator maintranslator, reptranslator;

    maintranslator.load("translations\\qt_ru");
    reptranslator.load("translations\\exaro_ru_Ru");

    a.installTranslator(&maintranslator);
    a.installTranslator(&reptranslator);

    LAP_Core w;
    w.show();
    return a.exec();

}
