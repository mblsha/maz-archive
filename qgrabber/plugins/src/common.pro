TEMPLATE	= lib
CONFIG		+= qt warn_off release plugin
DESTDIR		= ../../

INCLUDEPATH +=	../../../lib/	\
		../../../src/

DEFINES += USE_DLL

OBJECTS_DIR	= obj
MOC_DIR		= moc

isEmpty(plugins.path):plugins.path=../../../plugins
target.path += $$plugins.path/
INSTALLS += target
DESTDIR = ../../

#
# Libs
#
include (../../../src/alleg.pro)
unix:	LIBS	+= -L../../../ -lqgrabber
win32:	LIBS	+= ../../../qgrabber.lib
