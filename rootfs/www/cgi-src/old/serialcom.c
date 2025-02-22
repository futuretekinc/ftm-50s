#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <serialcom.h>

#define	_POSIX_SOURCE 1

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

#ifdef __cplusplus
}
#endif
