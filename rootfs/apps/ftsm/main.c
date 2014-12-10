#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h> 
#include <getopt.h>
#include "ftsm.h"

typedef	enum	_FTSM_CMD
{
	FTSM_CMD_UNKNOWN = 0,
	FTSM_CMD_SENSOR_ADD,
	FTSM_CMD_APPEND_DATA,
	FTSM_CMD_SENSOR_LIST,
	FTSM_CMD_INIT
} FTSM_CMD;

void print_usage(char *program);

static struct option long_options[] = 
{

};

static char *default_config_filename = "./ftsm.conf";

int main(int argc, char* argv[])
{
	int			opt, opt_idx;
	FTSM_CMD	cmd = FTSM_CMD_UNKNOWN;	
	char 		*config_filename = default_config_filename;
	char		*id = NULL;
	char		*type = NULL;
	char		*name = "";
	char		*sn = "";

	while((opt = getopt_long(argc, argv, "AILc: i: n: Ss: t: ", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	'A':
			if (cmd != FTSM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTSM_CMD_APPEND_DATA;
			break;

		case	'S':
			if (cmd != FTSM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTSM_CMD_SENSOR_ADD;
			break;

		case	'I':
			if (cmd != FTSM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTSM_CMD_INIT;
			break;

		case	'L':
			if (cmd != FTSM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTSM_CMD_SENSOR_LIST;
			break;

		case	'c':
			config_filename = optarg;
			break;
		
		case	'i':
			id = optarg;
			break;

		case	'n':
			name = optarg;
			break;

		case	's':
			sn = optarg;
			break;

		case	't':
			type = optarg;
			break;

		}
	}


	ftsm_open(config_filename);

	switch(cmd)
	{
	case	FTSM_CMD_SENSOR_ADD:
		if ((id == NULL) || (type == NULL))
		{
			print_usage(argv[0]);
		}
		else if (ftsm_sensor_value_db_create(id) != 0)
		{
			printf("Sensor(%s) DB creation failed\n", id);
		}
		break;

	case	FTSM_CMD_SENSOR_LIST:
		{
			ftsm_sensor_value_db_list();
		}
		break;

	case	FTSM_CMD_APPEND_DATA:
		if ((id == NULL) || (type == NULL))
		{
			print_usage(argv[0]);
		}
		else 
		{
			printf("Sensor 		");
		}
		break;

	case	FTSM_CMD_INIT:
		ftsm_sensors_info_db_create("sensors");
		break;

	default:
		print_usage(argv[0]);
	}

	goto finished;

cmd_duplicated_error:
	printf("Command duplicated\n");

finished:
	ftsm_close();
	return 0;
}

void print_usage(char *program)
{
	printf("Usage : %s [OPTOINS]\n", program);
	printf("OPTIONS include:\n");
	printf("   -A <id>        Add new points\n");
}
