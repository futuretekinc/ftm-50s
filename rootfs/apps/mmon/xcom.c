#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <bits/siginfo.h>
#include "xcom.h"
#include "debug.h"

#define	_POSIX_SOURCE 1

#ifndef	FALSE
	#define	FALSE	0
#endif

#ifndef	TRUE
	#define	TRUE	1
#endif

#define	BAUDRATE	B115200


/** @brief Initializes the serial port settings and opens the serial
 *         port.
 *
 *  Before opening the port for itself this functions saves the old 
 *  port settings. It also registers the restore_old_port_settings
 *  function to be executed when the program exits for any reason.
 *
 *  @param int argc   : Number arguments on the command line.
 *  @param char * argv: [0] - The name of this program's executable.
 *                    : [1] - The complete serial port name to be
 *                            opened.
 *
 *  @return int : 0 - Success.
 *                Not Zero - Error.
 *....................................................................
 */
int	XCOM_Open(char *lpszDevice, int nBaudrate, int nParity, XCOM *phCOM)
{  
	phCOM->nFrameTimeout = 1000;


	//We are ignoring SIGIO signal since we are using "select" instead
	signal( SIGIO, SIG_IGN );

	//Open device to be non-blocking (read will return immediately)
	phCOM->hPort=  open( lpszDevice, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if( phCOM->hPort <= 0 ) 
	{ 
		XTRACE( "\nSerial port[%s] failed to open with error[%d].\n", lpszDevice, errno);
		return 1; 
	}

	XTRACE( "\nSerial port[%s] was successfully opened.\n", lpszDevice); 

	//Make serial port do asynchronous input/outpuAt.
	int ret_value = fcntl( phCOM->hPort, F_SETFL, O_ASYNC );
	if( ret_value != 0 )
	{
		XTRACE( "\nfcntl F_SETFL O_ASYNC on new port[%s] failed with "\
				"error[%d].\n", lpszDevice, errno);
		return 2; 
	}

	XTRACE( "\nfcntl F_SETFL O_ASYNC on new port[%s] succeeded.\n", lpszDevice); 

	//save old port settings
	ret_value = tcgetattr( phCOM->hPort, &phCOM->oldtio );
	if( ret_value != 0 )
	{
		XTRACE( "\nPreserving old port[%s] failed with error[%d].\n", lpszDevice, errno);
		return 3; 
	}
	XTRACE( "\nPreserving old port[%s] was successful.\n", lpszDevice); 


	struct termios	newtio;
	//clear up struct for new port
	if( memset( &newtio, 0, sizeof(newtio) ) < (void *)1 )
	{
		XTRACE( "\nClearing up new port[%s] failed with error[%d].\n", lpszDevice, errno);
		return 4; 
	}

	XTRACE( "\nClearing up new port[%s] was successful.\n", lpszDevice); 

	int	nFlagBaudrate = 0;
	switch(nBaudrate)
	{
	case 0:		nFlagBaudrate = B0;break;
	case 50:	nFlagBaudrate = B50;break;
	case 75:	nFlagBaudrate =	B75;break;
	case 110:	nFlagBaudrate = B110;break;
	case 134:	nFlagBaudrate =	B134;break;
	case 150:	nFlagBaudrate =	B150;break;
	case 200:	nFlagBaudrate =	B200;break;
	case 300:	nFlagBaudrate =	B300;break;
	case 600:	nFlagBaudrate =	B600;break;
	case 1200:	nFlagBaudrate =	B1200;break;
	case 1800:	nFlagBaudrate =	B1800;break;
	case 2400:	nFlagBaudrate =	B2400;break;
	case 4800:	nFlagBaudrate =	B4800;break;
	case 9600:	nFlagBaudrate =	B9600;break;
	case 19200:	nFlagBaudrate =	B19200;break;
	case 38400:	nFlagBaudrate =	B38400;break;
	case 57600:	nFlagBaudrate =	B57600;break;
	case 115200:nFlagBaudrate =	B115200;break;
	case 230400:nFlagBaudrate =	B230400;break;
	}

	//set new port 8N1 settings for non-canonical input processing  
	//must be NOCTTY, set baud rate
	newtio.c_cflag = nFlagBaudrate | CS8 | CLOCAL | CREAD;
	//(ignore parity | map '\r' to '\n')
	newtio.c_iflag = ( IGNPAR | ICRNL );
	newtio.c_oflag = 0; //ONLCR converts '\n' to CR-LF pairs
	newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_cc[VMIN] = 0;  //return as soon as there is at least 1-byte
	newtio.c_cc[VTIME] = 1; //otherwise it returns in 0.1 s regardless.

	ret_value = tcflush( phCOM->hPort, TCIFLUSH );
	if( ret_value != 0 )
	{
		XTRACE( "\nSetting up, flushing Input data to port[%s] "\
				"if not read, failed with error[%d].\n",
				lpszDevice,
				errno
			  );
		return 5; 
	}

	XTRACE( "\nSetting up, flushing Input data to port[%s] "\
			"if not read, was successful.\n",
			lpszDevice
		  ); 

	ret_value = tcsetattr( phCOM->hPort, TCSANOW, &newtio );
	if( ret_value != 0 )
	{
		XTRACE( "\nActivating port[%s] settings failed with error[%d].\n", lpszDevice, errno);
		return 6; 
	}

	XTRACE( "\nActivating port[%s] settings successful.\n", lpszDevice); 

	return 0;
}//END init_serial_port-----------------------------------------------


int	XCOM_Close(XCOM *phCOM)
{
	tcsetattr( phCOM->hPort, TCSANOW, &phCOM->oldtio );

	return	0;
}

int	XCOM_Write(XCOM *phCOM, const char *lpszBuf, int nLen)
{
	write(phCOM->hPort, lpszBuf, nLen);

	return	nLen;
}

int XCOM_Read(XCOM *phCOM, char *lpszBuf, int nMaxBuf, int nTimeout)
{
	int				nReadCount = 0;
	unsigned long	ulStartTime;
	struct timeval	tv;
	char			ch;
	int				nStop = 0;

	gettimeofday(&tv, NULL);	
	ulStartTime = tv.tv_sec*1000000 + tv.tv_usec;

	while(!nStop)
	{
		unsigned long ulCurrentTime;

		gettimeofday(&tv, NULL);
		ulCurrentTime = tv.tv_sec * 1000000 + tv.tv_usec;

		if (ulCurrentTime - ulStartTime > nTimeout)
		{
			break;	
		}

		while (read( phCOM->hPort, &ch, 1))
		{
			if (nReadCount < nMaxBuf)
			{
				lpszBuf[nReadCount++] = ch;	
			}
			else
			{
				nStop = 1;
				break;	
			}
		}

		usleep(phCOM->nFrameTimeout);

		if ( ! nStop )
		{
			if(read( phCOM->hPort, &ch, 1))
			{
				if (nReadCount < nMaxBuf)
				{
					lpszBuf[nReadCount++] = ch;	
				}
				else
				{
					nStop = 1;
				}
			}
			else
			{
				nStop = 1;	
			}
		}
	}

	
	lpszBuf[nReadCount] = 0;

	return	nReadCount;
}


#ifdef __cplusplus
}
#endif
