#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "xconfig.h"
#include "xassert.h"

#define	BOOT_WAIT_TIME		10
#define	MIN_BOOT_WAIT_TIME	1
#define	MAX_BOOT_WAIT_TIME	60

#define	LIVE_CHECK_INTERVAL	10

int	XCFG_SetDefault(XCFG *pxConfig)
{
	XASSERT(pxConfig != NULL);

	memset(pxConfig, 0, sizeof(XCFG));

	pxConfig->nBootWaitTime = BOOT_WAIT_TIME;
	pxConfig->nLiveCheckInterval = LIVE_CHECK_INTERVAL;
	return	0;
}

int	XCFG_Load(XCFG *pxConfig, char *lpszName)
{
	char	szBuf[1024];

	XASSERT((pxConfig != NULL) && (lpszName != NULL));

	FILE *fp = fopen(lpszName, "r");
	if (fp == NULL)
	{
		return	-1;	
	}

	XCFG_SetDefault(pxConfig);

	while(fgets(szBuf, sizeof(szBuf), fp))
	{
		char	szField[64];
		char	szContent[1024];

		if ((sscanf(szBuf, "%s %s", szField, szContent) != 2) ||
			(szField[0] == '#'))
		{
			continue;	
		}

		if (strcmp(szField, "device:") == 0)
		{
			strncpy(pxConfig->szDevice, szContent, sizeof(pxConfig->szDevice) - 1);						
		}
		else if (strcmp(szField, "run:") == 0)
		{
			if (strcmp(szContent, "yes") == 0)
			{
				pxConfig->bRun= 1;	
			}
			else
			{
				pxConfig->bRun= 0;	
			}
		}
		else if (strcmp(szField, "boot_wait:") == 0)
		{
			int nTime = atoi(szContent);

			if ( MIN_BOOT_WAIT_TIME <= nTime && nTime <= MAX_BOOT_WAIT_TIME )
			{
				pxConfig->nBootWaitTime = nTime;
			}
		}
		else if (strcmp(szField, "live_check:") == 0)
		{
			int nTime = atoi(szContent);

			if ( 0 < nTime )
			{
				pxConfig->nLiveCheckInterval = nTime;
			}
		}
	}

	fclose(fp);

	return	0;
}
