#ifndef ROLES_GLOBAL_H
#define ROLES_GLOBAL_H

#include <QtCore/qglobal.h>
#include "../../lap_abstractmodule.h"

#if defined(ROLES_LIBRARY)
#  define ROLESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ROLESSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ROLES_GLOBAL_H
