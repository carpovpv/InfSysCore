#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QString>
#include <QDir>

const QString HomePath = QDir::homePath() + "/Base";

const QString AppFirm = "Laboratory Of Applied Programming";
const QString AppName = "LAP-BASE";

const QString DRIVER =   "QIBASE";     //InterBase driver.
const QString SERVER =   "localhost";
const QString DATABASE = "base";
const QString LOGIN =    "sysdba";
const QString PASSWORD = "cdrecord";

#endif // __GLOBALS_H__
