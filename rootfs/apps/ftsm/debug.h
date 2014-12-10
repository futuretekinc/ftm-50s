#ifndef	__DEBUG_H__
#define	__DEBUG_H__
#include <errno.h>
#include <syslog.h>

#ifdef DEBUG
	int	 TRACE_ON(void);
	int	 TRACE_OFF(void);
	void TRACE(const char *format, ...) ;
	int	 ERROR_ON(void);
	int	 ERROR_OFF(void);
	void ERROR(const char *format, ...) ;
#else
	#define	TRACE_ON()
	#define	TRACE_OFF()
	#define	TRACE(format, ...) 
	#define	ERROR_ON()
	#define	ERROR_OFF()
	#define	TRACE(format, ...) 
	#define	ERROR(format, ...) 
#endif

#define	SYSLOG(foramt, ...) syslog(LOG_INFO|LOG_LOCAL0, format, ## __VA_ARGS )
#endif
