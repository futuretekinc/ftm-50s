#ifndef	__DEBUG_H__
#define	__DEBUG_H__
#include <errno.h>

#ifdef DEBUG
#define	xTrace(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#define	xError(format, ...) fprintf(stderr, "ERR[ %s : %04d ] - "  format, __func__, __LINE__, ## __VA_ARGS__)
#else
#define	xTrace(format, ...) 
#define	xError(format, ...) 
#endif
#endif

