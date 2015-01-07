#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <libconfig.h>
#include "ftdm.h"
#include "debug.h"


static int ftdm_sensor_table_create(void);
static int ftdm_data_table_create(void);
static int ftdm_table_is_exist(char *name, int *exist);

static char 	*ftdm_default_config_file = "/etc/ftdm.conf";
static sqlite3	*ftdm_db = NULL;

int	ftdm_open(char *filename)
{
	config_t	config;
	int			ret;
	int			exist;	
	char		*config_filename;
	const char	*db_filename;

	if (filename != NULL)
	{
		config_filename = filename;
	}
	else
	{
		config_filename = ftdm_default_config_file;	
	}

	config_init(&config);
	if (CONFIG_TRUE != config_read_file(&config, config_filename))
	{
		TRACE("Configuration file loading failed\n");
		return	-1;	
	}

	if (CONFIG_TRUE != config_lookup_string(&config, "database",&db_filename))
	{
		TRACE("Can't find database information in Configuration file\n");
		return	-1;	
	}

	ret = sqlite3_open(db_filename, &ftdm_db);	
	if( ret )
	{
		ERROR("Can't open database: %s\n", sqlite3_errmsg(ftdm_db));
		return	-1;
	}
	TRACE("Opened database successfully\n");

	if (ftdm_table_is_exist("sensor", &exist) != 0)
	{
		return	-1;	
	}

	if ((exist == 0) && (ftdm_sensor_table_create() != 0))
	{
		return	-1;	
	}

	if (ftdm_table_is_exist("data", &exist) != 0)
	{
		return	-1;	
	}

	if ((exist == 0) && (ftdm_data_table_create() != 0))
	{
		return	-1;	
	}

	return	0;
}

int ftdm_close(void)
{
	if (ftdm_db == NULL)
	{
		return	-1;
	}

	sqlite3_close(ftdm_db);

	return	0;
}

/**************************************************************************/
static int ftdm_sensor_table_create(void)
{
	int		ret;
	char	*zErrMsg = NULL;
	char	*sql = 	"CREATE TABLE sensor ("
					"	ID		CHAR(64) 	PRIMARY KEY NOT NULL,"\
					"	TYPE	TEXT	NOT NULL,"\
					"	NAME	TEXT,"\
					"	SN		TEXT)";

	ret = sqlite3_exec(ftdm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);

		return	-1;
	}

	TRACE("Sensor table s creation done successfully\n");	

	return	0;
}

/*************************************************************************/
static int cb_sensor_count(void *data, int argc, char **argv, char **azColName)
{
	if (argc != 0)
	{
		*(int *)data = atoi(argv[0]);
	}
	else
	{
		*(int *)data = 0;	
	}

	return	0;
}

int	ftdm_sensor_count(int *count)
{
	int		ret;
	char	*zErrMsg = NULL;
	char	*sql = "SELECT COUNT(*) FROM sensor";

	
	ret = sqlite3_exec(ftdm_db, sql, cb_sensor_count, count, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}

	return	0;
}

/*************************************************************************/
typedef struct 
{
	int		ret;
	int		count;
	int		max_count;
	FTDM_ID *list;
}	CB_SENSOR_LIST_GET_PARAMS;

static int cb_sensor_list_get(void *data, int argc, char **argv, char **azColName)
{
	CB_SENSOR_LIST_GET_PARAMS *params = (CB_SENSOR_LIST_GET_PARAMS *)data;

	if (argc == 2)
	{
		if (params->count < params->max_count)
		{
			strncpy(params->list[params->count++], argv[1], sizeof(FTDM_ID));
		}
	}

	params->ret = 0;
	return	0;
}

int	ftdm_sensor_list_get(int start, int max_count, FTDM_ID *id_list, int *count)
{
	int		ret;
	char	*zErrMsg = NULL;
	char	sql[1024];
	CB_SENSOR_LIST_GET_PARAMS params = { .ret = 0, .count = 0, .max_count = max_count, .list = id_list};
	
	sprintf(sql,"SELECT (SELECT COUNT(*) FROM sensor AS t2 WHERE t2.ID <= t1.ID) AS row_num, ID"\
				"	FROM sensor AS t1 WHERE row_num >= %d ", start);

	ret = sqlite3_exec(ftdm_db, sql, cb_sensor_list_get, &params, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}

	*count = params.count;
	return	0;
}

/*************************************************************************/
int	ftdm_sensor_add(FTDM_ID id, char *type, char *name, char *sn)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	
	sprintf(sql, 
			"INSERT INTO sensor (ID, TYPE, NAME, SN) "\
			"	VALUES ( '%s', '%s', '%s', '%s')", id, type, name, sn);

	ret = sqlite3_exec(ftdm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("The new sensor(%s) has been added successfully\n", id);	
	}
	return	0;
}

