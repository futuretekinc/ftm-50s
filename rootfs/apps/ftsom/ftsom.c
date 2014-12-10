#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "xshared.h"
#include "debug.h"
#include "ftsom.h"

static FT_SOM_OBJ* ft_som_obj_get(FT_OBJECT_ID id);

key_t		key = 0x1726;
FT_SOM_DATA	*pdata = NULL;

key_t ft_som_create(void)
{

	srand(time(NULL));

	while(1)
	{
		key = rand() % 0x10000;
		pdata = XSD_Create(key, sizeof(FT_SOM_DATA));
		if (pdata != 0)
		{
			strcpy(pdata->head.model, "FTM-50S");
			strcpy(pdata->head.devid, "FTM-50S01A0001");

			return	key;
		}
	}

	return	0;

}

key_t ft_som_init(char *key_file)
{
	key_t key = ft_som_key_load(key_file);

	if ((key == 0) || (pdata = XSD_Open(key, sizeof(FT_SOM_DATA))) == 0)
	{
		if ((key = ft_som_create()) == 0)
		{
			fprintf(stderr, "Can't create shared memory!\n");	
			return	0;
		}

		ft_som_key_save(key_file, key);
	}
	else
	{
		pdata = XSD_Open(key, sizeof(FT_SOM_DATA));
	}
	return	key;
}

int ft_som_destroy(key_t key)
{
	XSD_Close(key);	

	return	0;
}

int	ft_som_reset(void)
{
	if (pdata != 0)
	{
		memset(pdata->objs, 0, sizeof(pdata->objs));
		pdata->obj_count = 0;
		return	0;
	}

	return	-1;
}

int	ft_som_get_devid(char* buff, int len)
{
	if (pdata != 0)
	{
		strncpy(buff, pdata->head.devid, len-1);
		return	0;
	}

	return	-1;
}

int	ft_som_set_devid(char *devid)
{
	if ((pdata != NULL) && (strlen(devid) < sizeof(pdata->head.devid) - 1))
	{
		strcpy(pdata->head.devid, devid);	

		return	0;
	}

	return	-1;
}

int ft_som_get_model(char* buff, int len)
{
	if (pdata != 0)
	{
		strncpy(buff, pdata->head.model, len-1);
		return	0;
	}

	return	-1;
}

int	ft_som_set_model(char *model)
{
	if ((pdata != NULL) && (strlen(model) < sizeof(pdata->head.model) - 1))
	{
		strcpy(pdata->head.model, model);	

		return	0;
	}

	return	-1;
}

int		ft_som_obj_is_exist(FT_OBJECT_ID id)
{
	if (0 != pdata)
	{
		int	i;

		for(i = 0 ; i < FT_SOM_OBJ_MAX; i++)
		{
			if (id == pdata->objs[i].id)
			{
				return	1;
			}
		}
	}

	return	0;
}

FT_SOM_OBJ* ft_som_obj_get(FT_OBJECT_ID id)
{
	if (0 != pdata)
	{
		int	i;

		for(i = 0 ; i < FT_SOM_OBJ_MAX; i++)
		{
			if (id == pdata->objs[i].id)
			{
				return	&pdata->objs[i];
			}
		}
	}

	return	NULL;	
}

int		ft_som_obj_add(FT_OBJECT_ID id, char *type, char *name, char *sn)
{
	int	i;

	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if ((NULL != obj) || (0 == pdata))
	{
		return	-1;
	}


	for(i = 0 ; i < FT_SOM_OBJ_MAX; i++)
	{
		if (0 == pdata->objs[i].id)
		{
			memset(&pdata->objs[i], 0, sizeof(pdata->objs[i]));

			pdata->objs[i].id = id;
			if (type != NULL)
			{
				strncpy(pdata->objs[i].type, type, sizeof(pdata->objs[i].type) - 1);
			}

			if (name != NULL)
			{
				strncpy(pdata->objs[i].name, name, sizeof(pdata->objs[i].name) - 1);
			}

			if (sn != NULL)
			{
				strncpy(pdata->objs[i].sn, 	 sn, sizeof(pdata->objs[i].sn) - 1);
			}

			pdata->obj_count++;

			return	0;
		}
	}

	return	-1;	
}

int		ft_som_obj_count(unsigned long type)
{
	int	count = 0;

	if (0 != pdata)
	{
		int	i;

		for(i = 0 ; i < FT_SOM_OBJ_MAX; i++)
		{
			if (type == (pdata->objs[i].id & 0xFF000000))
			{
				count++;
			}
		}

	}

	return	count;
}

int		ft_som_obj_type_set(FT_OBJECT_ID id, char *type)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(obj->type, type, sizeof(obj->type));

	return	0;
}

