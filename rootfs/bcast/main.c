#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <getopt.h>

#define PORT 2080
#define DEST_ADDR "192.168.1.255"
uint32_t stringToIP(char *pStr);

static struct option long_options[] =
{
};

int main(int argc, char *argv[])
{
	char    *lpszSrcIP = NULL;
	char    *lpszDestIP = NULL;
	char    *lpszMsg = "Hello?";
	unsigned short  nDestPort = 1234;
	int             opt, opt_idx;
	int sockfd;
	int broadcast=1;
	struct sockaddr_in sendaddr;
	struct sockaddr_in recvaddr;
	int numbytes;

	while((opt = getopt_long(argc, argv, "s: d: p: m:", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
			case    's': lpszSrcIP = optarg; break;
			case    'd': lpszDestIP = optarg; break;
			case    'p': nDestPort = atoi(optarg); break;
			case    'm': lpszMsg = optarg; break;
		}
	}

	if((sockfd = socket(PF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("sockfd");
		exit(1);
	}

	if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,
					&broadcast,sizeof broadcast)) == -1)
	{
		perror("setsockopt - SO_SOCKET ");
		exit(1);
	}

	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = 1234;
	sendaddr.sin_addr.s_addr = INADDR_ANY;
	memset(sendaddr.sin_zero,'\0',sizeof sendaddr.sin_zero);

	if(bind(sockfd, (struct sockaddr*) &sendaddr, sizeof sendaddr) == -1)
	{
		perror("bind");
		exit(1);
	}

	recvaddr.sin_family = AF_INET;
	recvaddr.sin_port = htons(nDestPort);
	recvaddr.sin_addr.s_addr = inet_addr(lpszDestIP);
	memset(recvaddr.sin_zero,'\0',sizeof recvaddr.sin_zero); 

	if ((numbytes = sendto(sockfd, lpszMsg, strlen(lpszMsg) , 0,
					(struct sockaddr *)&recvaddr, sizeof recvaddr)) == -1) 
	{
		printf("Send failed."); 
	}                                                


	close(sockfd);          

	return 0;            
}                                                             

