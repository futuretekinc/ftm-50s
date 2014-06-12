#ifndef	__XPPP_H__
#define __XPPP_H__

typedef	enum
{
	XP3_STATE_STOPPED=0,
	XP3_STATE_RUNNING
}	XP3_STATE;

XP3_STATE	XP3_Restart(void);
XP3_STATE	XP3_Start(void);
XP3_STATE	XP3_Stop(void);
XP3_STATE	XP3_Status(void);

#endif //__XPPP_H__
