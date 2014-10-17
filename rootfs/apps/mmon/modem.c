#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "xcom.h"
#include "modem.h"

enum	{
	MDM_CMD_AT		= 0,
	MDM_CMD_AT_GMM,
	MDM_CMD_ATE0,
	MDM_CMD_ATE1

};

const char	*lpszAT[] = 
{
	"AT\r",
	"AT+GMM\r",
	"ATE0\r",
	"ATE1\r"
};

const char	*lpszModel[] =
{
	"TX200K",
	"UC864-E",
	"UC864-K",
	"UNKNOWN",
};

const char	*lpszType[] =
{
	"3G/HSDPA",
	"LTE",
	"UNKNOWN"
};

const char *lpszManufacturer[] =
{
	"Telian",
	"Telit",
	"UNKNOWN"
};

MDM_MODEM_INFO	xModemInfos[] = 
{
	{	MDM_MODEL_TX200K,	MDM_TYPE_3G,		MDM_MANU_TELIAN	},
	{	MDM_MODEL_UC864_E,	MDM_TYPE_3G,		MDM_MANU_TELIT	},
	{	MDM_MODEL_UC864_K,	MDM_TYPE_3G,		MDM_MANU_TELIT	},
	{	MDM_MODEL_UNKNOWN,	MDM_TYPE_UNKNOWN,	MDM_MANU_UNKNOWN}
};

typedef	struct
{
	char	szBuf[4096];
	char	*lpszSplits[16];
	int		nSplit;
	int		bOK;
}	MDM_RESP;

MDM_ERROR	MDM_Send(MDM *pMDM, const char *lpszCmd);
MDM_ERROR	MDM_Recv(MDM *pMDM, MDM_RESP *pResp);
int	MDM_ParseResponse(char *lpszResp, MDM_RESP *pResp);

int	MDM_IsLive(void)
{
	int	nRet = 0;

	FILE *fp = popen("/usr/bin/service mdmctrl is_live", "r");
	if (fp != NULL)
	{
		char	szAnswer[64];
		
		if (fscanf(	fp, "%s", szAnswer) == 1)
		{
			XTRACE("Answer = %s\n", szAnswer);
			if (strcmp(szAnswer, "yes") == 0)
			{
				nRet = 1;
			}
		}
		
		pclose(fp);
	}

	return	nRet;
}

void MDM_Start(void)
{
	FILE *fp = popen("/usr/bin/service mdmctrl start", "r");
	if (fp != NULL)
	{
		pclose(fp);
	}
}

void MDM_Stop(void)
{
	FILE *fp = popen("/usr/bin/service mdmctrl stop", "r");
	if (fp != NULL)
	{
		pclose(fp);
	}
}

void MDM_Restart(void)
{
	FILE *fp = popen("/usr/bin/service mdmctrl restart", "r");
	if (fp != NULL)
	{
		pclose(fp);
	}
}


int	MDM_Init(MDM *pMDM, char *lpszDevice, int nBaudrate, int nTimeout)
{
	int			nRet = -1;
	MDM_MODEL	xModel;

	strcpy(pMDM->szDevice, lpszDevice);
	pMDM->nBaudrate = nBaudrate;
	pMDM->nTimeout = nTimeout;

	if (MDM_Connect(pMDM) == 0)
	{
		if ( MDM_IsRunning(pMDM) ) 
		{
			if (MDM_EchoOn(pMDM) == MDM_ERROR_OK) 
			{
				if (MDM_GetModel(pMDM, &xModel) == MDM_ERROR_OK)
				{
					xModel = pMDM->xModel;

					nRet = 0;
				}
			}
		}
		MDM_Disconnect(pMDM);
	}


	if (nRet == 0)
	{
		if (xModel == MDM_MODEL_TX200K)
		{
			FILE *fp = popen("/bin/ln -sf /etc/init.d/tx200kd /etc/init.d/pppd", "r");
			pclose(fp);
		}
		else if (xModel == MDM_MODEL_UC864_E)
		{
			FILE *fp = popen("/bin/ln -sf /etc/init.d/uc864ed /etc/init.d/pppd", "r");
			pclose(fp);
		}
		else if (xModel == MDM_MODEL_UC864_K)
		{
			FILE *fp = popen("/bin/ln -sf /etc/init.d/uc864ed /etc/init.d/pppd", "r");
			pclose(fp);
		}
	}

	return	nRet;
}

MDM_ERROR	MDM_Connect(MDM *pMDM)
{
	pMDM->bEcho = 1;

	if (XCOM_Open(pMDM->szDevice, pMDM->nBaudrate, 0, &pMDM->hCOM) < 0)
	{
		printf("Can't open device[%s]\n", pMDM->szDevice);
		return MDM_ERROR_CANT_OPEN_DEVICE;	
	}

	return	MDM_ERROR_OK;
}

MDM_ERROR	MDM_Disconnect(MDM *pMDM)
{
	XCOM_Close(&pMDM->hCOM);

	return	MDM_ERROR_OK;
}

int	MDM_IsRunning( MDM *pMDM)
{
	MDM_ERROR	xRet;
	MDM_RESP	xResp;

	MDM_Send(pMDM, lpszAT[MDM_CMD_AT]);

	xRet = MDM_Recv(pMDM, &xResp);
	if (xRet == MDM_ERROR_OK)
	{
		return	xResp.bOK;
	}

	return	0;
}

MDM_ERROR	MDM_Disonnect(MDM *pMDM)
{
	XCOM_Close(&pMDM->hCOM);

	return	MDM_ERROR_OK;
}

