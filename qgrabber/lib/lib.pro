TEMPLATE	= lib
CONFIG		+= qt warn_off release dll

OBJECTS_DIR	= obj
MOC_DIR		= moc

TARGET	= qgrabber
DESTDIR	= ../

INCLUDEPATH	+= ../src/

#
# Shared classes, widgets, dialogs and functions
#

FORMS	+=	imageplotbase.ui
SOURCES	+=	bitmapview.cpp		\
		imageplot.cpp
HEADERS	+=	bitmapview.h		\
		imageplot.h

# Source
SOURCES	+=	datafileitem.cpp	\
		factory.cpp		\
		plugins.cpp		\
		plugins_p.cpp		\
		common.cpp		\
		prefs.cpp
HEADERS	+=	datafileitem.h		\
		factory.h		\
		plugins.h		\
		plugins_p.h		\
		common.h		\
		prefs.h			\
		export.h

# Built-in plugins
SOURCES	+=	dat_data.cpp		\
		dat_file.cpp
HEADERS	+=	dat_data.h		\
		dat_file.h

dll: DEFINES	+= BUILD_DLL

include (../src/alleg.pro)
