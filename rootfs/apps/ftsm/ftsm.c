#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <jansson.h>
#include "ftsm.h"

static char	*ftsm_default_config_file = "/etc/ftsm.json";
FTSM_EP_INFO	*ep_infos = NULL;
int			ep_info_count = 0;

int	ftsm_init(char *filename)
{
	FILE			*fp;
	char			*buff = NULL;
	int				filesize;
	json_t			*root;
	json_error_t	error;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		return	-1;	
	}

	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	if (filesize != 0)
	{
		buff = (char *)calloc(1, filesize+1);	
		fread(buff, 1, filesize, fp);
	}

	fclose(fp);

	printf(buff);
	root = json_loads(buff, 0, &error);	
	if (buff != NULL)
	{
		free(buff);	
	}


	if (!root)
	{
		fprintf(stderr, "ERROR[%d] : %s\n", error.line, error.text);
	}

	json_decref(root);

	return	0;
}

int	ftsm_finish(void)
{
	if (ep_infos != NULL)
	{
		int	i;
	
		for(i = 0 ; i < ep_info_count ; i++)
		{
			free(ep_infos[i].buff);
		}

		free(ep_infos);

		ep_infos = NULL;
		ep_info_count = 0;
	}

	return	0;
}


int ftsm_print_config(void)
{
	if (ep_infos != NULL)
	{
		int	i;
	
		for(i = 0 ; i < ep_info_count ; i++)
		{
			printf("%s \n", ep_infos[i].title);
			printf("\t%s \n", ep_infos[i].oid.count);
			printf("\t%s \n", ep_infos[i].oid.id);
			printf("\t%s \n", ep_infos[i].oid.type);
		}

		free(ep_infos);

		ep_infos = NULL;
		ep_info_count = 0;
	}

	return	0;
}

int main(int argc, char *argv[])
{
	size_t			i;

	
	ftsm_init("./ftsm.json");
	return	0;
}
