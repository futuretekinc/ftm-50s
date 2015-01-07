#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sqlite3.h> 
#include <getopt.h>
#include "ftdm.h"
#include "debug.h"

typedef	enum	_FTDM_CMD
{
	FTDM_CMD_UNKNOWN = 0,
	FTDM_CMD_SENSOR_ADD,
	FTDM_CMD_APPEND_DATA,
	FTDM_CMD_SENSOR_LIST,
	FTDM_CMD_INIT
} FTDM_CMD;

void print_usage(char *program);

static struct option long_options[] = 
{

};

static char *default_config_filename = "./ftdm.conf";

int main(int argc, char* argv[])
{
	int			opt, opt_idx;
	FTDM_CMD	cmd = FTDM_CMD_UNKNOWN;	
	char 		*config_filename = default_config_filename;
	char		*id = NULL;
	char		*type = NULL;
	char		*name = "";
	char		*sn = "";
	char		*value = "";

	while((opt = getopt_long(argc, argv, "ADILc: i: n: Ss: t: v:", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	'D':
			if (cmd != FTDM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTDM_CMD_APPEND_DATA;
			break;

		case	'A':
			if (cmd != FTDM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTDM_CMD_SENSOR_ADD;
			break;

		case	'I':
			if (cmd != FTDM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTDM_CMD_INIT;
			break;

		case	'L':
			if (cmd != FTDM_CMD_UNKNOWN)
			{
				goto cmd_duplicated_error;			
			}
			cmd = FTDM_CMD_SENSOR_LIST;
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

		case	'v':
			value = optarg;

		}
	}


	ftdm_open(config_filename);

	switch(cmd)
	{
	case	FTDM_CMD_SENSOR_ADD:
		{
			int	exist = 0;

			if ((id == NULL) || (type == NULL))
			{
				print_usage(argv[0]);
			}

			if (ftdm_sensor_is_exist(id, &exist) == 0)
			{
				if (exist == 0)
				{
					ftdm_sensor_add(id, type, name, sn);
				}
				else
				{
					TRACE("Sensor %s is exist\n", id); 	
				}
			}
		}
		break;

	case	FTDM_CMD_SENSOR_LIST:
		{
		}
		break;

	case	FTDM_CMD_APPEND_DATA:
		if ((id == NULL) || (value == NULL))
		{
			print_usage(argv[0]);
		}
		else 
		{
			time_t	current_time;
			struct tm *time_info;
			char	time_string[128];

			time(&current_time);
			time_info = gmtime(&current_time);

			strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);
			ftdm_data_add(id, value, time_string);
		}
		break;

	case	FTDM_CMD_INIT:
		break;

	default:
		{
			int	i, count;
			FTDM_ID	id_list[16];
			char	type[32];	
			char	name[32];	
			char	sn[32];	

			ftdm_sensor_count(&count);
			printf("SENSOR COUNT : %d\n", count);
			ftdm_sensor_list_get(1, 16, id_list, &count);
			printf("     %8s %16s %16s %16s\n", "ID", "TYPE", "NAME", "S/N");
			printf("--------------------------------------------------------\n");
			for(i = 0 ; i < count ; i++)
			{
				ftdm_sensor_get(id_list[i], type, 32, name, 32, sn, 32);
				printf("%2d : %8s %16s %16s %16s\n", i+1, id_list[i], type, name, sn);
			}
			
		}
		//print_usage(argv[0]);
	}

	goto finished;

cmd_duplicated_error:
	printf("Command duplicated\n");

finished:
	ftdm_close();
	return 0;
}

void print_usage(char *program)
{
	printf("Usage : %s [OPTOINS]\n", program);
	printf("OPTIONS include:\n");
	printf("   -A <id>        Add new points\n");
}
