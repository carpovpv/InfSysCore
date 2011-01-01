#ifndef USERS_GLOBAL_H
#define USERS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(USERS_LIBRARY)
#  define USERSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define USERSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // USERS_GLOBAL_H
