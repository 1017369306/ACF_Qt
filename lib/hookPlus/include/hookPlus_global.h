#ifndef HOOKPLUS_GLOBAL_H
#define HOOKPLUS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HOOKPLUS_LIBRARY)
#  define HOOKPLUS_EXPORT Q_DECL_EXPORT
#else
#  define HOOKPLUS_EXPORT Q_DECL_IMPORT
#endif

#endif // HOOKPLUS_GLOBAL_H
