TEMPLATE	= app
CONFIG		+= qt warn_off release
TARGET		= qgrabber
DESTDIR		= ../

#SOURCES += lang.cpp varlist.cpp
#HEADERS += lang.h   varlist.h

FORMS	+=	edititem.ui		\
		prefsdialog.ui		\
		saveprogress.ui		\
		passworddialog.ui	\
		aboutdialog.ui		\
		fileviewer.ui

# Built-in widgets
SOURCES	+=	qgrabber.cpp		\
		datafileview.cpp	\
		propview.cpp		\
		drawfield.cpp

HEADERS	+=	qgrabber.h		\
		datafileview.h		\
		propview.h		\
		drawfield.h

# Common stuff
SOURCES	+=	main.cpp

HEADERS	+=	version.h		\
		datedit.h		\
		alleg.h

TRANSLATIONS +=	qgrabber_ru.ts


debug: {
	DEFINES	+= DEBUG
}

OBJECTS_DIR	= obj
MOC_DIR		= moc

INCLUDEPATH	+= ../lib/
DEFINES		+= USE_DLL

#
# Libs
#
include (alleg.pro)
unix:	LIBS += -L../ -lqgrabber
win32:	LIBS += ../qgrabber.lib
