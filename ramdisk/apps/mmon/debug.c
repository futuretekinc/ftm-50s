#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

#if DEBUG
static int nTraceOn = 0;
static int nErrorOn = 1;

int	 XTRACE_ON(void)
{
	nTraceOn = 1;
}

int	 XTRACE_OFF(void)
{
	nTraceOn = 0;
}

int	 XERROR_ON(void)
{
	nErrorOn = 1;
}

int	 XERROR_OFF(void)
{
	nErrorOn = 0;
}

void XTRACE(const char *format, ...) 
{

	if ( nTraceOn )
	{
		va_list arglist;

		va_start(arglist, format);
		vfprintf(stderr, format, arglist);
		va_end(arglist);
	}
}

void XERROR(const char *format, ...)
{
	if ( nErrorOn )
	{
		va_list arglist;

		va_start(arglist, format);
		vfprintf(stderr, format, arglist);
		va_end(arglist);
	}
}
#endif
