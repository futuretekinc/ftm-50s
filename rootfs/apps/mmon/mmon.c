#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "debug.h"
#include "xcom.h"
#include "modem.h"
#include "mmon.h"
#include "xppp.h"
#include "xshared.h"

MM_INFO		*pInfo = 0;

char *szStateName[] =
{
	"MM_STATE_INIT",
	"MM_STATE_RUNNING",
	"MM_STATE_STOPPED",
	"MM_STATE_RESET",
	"MM_STATE_ABNORMAL",
	"MM_STATE_FAILED",
	"MM_STATE_UNKNOWN"
};

int	MM_Init(void);
void MM_Event(int nSig);

void	MMON_Main(XCFG *pxCfg)
{
	signal(SIGUSR1, MM_Event);
	XTRACE("Start MMON_Main[PID : %d]\n", getpid());
	MM_Init();


	pInfo->xState = MM_STATE_INIT;

	while(1)
	{
		XTRACE("STATE : %s\n", szStateName[pInfo->xState]);
		switch(pInfo->xState)
		{
		case	MM_STATE_INIT:
			{
				MDM_Start();
						
				int	i;
				for(i = pxCfg->nBootWaitTime ; i > 0 ; i--)
				{
					if (MDM_IsLive())
					{
						XTRACE("INFO : MDM is live\n");
						break;
					}
					sleep(1);	
				}

				if ((i != 0) && (XP3_Start() == XP3_STATE_RUNNING))
				{
					pInfo->xState = MM_STATE_RUNNING;	
				}
				else
				{
					pInfo->xState = MM_STATE_ABNORMAL;
				}
			}
			break;

		case	MM_STATE_ABNORMAL:
			{
				MDM_Restart();
				int	i;
				for(i = pxCfg->nBootWaitTime ; i > 0 ; i--)
				{
					if (MDM_IsLive())
					{
						break;
					}
					sleep(1);	
				}

				if ((i != 0) && (XP3_Start() == XP3_STATE_RUNNING))
				{
					pInfo->xState = MM_STATE_RUNNING;	
				}
				else
				{
					pInfo->xState = MM_STATE_FAILED;	
				}

			}
			break;

		case	MM_STATE_RUNNING:
			{
				sleep(pxCfg->nLiveCheckInterval);	

				if (XP3_Status() != XP3_STATE_RUNNING)
				{
					pInfo->xState = MM_STATE_RESET;
				}
			}
			break;

		case	MM_STATE_STOPPED:
			{
				sleep(pxCfg->nLiveCheckInterval);

				if (XP3_Status() == XP3_STATE_RUNNING)
				{
					pInfo->xState = MM_STATE_RUNNING;	
				}
			}
			break;

		case	MM_STATE_RESET:
			{	
				MDM_Restart();
				int	i;
				for(i = pxCfg->nBootWaitTime ; i > 0 ; i--)
				{
					if (MDM_IsLive())
					{
						break;
					}
					sleep(1);	
				}

				if ((i != 0) && (XP3_Start() == XP3_STATE_RUNNING))
				{
					pInfo->xState = MM_STATE_RUNNING;
				}
				else
				{
					pInfo->xState = MM_STATE_ABNORMAL;
				}
			}
			break;
		}

		sleep(5);	
	}
}


int	MM_Init(void)
{
	pInfo = XSD_Open(MM_SHARED_KEY, sizeof(MM_INFO));
	if (pInfo == NULL)
	{
		pInfo = XSD_Create(MM_SHARED_KEY, sizeof(MM_INFO));
		if (pInfo == NULL)
		{
			exit(0);	
		}
	}
	pInfo->xPID = getpid();


	return	0;
}

void MM_Event(int nSig)
{
	switch(pInfo->xCmd)
	{
	case	MM_CMD_RUN:
		break;

	case	MM_CMD_STOP:
		break;

	case	MM_CMD_VERBOSE:
		XTRACE_ON();
		break;
	}
}
