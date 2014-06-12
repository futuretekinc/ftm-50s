#ifndef __MMON_H__
#define __MMON_H__

#include "xconfig.h"

#define	MM_SHARED_KEY	0x1123

typedef	enum
{
	MM_STATE_INIT = 0,
	MM_STATE_RUNNING,
	MM_STATE_STOPPED,
	MM_STATE_RESET,
	MM_STATE_ABNORMAL,
	MM_STATE_FAILED,
	MM_STATE_UNKNOWN
}	MM_STATE;

typedef	enum
{
	MM_CMD_STOP = 1,
	MM_CMD_RUN,
	MM_CMD_VERBOSE
}	MM_CMD;
typedef	struct
{
	pid_t		xPID;
	MM_STATE	xState;
	MM_CMD		xCmd;
}	MM_INFO;

void	MMON_Main(XCFG *pxConfig);

#endif

