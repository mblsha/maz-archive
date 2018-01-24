#include "debug.h"
#include <stdarg.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include <conio.h>

void debug(const char *format, ...)
{
	va_list argptr;
	static char data[1024];

	va_start(argptr, format);
	vsprintf(data, format, argptr);
	va_end(argptr);

	int fd = open("ntfsview.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	close(fd);

	//printf(data);
	//getch();
}

