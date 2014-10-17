#ifndef _XCOM_H_
#define _XCOM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <signal.h>
#include <bits/siginfo.h>

/* Controls :  CS8      = 8 bits
 *             CREAD    = Enable reading */
#define SERIAL_CONTROL  (CS8 | CREAD)
#define NO_PARITY_CHECK 0
#define PARITY_CHECK    1

/* Input    :  PARMRK   = If IGNPAR is not set, prefix a character with a parity error  or
			   framing  error  with  \377  \0 */
#define SERIAL_INPUT    PARMRK

typedef	struct
{
	int	hPort;
	int	nFrameTimeout;
	int	nBaudrate;
	int	nParity;

	struct termios	oldtio;
}	XCOM;


int	XCOM_Open(char *lpszDevice, int nBaudrate, int nParity, XCOM *phCOM);
int XCOM_Close(XCOM *phCOM);
int	XCOM_Write(XCOM *phCOM, const char *lpszBuf, int nBuf);
int XCOM_Read(XCOM *phCOM, char *lpszBuf, int nMaxBuf, int nTimeout);


#ifdef __cplusplus
}
#endif

#endif
