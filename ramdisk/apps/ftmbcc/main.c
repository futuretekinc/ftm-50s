#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include "xftmbcc.h"
#include "xserial.h"
#include "tx200k.h"
#include "xshared.h"

static struct option long_options[] = 
{
	{"daemon", 0, 0, 0}
};

int main(int argc, char *argv[])
{
	int		opt, option_index, onDaemon = 0;

	while((opt = getopt_long(argc, argv, "d", long_options, &option_index)) != -1)
	{
		switch(opt)
		{
		case	0:
			{
				printf("Long option : %s\n", long_options[option_index].name);	
			}
			break;
		case	'd':
			{
				onDaemon = 1;	
			}
			break;

		default:
			{
				printf("Usage : %s [-d]\n", argv[0]);
				exit(EXIT_FAILURE);	
			}
		}
	}


	if (XSD_Init() < 0)
	{
		xError("The shared memory initialization failed\n");
		exit(EXIT_FAILURE);	
	}

	if (onDaemon)
	{
		XFTMBCC_StartDaemon();
	}

	return	0;
}


