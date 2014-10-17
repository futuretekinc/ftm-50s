#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "qdecoder.h"
#include <unistd.h> /* UNIX standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <signal.h>
#include <sys/time.h>
#include "qdecoder.h"
#define MODEM "/dev/ttyS1"
#define BUFSIZE 255
#define BAUDRATE B115200
#define FALSE 0
#define TRUE 1

//FILE *fp;
//int wait_flag=TRUE;

//void signal_handler_IO (int status)
//{
//		printf("received SIGIO signal.\n");
//		wait_flag = FALSE;
//}

void check()
{
		FILE *rssiFP;
		char rssiBuf[256];
		rssiFP = popen("cat /tmp/rssi.log | awk '/RFSTS:/' | sed -e 's/,/ /g' | awk 'END {print $7}'", "r");
		if (rssiFP != NULL)
		{
				while (fgets(rssiBuf, sizeof(rssiBuf), rssiFP))
				{
						rssiBuf[strlen(rssiBuf)-1] = 0;
						int num;
						num = atoi(rssiBuf);
						//						printf("rssi : %d\n", num);

						FILE *mdmLiveFP;
						char mdmBuf[256];
						//mdmLiveFP = popen("service mdmctrl is_live", "r");

						if (num < -100)
						{
								//FILE *mdmLiveFP;
								//mdmBuf[256];
								mdmLiveFP = popen("service pppd status", "r");
								if (mdmLiveFP != NULL)
								{
										while(fgets(mdmBuf, sizeof(mdmBuf), mdmLiveFP))
										{
												mdmBuf[strlen(mdmBuf)-1] = 0;
												//												printf ("mdm is live : %s\n", mdmBuf);
												if (strcmp(mdmBuf, "running") == 0)
												{
														// modem is live
														FILE *mdmfp1;
														mdmfp1 = popen("killall mmon;service pppd stop", "r");
														if (mdmfp1 != NULL)
														{
																pclose(mdmfp1);
														}
														//mdmLiveFP = popen("killall mmon;service pppd stop;service mdmctrl stop", "r");
												} else 
												{
														// modem is stop
														//mdmLiveFP = popen("killall mmon;service pppd stop;mmon -d;service mdmctrl start", "r");
														//mdmLiveFP = popen("killall mmon;service pppd stop;service mdmctrl stop", "r");
												}

										}
										pclose(mdmLiveFP);
								}
						} else {

								mdmLiveFP = popen("service pppd status", "r");
								if (mdmLiveFP != NULL)
								{
										while(fgets(mdmBuf, sizeof(mdmBuf), mdmLiveFP))
										{
												mdmBuf[strlen(mdmBuf)-1] = 0;
												//												printf ("mdm is live : %s\n", mdmBuf);
												if (strcmp(mdmBuf, "running") == 0)
												{
														// modem is live
														//mdmLiveFP = popen("killall mmon;service pppd stop;service mdmctrl stop", "r");
												} else 
												{
														// modem is stop
														FILE *mdmfp2;
														mdmfp2 = popen("service pppd stop;mmon -d", "r");
														if (mdmfp2 != NULL)
														{
																pclose(mdmfp2);
														}

												}
										}
										pclose(mdmLiveFP);
								}


						}

				}
				pclose(rssiFP);
		}
}

int open_port(char *port)
{
		struct termios options;
		//struct sigaction saio; // add
		int fd;
		fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd == -1)
		{
				printf("open_port: Unable to open the port - ");
		}
		else
		{
				printf ( "Port %s with file descriptor=%i",port, fd);
				fcntl(fd, F_SETFL, FNDELAY);

				//saio.sa_handler = signal_handler_IO;
				//saio.sa_mask = 0;
				//saio.sa_flags = 0;
				//saio.sa_restorer = NULL;
				//sigaction(SIGIO,&saio,NULL);

				tcgetattr( fd, &options );
				cfsetispeed( &options, BAUDRATE );
				cfsetospeed( &options, BAUDRATE );
				options.c_cflag |= ( CLOCAL | CREAD);
				options.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CSIZE);
				options.c_cflag |= CS8;
				options.c_cflag &= ~CRTSCTS;
				options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
				options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR);
				options.c_oflag &= ~OPOST;
				
				/*
				if ( tcsetattr( fd, TCSANOW, &options ) == -1 )
						printf ("Error with tcsetattr = %s\n", strerror ( errno ) );
				else        
						printf ( "%s\n", "succeed" );
						*/
		}
		return (fd);
}

