#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "mmon.h"
#include "xcom.h"
#include "modem.h"
#include "debug.h"
#include "xshared.h"
#include "xconfig.h"
#include "xutil.h"

#define PORT		"/dev/ttyS1"
#define	BAUDRATE	115200
#define	TIMEOUT		(1000 * 1000)

MDM_MODEL	xModel = MDM_MODEL_UNKNOWN;

int MM_IsRunning(void);
int MM_ShowModel(void);
int MM_GetPPPStatus(void);
int	MM_Reset(void);
int	MM_CheckAndStart(void);

static int	bDaemonFlag = 0;
static char *lpszConfig = "/etc/mmon.conf";
static XCFG	xCfg;

static struct option long_options[] =
{
	{"daemon",	0,	&bDaemonFlag,	1}
};

int main(int argc, char *argv[])
{

	int	opt, opt_idx;

	while((opt = getopt_long(argc, argv, "c: dhrsv", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	0:
			if (long_options[opt_idx].flag != 0) 
			{
				break;
			}
			break;
		
		case	'c':
			lpszConfig = optarg;
			break;

		case	'd':
			bDaemonFlag = 1;
			break;

		case	'h':
			{

			}
			break;

		
		case	'r':
			{
			}
			break;

		case	's':
			{
			}
			break;

		case	'v':
			{	
				XTRACE_ON();
			}
			break;

		default:
			{
				fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], opt);
				return	0;
			}
		}
	}

	if (XCFG_Load(&xCfg, lpszConfig) < 0)
	{
		fprintf(stderr, "The configuration file does not exist!\nI will set to default configuration.\n");	

		strcpy(xCfg.szDevice, PORT);
		xCfg.bRun				= 1;
		xCfg.nBaudrate			= BAUDRATE;
		xCfg.nTimeout			= TIMEOUT;
		xCfg.nLiveCheckInterval = 10;
		xCfg.nBootWaitTime		= 10;
	}
		
	if (bDaemonFlag)
	{
		int	nPID;

		nPID = fork();
		if (nPID == 0)
		{
			setsid();
			MMON_Main(&xCfg);
		}
		else if (nPID < 0)
		{
			fprintf(stderr, "Can't fork child process!\n");
		}
	}

	return	0;
}

pid_t MM_GetDaemonID(void)
{
	MM_INFO *pInfo = XSD_Open(MM_SHARED_KEY, sizeof(MM_INFO));
	if (pInfo == NULL)
	{
		XTRACE("shared memory not found!\n");	
		return 0;
	}

	return	pInfo->xPID;
}

int	MM_SendCmd(MM_CMD xCmd)
{
	MM_INFO *pInfo = XSD_Open(MM_SHARED_KEY, sizeof(MM_INFO));
	if (pInfo == NULL)
	{
		XTRACE("shared memory not found!\n");	
		return 0;
	}
	
	if (pInfo->xPID != 0)
	{
		pInfo->xCmd = xCmd;
		kill(pInfo->xPID, SIGUSR1);	
	}

	return	0;
}

