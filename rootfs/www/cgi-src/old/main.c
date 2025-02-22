#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

void signal_handler_IO (int status);   /* definition of signal handler */
int wait_flag=TRUE;                    /* TRUE while no signal received */

int main(int argc, char *argv[])
{
	int fd;
	int c, res, id = 0, on = 0;
	struct termios oldtio,newtio;
	struct sigaction saio;           /* definition of signal action */
	char buf[255];

	id = atoi(argv[1]);	
	on = atoi(argv[2]);	
	
	/* open the device to be non-blocking (read will return immediatly) */
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd <0) 
	{	
		perror(MODEMDEVICE); 
		exit(-1); 
	}

	/* install the signal handler before making the device asynchronous */
	saio.sa_handler = signal_handler_IO;
	sigemptyset(&saio);
	//saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	/* allow the process to receive SIGIO */
	fcntl(fd, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fd, F_SETFL, FASYNC);

	tcgetattr(fd,&oldtio); /* save current port settings */
	/* set new port settings for canonical input processing */
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	buf[0] = 0xFA;
	buf[1] = (0xF1 + id + on * 4);
	write(fd, buf, 2);

	/* loop while waiting for input. normally we would do something
	   useful here */
	printf("%s[%d]\n", __FUNCTION__, __LINE__);
	while (STOP==FALSE) 
	{
		usleep(100000);
		/* after receiving SIGIO, wait_flag = FALSE, input is available
		   and can be read */
		if (wait_flag==FALSE) 
		{
			int	i;

			res = read(fd,buf,255);
			buf[res]=0;
			for(i = 0 ; i < res ; i++)
			{
				printf("%02x ", buf[i]);
			}
			printf("\n");
			if (res!=1) 
			{
				STOP=TRUE; /* stop loop if only a CR was input */
			}
			else
			{
				wait_flag = TRUE;      /* wait for new input */
			}
		}
	}
	/* restore old port settings */
	tcsetattr(fd,TCSANOW,&oldtio);

	close(fd);

	return	0;
}
/***************************************************************************
 * signal handler. sets wait_flag to FALSE, to indicate above loop that     *
 * characters have been received.                                           *
 ***************************************************************************/

void signal_handler_IO (int status)
{
	printf("received SIGIO signal.\n");
	wait_flag = FALSE;
}


