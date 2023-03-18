#ifndef GLOBALRESOURCE_GLOBAL_H
#define GLOBALRESOURCE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GLOBALRESOURCE_LIBRARY)
#  define GLOBALRESOURCE_EXPORT Q_DECL_EXPORT
#else
#  define GLOBALRESOURCE_EXPORT Q_DECL_IMPORT
#endif

#endif // GLOBALRESOURCE_GLOBAL_H