MDM_ERROR MDM_GetModel( MDM *pMDM, MDM_MODEL *pModel)
{
	MDM_ERROR	xRet;
	MDM_RESP	xResp;

	MDM_Send(pMDM, lpszAT[MDM_CMD_AT_GMM]);
	xRet = MDM_Recv(pMDM, &xResp);
	if (xRet == MDM_ERROR_TIMEOUT)
	{
		MDM_Send(pMDM, lpszAT[MDM_CMD_AT]);
		xRet = MDM_Recv(pMDM, &xResp);
	}

	if (xRet == MDM_ERROR_OK)
	{
		MDM_MODEM_INFO	*pInfo = xModemInfos;

		while(pInfo->xModel != MDM_MODEL_UNKNOWN)
		{

			if (strcmp(MDM_ModelName(pInfo->xModel), xResp.lpszSplits[MDM_RIDX_MODEL + pMDM->bEcho]) == 0)
			{
				*pModel = pInfo->xModel;

				return	MDM_ERROR_OK;
			}

			pInfo++;
		}	
	}

	return	xRet;			
}

MDM_MODEM_INFO *MDM_GetModemInfo(MDM_MODEL xModel)
{
	MDM_MODEM_INFO	*pInfo = xModemInfos;

	while(pInfo->xModel != MDM_MODEL_UNKNOWN)
	{
		if (pInfo->xModel == xModel)
		{
			break;
		}
	}
	
	return	pInfo;
}

MDM_ERROR MDM_SetEcho( MDM *pMDM, int on)
{
	MDM_RESP	xResp;
	char		szRes[64];

	if (on)
	{
		MDM_Send(pMDM, lpszAT[MDM_CMD_ATE1]);
	}
	else
	{
		MDM_Send(pMDM, lpszAT[MDM_CMD_ATE0]);
	}

	if (XCOM_Read(&pMDM->hCOM, (char *)szRes, sizeof(szRes), pMDM->nTimeout) == 0)
	{
		return	MDM_ERROR_TIMEOUT;
	}

	if (MDM_ParseResponse(szRes, &xResp) == 0)
	{
		if (xResp.bOK)
		{
			pMDM->bEcho = on;
			return	MDM_ERROR_OK;
		}
	}

	return	MDM_ERROR_INVALID;
}

MDM_ERROR MDM_EchoOff( MDM *pMDM )
{
	return	MDM_SetEcho( pMDM, 0) ;
}

MDM_ERROR MDM_EchoOn( MDM *pMDM )
{
	return	MDM_SetEcho( pMDM, 1) ;
}


MDM_ERROR MDM_Send(MDM *pMDM, const char *lpszCmd)
{
	XCOM_Write(&pMDM->hCOM, lpszCmd, strlen(lpszCmd));

	return	MDM_ERROR_OK;
}

MDM_ERROR MDM_Recv(MDM *pMDM, MDM_RESP *pResp)
{
	char	szBuf[1024];

	if (XCOM_Read(&pMDM->hCOM, (char *)szBuf, sizeof(szBuf), pMDM->nTimeout) == 0)
	{
		return	MDM_ERROR_TIMEOUT;
	}

	if (MDM_ParseResponse(szBuf, pResp) == 0)
	{
		return	MDM_ERROR_OK;
	}

	return	MDM_ERROR_INVALID;
}

int	MDM_ParseResponse(char *lpszResp, MDM_RESP *pResp)
{

	pResp->nSplit = 0;
	
	strcpy(pResp->szBuf, lpszResp);

	char *lpszPos = pResp->szBuf;
	char *lpszEnd = pResp->szBuf + strlen(pResp->szBuf);
	while(lpszPos < lpszEnd)
	{
		char *lpszNewLine = strchr(lpszPos, (char)0x0A);
		if (lpszNewLine != NULL)
		{
			*lpszNewLine = 0;	
			if (strlen(lpszPos) > 1)
			{
				if ((lpszNewLine[-1] == 0x0D) || (lpszNewLine[-1] == 0x0A))
				{
					lpszNewLine[-1] = 0;	
				}
				pResp->lpszSplits[pResp->nSplit++] = lpszPos;
			}
			else if (strlen(lpszPos) == 1)
			{
				if ((lpszNewLine[-1] != 0x0D) && (lpszNewLine[-1] != 0x0A))
				{
					pResp->lpszSplits[pResp->nSplit++] = lpszPos;
				}	
			}
			lpszPos = lpszNewLine+1;
		}

	}

	if ((pResp->nSplit != 0) && (strncmp(pResp->lpszSplits[pResp->nSplit - 1], "OK", 2) == 0))
	{
		pResp->bOK = 1;	
	}
	else
	{
		pResp->bOK = 0;	
	}
	return	0;
}

const char*	MDM_ModelName(MDM_MODEL xModel)
{
	if (xModel < MDM_MODEL_UNKNOWN)
	{
		return	lpszModel[xModel];	
	}

	return	lpszModel[MDM_MODEL_UNKNOWN];
}

const char*	MDM_TypeName(MDM_TYPE xType)
{
	if (xType < MDM_TYPE_UNKNOWN)
	{
		return	lpszType[xType];	
	}

	return	lpszType[MDM_TYPE_UNKNOWN];
}

const char*	MDM_ManufacturereName(MDM_MANUFACTURER xManu)
{
	if (xManu < MDM_MANU_UNKNOWN)
	{
		return	lpszManufacturer[xManu];	
	}

	return	lpszManufacturer[MDM_MANU_UNKNOWN];
}
