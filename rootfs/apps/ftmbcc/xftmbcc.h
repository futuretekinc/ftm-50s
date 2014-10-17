#ifndef	__XFTMBCC_H__
#define	__XFTMBCC_H__

#define	DEBUG	1
#include "debug.h"

#define	XFTMBCC_SHARED_MEM_ID	0x1025
#define	XFTMBCC_MAX_TASKS		16
#define	XFTMBCC_MAX_NAME_LEN	64
typedef	enum
{
	XFTC_NONE	= 0,
	XFTC_START,
	XFTC_STOP
} XFTMBCC_TASK_SIGNAL;

typedef	enum
{
	XFTS_UNINITIALIZED = 0,
	XFTS_INITIALIZING,
	XFTS_INITIALIZED,
	XFTS_START,
	XFTS_RESTART,
	XFTS_RUNNING,
	XFTS_STOPPING,
	XFTS_STOPPED,
	XFTS_WAIT_FOR_REINIT
}	XFTMBCC_TASK_STAT;

typedef	struct _XFTMBCC_TASK
{
	char				name[XFTMBCC_MAX_NAME_LEN];
	XFTMBCC_TASK_SIGNAL	signal;
	XFTMBCC_TASK_STAT	stat;
	int					pid;
	int					(*fInit)(struct _XFTMBCC_TASK *pTask);
	int					(*fIsInitDone)(struct _XFTMBCC_TASK *pTask);
	int					(*fIsAlive)(struct _XFTMBCC_TASK *pTask);
	int					(*fReset)(struct _XFTMBCC_TASK *pTask);
	char				szPrivateData[256];
}	XFTMBCC_TASK;

typedef	struct
{
	unsigned long	ulTags;
	int				nDaemonID;
	int				nTasks;
	XFTMBCC_TASK	lpTasks[XFTMBCC_MAX_TASKS];	
	char			lpszBuf[1024];
}	XFTMBCC_SHARED_MEM;
	
int	XFTMBCC_StartDaemon(void);

#endif

