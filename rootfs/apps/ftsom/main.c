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



int 		sk_save(key_t key);
key_t 		sk_load(void);

int				cfg_load(config_t *cfg, char *filename);
static char		*config_filename = "/etc/ftsom.conf";
static config_t	main_config;
static struct option long_options[] =
{
};


int main(int argc, char *argv[])
{
	int		opt, opt_idx;
	key_t	key = 0x1233;

	while((opt = getopt_long(argc, argv, "c: V", long_options, &opt_idx)) != -1)
	{
		switch(opt)
		{
		case	'c':
			config_filename = optarg;
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

	cfg_load(&main_config, config_filename);

	key = sk_load();
	if (0 != key)
	{
		key_t	new_key;

		new_key = ft_som_init(key);	
		if (0 == new_key)
		{
			fprintf(stderr, "%s: Shared memory allocation failed!\n", argv[0]);
			return	0;	
		}

		if (new_key != key)
		{
			sk_save(new_key);	
		}
	}
	
	if (1 == argc)
	{
		ft_som_print();

		return	0;
	}

	
	return	0;
}

key_t sk_load(void)
{
	FILE *fp = fopen("/var/ftsom/ftsom.key", "w");
	if (NULL != fp)
	{
		int	value;

		if (fscanf(fp, "%x", &value) != 0)
		{
			return	(key_t)value;	
		}
	}


	return	0;
}

int sk_save(key_t key)
{
	FILE	*fp;
	struct stat st;

	if (stat("/var/ftsom", &st) == -1)
	{
		mkdir("var/ftsom", 0700);	
	}

	fp = fopen("/var/ftsom/ftsom.key", "w");
	if (NULL == fp)
	{
		return	-1;	
	}


	fprintf(fp, "%04x", key);
	fclose(fp);

	return	0;
}

int		cfg_load(config_t *cfg, char *filename)
{
	const char	*str;

	config_init(cfg);	
	if (CONFIG_TRUE != config_read_file(cfg, filename))
	{
		config_destroy(cfg);
		return	-1;	
	}

	config_lookup_string(cfg, "devid", &str);
	printf("devid = %s\n", str);

	return	0;
}
