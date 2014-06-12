#ifndef __TX200K_H__
#define	__TX200K_H__

int TX200K_ConnectionCheck(char *lpszDeviceName);
int	TX200K_TaskInit(struct _XFTMBCC_TASK *pTask);
int	TX200K_IsTaskAlive(struct _XFTMBCC_TASK *pTask);

#endif

