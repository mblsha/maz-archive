#ifndef EXPORT_H
#define EXPORT_H

#include <qglobal.h>

#ifdef Q_WS_WIN
#	ifdef BUILD_DLL
#		define EXPORT  __declspec(dllexport)
#	elif  USE_DLL
#		define EXPORT  __declspec(dllimport)
#	endif
#endif

#ifndef EXPORT
#  define EXPORT
#endif

#endif
