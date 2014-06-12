#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h> 
#include <assert.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/signal.h> 
#include <sys/ioctl.h> 
#include <sys/poll.h> 
#include <termios.h>
#include "xserial.h"
#include "xftmbcc.h"
#include "xutil.h"

#define	TX200K_STAT_UNINITIALIZED	0
#define	TX200K_STAT_MODEM_CHECKED	1
#define	TX200K_STAT_INITIALIZING	2
#define	TX200K_STAT_RUNNING			3
#define	TX200K_STAT_STOPPED			4

typedef	struct
{
	int		stat;
	int		nTimer;
	FILE	*fp;
	int		nPID;
}	TX200K_DATA;

int	TX200K_Main(struct _XFTMBCC_TASK *pTask);
int	TX200K_Init(struct _XFTMBCC_TASK *pTask);
int TX200K_ConnectionCheck(char *lpszDeviceName);

int	TX200K_TaskInit(struct _XFTMBCC_TASK *pTask)
{
	int	pid;

	assert(pTask != NULL);

	pid = fork();
	if (pid == -1)
	{
		pTask->stat = XFTS_STOPPED;
		return	-1;			
	}
	else if (pid == 0)
	{
		pTask->stat = XFTS_INITIALIZING;
		TX200K_Main(pTask);
		exit(1);
	}
	else 
	{
		pTask->pid = pid;
	}

	return	0;
}

int	TX200K_IsTaskAlive(struct _XFTMBCC_TASK *pTask)
{
	TX200K_DATA *pData = (TX200K_DATA *)pTask->szPrivateData;

	switch(pData->stat)
	{
	case	TX200K_STAT_MODEM_CHECKED:
	case	TX200K_STAT_INITIALIZING:
		return	XFTS_INITIALIZING;

	case	TX200K_STAT_RUNNING:
		return	XFTS_RUNNING;

	case	TX200K_STAT_UNINITIALIZED:
	case	TX200K_STAT_STOPPED:
		return	XFTS_STOPPED;
	
	}

	return	XFTS_UNINITIALIZED ;
}

int	TX200K_Main(struct _XFTMBCC_TASK *pTask)
{
	TX200K_DATA *pData = (TX200K_DATA *)pTask->szPrivateData;

	pData->stat		= TX200K_STAT_UNINITIALIZED;
	pData->nTimer	= 5;
	pData->nPID		= 0;

	while(1)
	{
		switch(pTask->signal)
		{
		case	XFTC_START:
			{
				if ((pData->stat == TX200K_STAT_MODEM_CHECKED) || (pData->stat == TX200K_STAT_STOPPED))
				{
					return	TX200K_Init(pTask);	
				}
			}
			break;

		case	XFTC_STOP:
			{
				return	0;
			}
			break;
		default:
			{
			}
			break;
		}

		switch (pData->stat)
		{
		case	TX200K_STAT_UNINITIALIZED:
			{
				if (TX200K_ConnectionCheck("/dev/ttyS1") == 0)
				{
					pData->stat = TX200K_STAT_MODEM_CHECKED;
				}
			}
			break;
		
		case	TX200K_STAT_MODEM_CHECKED:
			{	
			}
			break;

		case	TX200K_STAT_INITIALIZING:
			{
				FILE *fp = fopen("/var/run/ppp0.pid", "r");
				if (fp != 0)
				{
					int	nPID;

					if (fscanf(fp, "%d", &nPID) != 0)
					{
						PRC_STATE nState = getProcessState(nPID);
						if (nState == PRCS_R || nState == PRCS_S)
						{
							pData->nPID = nPID;
							pData->nTimer= 10;
							pData->stat = TX200K_STAT_RUNNING;	
						}
						
					}
					fclose(fp);
				}
			}
			break;

		case	TX200K_STAT_RUNNING:
			{	
				if (--pData->nTimer <= 0)
				{
					PRC_STATE nState = getProcessState(pData->nPID);
					if (nState == PRCS_Z || nState == PRCS_U)
					{
						pData->stat = TX200K_STAT_STOPPED;	
					}
				}
			}
			break;
		}
		sleep(1);
	}

}

int	TX200K_Init(struct _XFTMBCC_TASK *pTask)
{
	TX200K_DATA *pData = (TX200K_DATA *)pTask->szPrivateData;

	FILE *fp = popen("pppd call tx200k", "r");
	pclose(fp);

	pData->stat = TX200K_STAT_INITIALIZING;
	pData->nTimer= 10;

	return	0;
}

int TX200K_Reset(struct _XFTMBCC_TASK *pTask)
{
	if (pTask->pid != 0)
	{
		kill(pTask->pid, SIGTERM);	
	}

	printf("tx200k_reset\n");
	pTask->pid = 0;	

	return	0;
}

//------------------------------------------------------------------------------ 
// 설명 : 해당 시리얼 포트에 TX200K가 연결되어 있는지 확인한다.
// 주의 : 
//------------------------------------------------------------------------------ 
int TX200K_ConnectionCheck(char *lpszDeviceName)
{ 
    int fd; // 시리얼포트 파일핸들
    int ret = -1;
    int loop = 0;
    char buf[128]; // 데이타 버퍼
    int rdcnt; 
    struct termios oldtio;

    fd = open_serial( lpszDeviceName, 115200, &oldtio); 
    if ( fd < 0 ) return -2; 

    for(loop = 0 ; loop < 5 ; loop++)
    {
        write( fd, "AT\n", strlen("AT\n")); 
        while(1)
        {
            rdcnt = read( fd, buf, sizeof(buf) - 1); 
            if ( rdcnt > 0 ) 
            {
                buf[rdcnt] = 0;
                if (strstr(buf, "OK")  != NULL)
                {
                    return  0;
                } 
            }
            else
            {
                break; 
            }
        }
    }

    close_serial( fd ); 
  
    return ret; 
} 

