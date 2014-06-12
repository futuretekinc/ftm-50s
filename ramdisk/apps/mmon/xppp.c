#include <stdio.h>
#include <string.h>
#include "xppp.h"

XP3_STATE	XP3_Restart(void)
{
	FILE *fp = popen("/usr/bin/service pppd restart", "r");
	if (fp != NULL)
	{
		pclose(fp);	
	}

	return	XP3_Status();
}

XP3_STATE	XP3_Start(void)
{
	FILE *fp = popen("/usr/bin/service pppd start", "r");
	if (fp != NULL)
	{
		pclose(fp);	
	}

	return	XP3_Status();
}

XP3_STATE	XP3_Stop(void)
{
	FILE *fp = popen("/usr/bin/service pppd stop", "r");
	if (fp != NULL)
	{
		pclose(fp);	
	}

	return	XP3_Status();
}

XP3_STATE	XP3_Status(void)
{
	char	szStatus[64];
	FILE *fp = popen("/usr/bin/service pppd status", "r");
	if (fp != NULL)
	{
		fscanf(fp, "%s", szStatus);
		pclose(fp);	

		if (strcmp(szStatus, "running") == 0)
		{
			return	XP3_STATE_RUNNING;
		}
	}

	return	XP3_STATE_STOPPED;
}
