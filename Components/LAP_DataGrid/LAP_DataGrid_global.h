#ifndef LAP_DATAGRID_GLOBAL_H
#define LAP_DATAGRID_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LAP_DATAGRID_LIBRARY)
#  define LAP_DATAGRIDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LAP_DATAGRIDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LAP_DATAGRID_GLOBAL_H
