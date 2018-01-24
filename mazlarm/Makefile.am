####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = mazlarm

## INCLUDES were found outside kdevelop specific part

mazlarm_SOURCES = preferences.ui alarm.ui preferencesimpl.cpp mazlarm.cpp main.cpp 
mazlarm_LDADD   = /usr/local/lib/libfmod-3.5.so $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = main.cpp mazlarm.cpp mazlarm.h mazlarm.desktop lo32-app-mazlarm.png big_clock.xpm disabled_clock.xpm lo16-app-mazlarm.png normal_clock.xpm preferencesimpl.cpp preferencesimpl.h alarm.ui preferences.ui 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/mazlarm.desktop $(kde_appsdir)/Applications/mazlarm.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-mazlarm.png $(kde_icondir)/locolor/32x32/apps/mazlarm.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/mazlarm.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/mazlarm.png

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
mazlarm_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE
#rcdir = $(kde_datadir)/mazlarm
#rc_DATA = mazlarmui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/mazlarm.pot; \
	fi