/*************************************************************************/
typedef struct 
{
	int		ret;
	char	*type;
	int		type_len;
	char	*name;
	int		name_len;
	char	*sn;
	int		sn_len;
	FTDM_ID *list;
}	CB_SENSOR_GET_PARAMS;

static int cb_sensor_get(void *data, int argc, char **argv, char **azColName)
{
	CB_SENSOR_GET_PARAMS *params = (CB_SENSOR_GET_PARAMS *)data;

	if (argc == 4)
	{
		strncpy(params->type, 	argv[1], params->type_len);
		strncpy(params->name, 	argv[2], params->name_len);
		strncpy(params->sn, 	argv[3], params->sn_len);
	}

	params->ret = 0;
	return	0;
}

int	ftdm_sensor_get(FTDM_ID id, char *type, int type_len, char *name, int name_len, char *sn, int sn_len)
{
	CB_SENSOR_GET_PARAMS params;
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	params.type = type;
	params.type_len = type_len;
	params.name = name;
	params.name_len = name_len;
	params.sn = sn;
	params.sn_len = sn_len;

	sprintf(sql, "SELECT ID, TYPE, NAME, SN FROM sensor WHERE ID = '%s'", id);
	ret = sqlite3_exec(ftdm_db, sql, cb_sensor_get, &params, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}

	return	0;
}
/*************************************************************************/
typedef	struct 
{
	int		ret;
	FTDM_ID	id;
}	CB_SENSOR_IS_EXIST_PARAMS;

static int cb_sensor_is_exist(void *data, int argc, char **argv, char **azColName)
{
	int	i;
	CB_SENSOR_IS_EXIST_PARAMS *params = (CB_SENSOR_IS_EXIST_PARAMS *)data;

	if (argc != 0)
	{
		for(i = 0 ; i < argc ; i++)
		{
			if (strcmp(params->id, argv[i]) == 0)
			{
				params->ret = 1;
				break;	
			}
		}
			
	}

	return	0;
}

int	ftdm_sensor_is_exist(FTDM_ID id, int *exist)
{
	CB_SENSOR_IS_EXIST_PARAMS params;
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	memcpy(params.id, id, sizeof(FTDM_ID));
	params.ret = 0;

	sprintf(sql, "SELECT COUNT(ID) FROM sensor WHERE ID = '%s'", id);
	ret = sqlite3_exec(ftdm_db, sql, cb_sensor_is_exist, &params, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}

	*exist = params.ret;

	return	0;
}

/**************************************************************************/
static int ftdm_data_table_create(void)
{
	int		ret;
	char	*zErrMsg = NULL;
	char	*sql = 	"CREATE TABLE data ("
					"	ID		TEXT 	NOT NULL,"\
					"	VALUE	TEXT,"\
					"	TIME	DATE);";

	ret = sqlite3_exec(ftdm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);

		return	-1;
	}

	TRACE("Data table creation done successfully\n");	

	return	0;
}

/*************************************************************************/
int	ftdm_data_add(FTDM_ID id, char *value, char *date)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;
	
	sprintf(sql, "INSERT INTO data (ID, VALUE, TIME) VALUES ( '%s', '%s', '%s')", id, value, date);

	ret = sqlite3_exec(ftdm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("The new data has been added successfully\n");	
	}
	return	0;
}

/**************************************************************************/
typedef	struct 
{
	int		ret;
	char 	*name;
}	CB_TABLE_IS_EXIST_PARAMS;

static int cb_table_is_exist(void *data, int argc, char **argv, char **azColName)
{
	CB_TABLE_IS_EXIST_PARAMS *params = (CB_TABLE_IS_EXIST_PARAMS *)data;
	int	i;
	
	for(i = 0 ; i < argc ; i++)
	{
		if (strcmp(params->name, argv[i]) == 0)
		{
			params->ret = 1;
			break;
		}
	}

	return	0;
}

static int	ftdm_table_is_exist(char *name, int *exist)
{
	int		ret;
	CB_TABLE_IS_EXIST_PARAMS	params = { .ret = 0, .name = name};
	char	*sql = "select name from sqlite_master where type='table' order by name";
	char	*zErrMsg = NULL;

	ret = sqlite3_exec(ftdm_db, sql, cb_table_is_exist, &params, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);

		return	-1;
	}

	TRACE("Sensor list get completed successfully\n");	

	*exist = params.ret;

	return	0;
}

