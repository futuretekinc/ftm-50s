#ifndef	__XSHARED_H__
#define	__XSHARED_H__

typedef	struct
{
	key_t	xKey;
	int		nSize;
}	XSD_INFO;
	
int		XSD_Init(void);
int		XSD_IsExist(int nID);
void	*XSD_GetMemAddr(int nID);
#endif

