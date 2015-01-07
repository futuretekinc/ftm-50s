#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <jansson.h>
#include "ftsm.h"

static char	*ftsm_default_config_file = "/etc/ftsm.conf";
FTSM_EP_INFO	*ep_infos = NULL;
int			ep_info_count = 0;

int	ftsm_init(char *filename)
{
	config_t			config;
	config_setting_t 	*endpoints;
	char				*config_filename;
	int					endpoints_count;
	int					i;
	
	if (filename != NULL)
	{
		config_filename = filename;	
	}
	else
	{
		config_filename = ftsm_default_config_file;	
	}

	config_init(&config);

	if (config_read_file(&config, config_filename) != CONFIG_TRUE)
	{
		goto error;
	}

	endpoints = config_lookup(&config, "mibs.endpoints");
	if (endpoints == NULL)
	{
		goto error;	
	}

	endpoints_count = config_setting_length(endpoints);
	if (endpoints_count != 0)
	{
		ep_infos = (FTSM_EP_INFO *)malloc(sizeof(FTSM_EP_INFO) * endpoints_count);
		if (ep_infos == NULL)
		{
			goto error;
		
		}

		for(i = 0 ; i < endpoints_count ; i++)
		{
			int	buff_len;
			const char *title, *count, *id, *name, *type, *sn, *value;
			int		offset;

			config_setting_t	*endpoint = config_setting_get_elem(endpoints, i);

			if (!( config_setting_lookup_string(endpoint, "title", &title)
					&& config_setting_lookup_string(endpoint, "count", &count)
					&& config_setting_lookup_string(endpoint, "id", &id)
					&& config_setting_lookup_string(endpoint, "name", &name)
					&& config_setting_lookup_string(endpoint, "type", &type)
					&& config_setting_lookup_string(endpoint, "sn", &sn)
					&& config_setting_lookup_string(endpoint, "value", &value)))
			{
				continue;
			}


			buff_len = strlen(title) 
						+ strlen(count) 
						+ strlen(id) 
						+ strlen(name) 
						+ strlen(type) 
						+ strlen(sn) 
						+ strlen(value) 
						+ 32;	

			ep_infos[ep_info_count].buff = (char *)malloc(buff_len);
			if (ep_infos[ep_info_count].buff == NULL)
			{
				continue;	
			}

			offset = 0;
			ep_infos[ep_info_count].title = strcpy(&ep_infos[ep_info_count].buff[offset], title);
			offset += strlen(title) + 1;
			ep_infos[ep_info_count].oid.count = strcpy(&ep_infos[ep_info_count].buff[offset], count);
			offset += strlen(count) + 1;
			ep_infos[ep_info_count].oid.id = strcpy(&ep_infos[ep_info_count].buff[offset], id);
			offset += strlen(id) + 1;
			ep_infos[ep_info_count].oid.name = strcpy(&ep_infos[ep_info_count].buff[offset], name);
			offset += strlen(name) + 1;
			ep_infos[ep_info_count].oid.type = strcpy(&ep_infos[ep_info_count].buff[offset], type);
			offset += strlen(type) + 1;
			ep_infos[ep_info_count].oid.sn = strcpy(&ep_infos[ep_info_count].buff[offset], sn);
			offset += strlen(sn) + 1;
			ep_infos[ep_info_count].oid.value = strcpy(&ep_infos[ep_info_count].buff[offset], value);

			
			ep_info_count++;
		}	
	}
error:
	config_destroy(&config);

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
	json_t			*root;
	json_error_t	error;

	

	return	0;
}
