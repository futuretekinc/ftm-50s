#ifndef	__MODEM_H__
#define	__MODEM_H__

typedef	enum
{
	MDM_ERROR_OK = 0,
	MDM_ERROR_INVALID,
	MDM_ERROR_TIMEOUT,
	MDM_ERROR_CANT_OPEN_DEVICE
}	MDM_ERROR;

typedef	enum
{
	MDM_MODEL_TX200K	=	0,
	MDM_MODEL_UC864_E	=	1,
	MDM_MODEL_UC864_K	=	2,
	MDM_MODEL_UNKNOWN
}	MDM_MODEL;

typedef	enum
{
	MDM_TYPE_3G			=	0,
	MDM_TYPE_LTE		=	1,
	MDM_TYPE_UNKNOWN
}	MDM_TYPE;

typedef	enum
{
	MDM_MANU_TELIAN		=	0,
	MDM_MANU_TELIT		=	1,
	MDM_MANU_UNKNOWN
}	MDM_MANUFACTURER;
	
typedef struct	
{
	MDM_MODEL			xModel;
	MDM_TYPE			xType;
	MDM_MANUFACTURER	xManufacturer;
}	MDM_MODEM_INFO;

typedef	struct
{
	XCOM		hCOM;
	MDM_MODEL	xModel;
	char		szDevice[1024];
	int			bEcho;
	int			nBaudrate;
	int			nTimeout;
}	MDM;

#define	MDM_RIDX_MODEL		0

#define	MDM_DEFAULT_SPEED	115200
#define	MDM_DEFAULT_TIMEOUT	1000000

int	 MDM_IsLive(void);
void MDM_Start(void);
void MDM_Stop(void);
void MDM_Restart(void);

int	MDM_Init(MDM *hMDM, char *lpszDevice, int nBaudrate, int nTimeout);

MDM_ERROR	MDM_Connect(MDM *phDesc);
MDM_ERROR	MDM_Disconnect(MDM *phDesc);

int	MDM_IsRunning( MDM *phDesc);
MDM_ERROR MDM_SetEcho( MDM *phDesc, int on);
MDM_ERROR MDM_EchoOff( MDM *phDesc);
MDM_ERROR MDM_EchoOn( MDM *phDesc);
MDM_ERROR MDM_GetModel( MDM *phDesc, MDM_MODEL *pModel );
MDM_ERROR MDM_GetType( MDM *phDesc, MDM_TYPE *pType);
MDM_ERROR MDM_GetManufacturerModel( MDM *phDesc, MDM_MANUFACTURER *pManu);

const char*	MDM_ModelName(MDM_MODEL xModel);
const char*	MDM_TypeName(MDM_TYPE xType);
const char*	MDM_ManufacturereName(MDM_MANUFACTURER xManu);
#endif

