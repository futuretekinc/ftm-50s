#ifndef	__XCONFIG_H__
#define	__XCONFIG_H__

typedef	struct
{
	int		bRun;
	char	szDevice[1024];
	int		nBaudrate;
	int		nTimeout;
	int		nLiveCheckInterval;
	int		nBootWaitTime;
}	XCFG;

int	XCFG_Load(XCFG *pxConfig, char *lpszName);

#endif

