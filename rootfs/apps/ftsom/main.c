#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "xshared.h"
#include "debug.h"
#include "ftsom.h"
#include "libconfig.h"



typedef	enum _FT_SOM_CMD
{
	FT_SOM_CMD_UNKNOWN 	= 0,
	FT_SOM_CMD_ADD,
	FT_SOM_CMD_GET,
	FT_SOM_CMD_SET,
	FT_SOM_CMD_PRINT
}	FT_SOM_CMD;

int				cfg_load(config_t *cfg, char *filename);
static char		*cfg_filename = "/etc/ftsom.conf";

static struct option long_options[] =
{
};


int main(int argc, char *argv[])
{
	FT_SOM_CMD	cmd = FT_SOM_CMD_UNKNOWN;
	int		opt, opt_idx;
	FT_OBJECT_ID	id = 0;
	char	*name = 0;
	char	*type = 0;
	char	*value = 0;
	char	*time = 0;
	key_t	key = 0x1233;

	while((opt = getopt_long(argc, argv, "ac: gi: n: psT: t: v: V", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	'a':
			cmd = FT_SOM_CMD_ADD;
			break;

		case	'c':
			cfg_filename = optarg;
			break;

		case	'g':
			if (FT_SOM_CMD_UNKNOWN != cmd)
			{
				goto error;
			}
			cmd = FT_SOM_CMD_GET;
			break;

		case	'i':
			id = strtoul(optarg, NULL, 16);
			if (errno == ERANGE)
			{
				fprintf(stderr, "%s: invalid ID\n", argv[0]);
				goto error;
			}
			break;

		case	'n':
			name = optarg;
			break;

		case	'p':
			cmd = FT_SOM_CMD_PRINT;
			break;

		case	's':
			if (FT_SOM_CMD_UNKNOWN != cmd)
			{
				goto error;
			}
			cmd = FT_SOM_CMD_SET;
			break;

		case	'T':
			type = optarg;
			break;

		case	't':
			time = optarg;
			break;

		case	'v':
			value = optarg;
			break;

		case	'V':
			XTRACE_ON();
			break;

		default:
			{
				fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], opt);
				return	0;
			}
		}
	}

	key = ft_som_init(NULL);	
	if (0 == key)
	{
		fprintf(stderr, "%s: Shared memory allocation failed!\n", argv[0]);
		return	0;	
	}

	switch(cmd)
	{
	case	FT_SOM_CMD_ADD:
		{
			if (!id)
			{
				goto error;
			}
			else
			{
				if (!ft_som_obj_is_exist(id))
				{
					printf("add new object : %08x\n", id);
					ft_som_obj_add(id);

					if (0 != type)
					{
						ft_som_obj_type_set(id, type);
					}

					if (0 != name)
					{
						ft_som_obj_name_set(id, name);
					}

					printf("OK\n");
				}
				else
				{
					printf("FAILED\n");	
				}
			}
		}	
		break;

	case	FT_SOM_CMD_GET:
		{
			if (!id)
			{
				fprintf(stderr, "ERROR : Invalid set parameters\n");	
				goto error;
			}
			else
			{
				char	value[FT_SOM_VALUE_LEN+1];
				char	time[FT_SOM_VALUE_LEN+1];
				if (ft_som_value_get(id, value, sizeof(value), time, sizeof(time)) != 0)
				{
					goto error;
				}
				printf("%s %s\n", value, time);	
			}
		}
		break;

	case	FT_SOM_CMD_SET:
		{
			if (!id || !value || !time)
			{
				fprintf(stderr, "ERROR : Invalid set parameters\n");	
				goto error;
			}
			else
			{
				ft_som_value_set(id, value, time);
			}
		}
		break;
	case	FT_SOM_CMD_PRINT:
	default:
		{
			ft_som_print();
		}
		break;

	}

	return	0;

error:
	return	-1;
}


int		cfg_load(config_t *cfg, char *filename)
{
	config_setting_t	*points;
	const char			*str;
	int					i;

	config_init(cfg);	
	if (CONFIG_TRUE != config_read_file(cfg, filename))
	{
		fprintf(stderr, "Configuration file loading failed\n");
		return	-1;	
	}

	config_lookup_string(cfg, "devid", &str);
	ft_som_set_devid((char*)str);
	config_lookup_string(cfg, "model", &str);
	ft_som_set_model((char*)str);

	points = config_lookup(cfg, "points");
	for(i = 0 ; ; i++)
	{
		config_setting_t* 	point;
		const char*			value;
		FT_OBJECT_ID		id;
		const char*			type;
		const char*		  	name;
		
		point = config_setting_get_elem(points, i);
		if (NULL == point)
		{
			break;	
		}

		value	= config_setting_get_string_elem(point, 0);
		id = strtoul(value, NULL, 16);
		type	= config_setting_get_string_elem(point, 1);
		name 	= config_setting_get_string_elem(point, 2);

		if( !ft_som_obj_add(id))
		{
			ft_som_obj_name_set(id, (char *)type);
			ft_som_obj_name_set(id, (char *)name);
		}
	}

	return	0;
}