void timer_handler (int signum)
{
		int STOP=FALSE;
		int res;
		int fd = open_port(MODEM);
		char buf[BUFSIZE] = {0, };
		//bzero (buf, sizeof(buf));
		//read(fd, buf, sizeof(buf));
		//strcpy(buf, "AT#RFSTS\r");
		//write (fd, buf, strlen(buf));
		write(fd, "AT#RFSTS\r", strlen("AT#RFSTS\r"));
		
		FILE *fp;
		fp = fopen("/tmp/isserial", "w");
		fprintf(fp, "used");
		fclose(fp);

		while (STOP==FALSE) {
				//if (wait_flag==FALSE)
				//{
						res = read(fd, buf, BUFSIZE);
						buf[res]=0;
						if (strstr(buf, "OK") != NULL) {
								STOP=TRUE;
								//qcgires_setcontenttype(req, "text/xml");
								//printf("<data>\n");
								//printf("<res>OK</res>\n");
								//printf("<text>%s</text>\n", buf);
								//printf("</data>");
								fp = fopen("/tmp/rssi.log", "w");
								fprintf(fp, "%s", buf);
								fclose(fp);
								check();
						}
						else if (strstr(buf, "ERROR") != NULL) {
								STOP=TRUE;
								//qcgires_setcontenttype(req, "text/xml");
								//printf("<data>\n");
								//printf("<res>ERROR</res>\n");
								//printf("<text>%s</text>\n", buf);
								//printf("</data>\n");
								fp = fopen("/tmp/rssi.log", "w");
								fprintf(fp, "%s", buf);
								fclose(fp);

						}
						//if (res==1)
						//{
						//		STOP=TRUE;
						//}
						//wait_flag = TRUE;
				//}

		}
		
		close(fd);

		fp = fopen("/tmp/isserial", "w");
		fprintf(fp, "none");
		fclose(fp);

		/*
		FILE *fp;
		fp = popen("cat /tmp/log | awk '/RFSTS:/' | sed -e 's/,/ /g' | awk 'END {print $7}'", "r");
		if (fp != NULL)
		{
			pclose(fp);
		}
		*/
}

void phoneNumInit()
{
		int STOP=FALSE;		
		int res;
		int serialFD = open_port(MODEM);
		char numbuf[BUFSIZE];
		FILE *fp;

		write(serialFD, "AT+CNUM\r" , strlen("AT+CNUM\r"));
		while (STOP==FALSE) {

				res = read(serialFD, numbuf, BUFSIZE);
				numbuf[res]=0;
				//printf("%s:%d\n", buf, res);

				if (strstr(numbuf, "OK") != NULL) {
						STOP=TRUE;
						fp = fopen("/tmp/pNum.log", "w");
						fprintf(fp, "%s", numbuf);
						fclose(fp);
						
						fp = popen ("cp /tmp/pNum.log /etc/pNum.log; sync", "r");
						pclose(fp);
						fp = popen ("rm /tmp/pNum.log", "r");
						pclose(fp);


				}
				else if (strstr(numbuf, "ERROR") != NULL) {
						STOP=TRUE;
						fp = fopen("/tmp/pNum.log", "w");
						fprintf(fp, "%s", numbuf);
						fclose(fp);
				}

		}
		close(serialFD);
}


int main()
{
		//phoneNumInit();

		struct sigaction sa;
		struct itimerval timer;

		/* Install timer_handler as the signal handler for SIGVTALRM. */
		memset (&sa, 0, sizeof (sa));
		sa.sa_handler = &timer_handler;
		sigaction (SIGVTALRM, &sa, NULL);

		/* Configure the timer to expire after 250 msec... */
		timer.it_value.tv_sec = 5;
		timer.it_value.tv_usec = 0; //2500;

		/* ... and every 250 msec after that. */
		timer.it_interval.tv_sec = 5;
		timer.it_interval.tv_usec = 0; // 2500;

		/* Start a virtual timer. It counts down whenever this process is executing. */
		setitimer (ITIMER_VIRTUAL, &timer, NULL);
		while(1);
		return 0;
}