int		ft_som_obj_type_get(FT_OBJECT_ID id, char *type, int type_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(type, obj->type, type_len);

	return	0;
}

int		ft_som_obj_name_set(FT_OBJECT_ID id, char *name)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(obj->name, name, sizeof(obj->name));

	return	0;
}

int		ft_som_obj_name_get(FT_OBJECT_ID id, char *name, int name_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(name, obj->name, name_len);
	return	0;
}

int		ft_som_obj_sn_set(FT_OBJECT_ID id, char *sn)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(obj->sn, sn, sizeof(obj->sn));

	return	0;
}

int		ft_som_obj_sn_get(FT_OBJECT_ID id, char *sn, int sn_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(sn, obj->sn, sn_len);
	return	0;
}

int		ft_som_obj_value_set(FT_OBJECT_ID id, char *value)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(obj->value, value, sizeof(obj->value));

	return	0;
}

int		ft_som_obj_value_get(FT_OBJECT_ID id, char *buff, int buff_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(buff, obj->value, buff_len -1);

	return	0;
}

int		ft_som_obj_time_get(FT_OBJECT_ID id, char *buff, int buff_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(buff, obj->time, buff_len -1);

	return	0;
}

int		ft_som_obj_last_value_get(FT_OBJECT_ID id, char *buff, int buff_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(buff, obj->last_value, buff_len -1);

	return	0;
}

int		ft_som_obj_last_time_get(FT_OBJECT_ID id, char *buff, int buff_len)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	strncpy(buff, obj->last_time, buff_len -1);

	return	0;
}

int		ft_som_obj_interval_set(FT_OBJECT_ID id, unsigned long interval)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	obj->interval = interval;

	return	0;

}

int		ft_som_obj_interval_get(FT_OBJECT_ID id, unsigned long *interval)
{
	FT_SOM_OBJ *obj = ft_som_obj_get(id);
	if (NULL == obj)
	{
		return	-1;
	}

	*interval = obj->interval;
	return	0;
}

int		ft_som_value_set(FT_OBJECT_ID id, char *value, char *time)
{
	if (0 != pdata)
	{
		int	i;

		for(i = 0 ; i < pdata->obj_count; i++)
		{
			if (pdata->objs[i].id == id)
			{
				strncpy(pdata->objs[i].value, value, FT_SOM_VALUE_LEN);
				if (NULL != time)
				{
					strncpy(pdata->objs[i].last_time, time, FT_SOM_TIME_LEN);
				}

				return	0;
			}
		}
	}

	return	-1;	
}

int		ft_som_value_get(FT_OBJECT_ID id, char *value, int value_len, char *time, int time_len)
{
	if (0 != pdata)
	{
		int	i;

		for(i = 0 ; i < pdata->obj_count; i++)
		{
			if (pdata->objs[i].id == id)
			{
				strncpy(value, pdata->objs[i].value, value_len -1);
				if (NULL != time)
				{
					strncpy(time, pdata->objs[i].last_time, time_len);
				}

				return	0;
			}
		}
	}

	return	-1;	
}

int	ft_som_print(void)
{
	if (NULL != pdata)
	{
		int	i;

		printf( "DEVID : %s\n", pdata->head.devid);	
		printf(" MODEL : %s\n", pdata->head.model);	
		printf("OBJECT : %d\n", pdata->obj_count);

		for(i = 0 ; i < pdata->obj_count ; i++)
		{
			printf("%08x %15s %16s %16s\n", 
				(unsigned int)pdata->objs[i].id, 
				pdata->objs[i].type, 
				pdata->objs[i].name, 
				pdata->objs[i].value);
		}
	}
	return	0;
}

key_t ft_som_key_load(char *path)
{
	char	full_name[256];

	if (NULL != path)
	{
		sprintf(full_name, "%s/skey.id", path);
	}
	else
	{
		sprintf(full_name, "/var/ftsom/skey.id");
	}

	FILE *fp = fopen(full_name, "r");
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

int ft_som_key_save(char *path, key_t key)
{
	FILE	*fp;
	struct stat st;
	char	full_name[256];

	if (NULL == path)
	{
		path = "/var/ftsom";
	}

	if (stat(path, &st) == -1)
	{
		if (mkdir(path, 0777) < 0)
		{
			fprintf(stderr, "Can't create %s\n", path);
			return	-1;	
		}
	}

	sprintf(full_name, "%s/skey.id", path);
	fp = fopen(full_name, "w");
	if (NULL == fp)
	{
		fprintf(stderr, "Can't create %s\n", full_name);
		return	-1;	
	}


	fprintf(fp, "%04x", key);
	fclose(fp);

	return	0;
}
