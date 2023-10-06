#ifndef LOGINLIB_GLOBAL_H
#define LOGINLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LOGINLIB_LIBRARY)
#  define LOGINLIB_EXPORT Q_DECL_EXPORT
#else
#  define LOGINLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // LOGINLIB_GLOBAL_H
