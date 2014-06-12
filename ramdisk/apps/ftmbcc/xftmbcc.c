#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "xftmbcc.h"
#include "xshared.h"
#include "xutil.h"
#include "tx200k.h"

static void XFTMBCC_Main(void);
static int XFTMBCC_Init(XFTMBCC_SHARED_MEM *pSharedMem);

int	XFTMBCC_StartDaemon(void)
{
	int	pid;
	int	lpPIDs[8];

	XFTMBCC_SHARED_MEM *pSharedMem = XSD_GetMemAddr(0);
	if (pSharedMem == NULL)
	{
		xError("The shared memory not found!\n");
		return	-1;	
	}

	if (pSharedMem->nDaemonID != 0)
	{
		int nPIDs = getProcessIDList("ftmbcc", lpPIDs, 8) ;
		if (nPIDs > 0)
		{
			int	i;

			for(i = 0 ; i < nPIDs ; i++)
			{
				if (lpPIDs[i] != getpid())	
				{
					return	0;	
				}
			}
		}
	}

	pid = fork();
	if (pid == -1)
	{
		return	-1;			
	}
	else if (pid == 0)
	{	
		setsid();
		XSD_Init();
		XFTMBCC_Main();	
	}
	else
	{
		pSharedMem->nDaemonID = pid;	
	}
	return	0;
}

void XFTMBCC_Main(void)
{
	XFTMBCC_SHARED_MEM *pSharedMem = XSD_GetMemAddr(0);
	if (pSharedMem == NULL)
	{
		printf("Shared memory creation failed!\n");
		exit(1);
	}

	XFTMBCC_Init(pSharedMem);

	while(1)
	{
		int	i;

		for(i = 0 ; i < pSharedMem->nTasks ; i++)
		{
			XFTMBCC_TASK *pTask = &pSharedMem->lpTasks[i];
			switch(pTask->stat)
			{
				case	XFTS_UNINITIALIZED:
					{
						pTask->fInit(pTask);
					}
					break;

				case	XFTS_INITIALIZING:
					{
					}
					break;

				case	XFTS_INITIALIZED:
					{
					}
					break;

				case	XFTS_RUNNING:
					{	
					}
					break;

				case	XFTS_STOPPED:
					break;
			}
		}
		sleep(5);
	}
}

int XFTMBCC_Init(XFTMBCC_SHARED_MEM *pSharedMem)
{
	if (pSharedMem == NULL)
	{
		xError("shared memory is NULL\n");	

		return	-1;
	}

	//if (pSharedMem->nTasks == 0)
	{
		pSharedMem->nTasks = 0;
		strncpy(pSharedMem->lpTasks[pSharedMem->nTasks].name, "TX200K", XFTMBCC_MAX_NAME_LEN);
		pSharedMem->lpTasks[pSharedMem->nTasks].stat		= XFTS_UNINITIALIZED;
		pSharedMem->lpTasks[pSharedMem->nTasks].pid			= 0;
		pSharedMem->lpTasks[pSharedMem->nTasks].fInit		= TX200K_TaskInit;
		pSharedMem->lpTasks[pSharedMem->nTasks].fIsAlive	= TX200K_IsAlive;
		pSharedMem->nTasks++;
	}

	return	0;
}

int	XFTMBCC_Final(void)
{
	return	0;
}

