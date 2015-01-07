#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sqlite3.h> 
#include <getopt.h>
#include "ftsm.h"
#include "debug.h"


void print_version(char *program);
void print_usage(char *program);

static struct option long_options[] = 
{

};

int main(int argc, char* argv[])
{
	int			opt, opt_idx;

	while((opt = getopt_long(argc, argv, "v", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	'v':
			print_version(argv[0]);
			break;

		default:
			print_usage(argv[0]);
		}
	}

	return 0;
}

void print_version(char *program)
{
	printf("%s Version 1.0.0.0\n", program);
}

void print_usage(char *program)
{
	printf("Usage : %s [OPTOINS]\n", program);
	printf("OPTIONS include:\n");
	printf("   -A <id>        Add new points\n");
}
