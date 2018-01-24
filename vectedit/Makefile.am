####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = vectedit

## INCLUDES were found outside kdevelop specific part

vectedit_SOURCES = colorbutton.cpp editor.cpp vecteditview.cpp vectedit.cpp main.cpp 
SOURCES = colorbutton.cpp editor.cpp vecteditview.cpp vectedit.cpp main.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
vectedit_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = docs 

EXTRA_DIST = main.cpp vectedit.cpp vectedit.h vecteditview.cpp vecteditview.h lo32-app-vectedit.png lo16-app-vectedit.png filenew.xpm filesave.xpm fileopen.xpm editor.h editor.cpp colorbutton.cpp colorbutton.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

vectedit_METASOURCES = AUTO

# the library search path. 
vectedit_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(vectedit_METASOURCES)

