#
# Allegro libraries
#
unix {
	# Output of `allegro-config --libs`. 
	#
	# I only modified it to use -lalleg-4 instead of -lalleg-4.x.x, so
	# you'll need to create symlink, or modify this file in order to compile.
	# Example: `ln -s liballeg-4.0.2.so liballeg-4.so`
	
	LIBS	+= -L/usr/local/lib -L/usr/X11R6/lib -Wl,-export-dynamic -lalleg-4 -laldat 
	!plugin: LIBS += -lalleg_unsharable
}
win32 {
	LIBS	+= alleg.lib aldat.lib
}
