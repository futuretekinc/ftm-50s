#ifndef __XUTIL_H__
#define __XUTIL_H__

typedef	enum
{
	PRCS_U	=	'U',
	PRCS_D	=	'D',
	PRCS_R	=	'R',
	PRCS_T	=	'T',
	PRCS_Z	=	'Z',
	PRCS_W	=	'W',
	PRCS_N	=	'N',
	PRCS_L	=	'L',
	PRCS_S	=	'S'
}	PRC_STATE;

unsigned int getProcessID(char *p_processname) ;
unsigned int getProcessIDList(char *p_processname, int *lpPIDs, int nMaxCount) ;
PRC_STATE	getProcessState(int nPID);
#endif

