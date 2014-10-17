#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/signal.h> 
#include <sys/ioctl.h> 
#include <sys/poll.h> 
#include "xserial.h"
#include "xftmbcc.h"

#define SERIAL_CONTROL  (CS8 | CREAD)
#define NO_PARITY_CHECK 0
#define PARITY_CHECK    1
#define SERIAL_INPUT    PARMRK
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

	typedef void (*serial_handler)(int status);
	typedef void (*serial_handler_IO)(int status, siginfo_t *ioinfo, void *context);
int serialConfiguration(serial_handler handler, serial_handler_IO handler_IO, const char *device,
				tcflag_t baudrate, char parity);
int nWaitFlag=TRUE;                    /* TRUE while no signal received */
void PMUC_SignalHandler(int status)
{
	nWaitFlag = FALSE;
}

void PMUC_SignalIOHandler(int nStatus, siginfo_t *pIOInfo, void *pContext)
{
	switch(pIOInfo->si_code)
	{
	case	POLL_IN:
		nWaitFlag = FALSE;
		break;

	case	POLL_OUT:
		break;
	}
}

//------------------------------------------------------------------------------ 
// Open Serial Port
//------------------------------------------------------------------------------ 
int open_serial( char *dev_name, int baud, struct termios *oldtio) 
{ 
	return	serialConfiguration(PMUC_SignalHandler, PMUC_SignalIOHandler, MODEMDEVICE, BAUDRATE, NO_PARITY_CHECK);
#if 0
    int fd; 
    struct termios newtio; 
    
    fd = open( dev_name, O_RDWR | O_NOCTTY ); 
    if ( fd < 0 ) 
    { 
        xError("Device OPEN FAIL %s\n", dev_name ); 
        return -1; 
    } 
  
    if (oldtio != NULL)
    {
        tcgetattr(fd, oldtio);
    }

    memset(&newtio, 0, sizeof(newtio)); 
    newtio.c_iflag = IGNPAR; // non-parity 
    newtio.c_oflag = 0; 
    newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts
    switch( baud ) 
    { 
        case 115200 : newtio.c_cflag |= B115200; break; 
        case 57600 : newtio.c_cflag |= B57600; break; 
        case 38400 : newtio.c_cflag |= B38400; break; 
        case 19200 : newtio.c_cflag |= B19200; break; 
        case 9600 : newtio.c_cflag |= B9600; break; 
        case 4800 : newtio.c_cflag |= B4800; break; 
        case 2400 : newtio.c_cflag |= B2400; break; 
        default : newtio.c_cflag |= B115200; break; 
    } 

    newtio.c_lflag = oldtio->c_lflag | ISIG | ICANON ; 
    newtio.c_oflag = OPOST | ONLCR; 
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0; 
    tcflush ( fd, TCIFLUSH ); 
    tcsetattr( fd, TCSANOW, &newtio ); 

    return fd; 
#endif
} 

int serialConfiguration(serial_handler handler, serial_handler_IO handler_IO, const char *device,
				tcflag_t baudrate, char parity)
{
	/* Definition of new serial port configuration */
	struct termios newtio;
	/* Definition of signal action */
	struct sigaction saio;

	/* Open the device
	 * O_RDWR     : open in read/write mode
	 * O_NONBLOCK : open in not blocked mode. Read will return immediatly */
	int fdesc = open(device, O_RDWR | O_NONBLOCK);
	if (fdesc < 0)
	{
		perror(device);
		return fdesc;
	}

	/* Install the signal handler before making the device asynchronous */

	sigemptyset(&saio.sa_mask);
	saio.sa_handler = handler;
	saio.sa_flags = 0;
	//saio.sa_sigaction= handler_IO;
	//saio.sa_flags = saio.sa_flags | SA_SIGINFO;
	saio.sa_restorer = NULL;
	sigaction(SIGIO, &saio, NULL);

	/* Allow the process to receive SIGIO */
	fcntl(fdesc, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous.
	 * It is not possible to enable SIGIO receiving by specifying
	 * O_ASYNC when calling open (see open man page)  */
	fcntl(fdesc, F_SETFL, FASYNC);

	/* Set new port settings */
	newtio.c_cflag = baudrate | CLOCAL | SERIAL_CONTROL;
	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME]= 0;
	/* Input settings */
	if (parity == NO_PARITY_CHECK)
	{
		newtio.c_iflag = IGNPAR | SERIAL_INPUT;
	}
	else
	{
		newtio.c_iflag = SERIAL_INPUT;
	}

	tcflush(fdesc, TCIFLUSH);
	tcflush(fdesc, TCOFLUSH);
	tcsetattr(fdesc, TCSANOW, &newtio);

	return fdesc;
}
//------------------------------------------------------------------------------ 
// Close Serial Port
//------------------------------------------------------------------------------
void close_serial( int fd ) 
{ 
    close( fd ); 
} 
