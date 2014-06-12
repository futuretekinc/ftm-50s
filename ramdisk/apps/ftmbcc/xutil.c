#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "xutil.h"

unsigned int getProcessID(char *p_processname) 
{
	DIR				*dir_p;
	struct dirent	*dir_entry_p;
	char			dir_name[40];	// ??? buffer overrun potential
	char			target_name[252];  // ??? buffer overrun potential
	int				target_result;
	char			exe_link[252];
	int				result;

	result=0;
	dir_p = opendir("/proc/"); // Open /proc/ directory
	 // Reading /proc/ entries
	while(NULL != (dir_entry_p = readdir(dir_p))) 
	{
		// Checking for numbered directories 
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) 
		{
			strcpy(dir_name, "/proc/");
			strcat(dir_name, dir_entry_p->d_name);
			// Obtaining the full-path eg: /proc/24657/ 
			strcat(dir_name, "/");
			exe_link[0] = 0;
			strcat(exe_link, dir_name);
			// Getting the full-path of that exe link
			strcat(exe_link, "exe"); 
			// Getting the target of the exe ie to which binary it points to
			target_result = readlink(exe_link, target_name, sizeof(target_name)-1); 
			if (target_result > 0) 
			{
				target_name[target_result] = 0;	
				// Searching for process name in the target name -- ??? could be a better search !!!
				if (strstr(target_name, p_processname) != NULL) 
				{
					result = atoi(dir_entry_p->d_name);
					closedir(dir_p);
					return result;
				}
			}
		}
	}
	closedir(dir_p);

	return result;
}

unsigned int getProcessIDList(char *p_processname, int *lpPIDs, int nMaxCount) 
{
	DIR				*dir_p;
	struct dirent	*dir_entry_p;
	char			dir_name[40];	// ??? buffer overrun potential
	char			target_name[252];  // ??? buffer overrun potential
	int				target_result;
	char			exe_link[252];
	int				result;

	result=0;
	dir_p = opendir("/proc/"); // Open /proc/ directory
	 // Reading /proc/ entries
	while(NULL != (dir_entry_p = readdir(dir_p))) 
	{
		// Checking for numbered directories 
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) 
		{
			strcpy(dir_name, "/proc/");
			strcat(dir_name, dir_entry_p->d_name);
			// Obtaining the full-path eg: /proc/24657/ 
			strcat(dir_name, "/");
			exe_link[0] = 0;
			strcat(exe_link, dir_name);
			// Getting the full-path of that exe link
			strcat(exe_link, "exe"); 
			// Getting the target of the exe ie to which binary it points to
			target_result = readlink(exe_link, target_name, sizeof(target_name)-1); 
			if (target_result > 0) 
			{
				target_name[target_result] = 0;	
				// Searching for process name in the target name -- ??? could be a better search !!!
				if (strstr(target_name, p_processname) != NULL) 
				{
					if (result < nMaxCount)
					{
						lpPIDs[result++] = atoi(dir_entry_p->d_name);
					}
				}
			}
		}
	}
	closedir(dir_p);

	return result;
}

PRC_STATE	getProcessState(int nPID)
{
	char	szCmd[128];
	char	szBuf[512];

	sprintf(szCmd, "/proc/%d/status", nPID);
	FILE	*fp = fopen(szCmd, "r");
	if(fp == NULL)
	{
		return	PRCS_U;
	}

	while(fgets(szBuf, sizeof(szBuf), fp) != NULL)
	{
		char	szTag[32];
		char	szValue[128];

		if (sscanf(szBuf, "%s %s", szTag, szValue) == 2)
		{
			if (strcmp(szTag, "State:") == 0)
			{
				fclose(fp);
				return	(PRC_STATE)szValue[0];
			}
		}
	}

	fclose(fp);

	return	PRCS_U;
}
