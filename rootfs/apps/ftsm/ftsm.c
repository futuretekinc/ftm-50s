#include <sqlite3.h>
#include <libconfig.h>
#include "debug.h"


static char 	*ftsm_default_config_file = "/etc/ftsm.conf";
static sqlite3	*ftsm_db = NULL;

int	ftsm_open(char *filename)
{
	config_t	config;
	int			ret;
	char		*config_filename;
	const char	*db_filename;

	if (filename != NULL)
	{
		config_filename = filename;
	}
	else
	{
		config_filename = ftsm_default_config_file;	
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

	ret = sqlite3_open(db_filename, &ftsm_db);	
	if( ret )
	{
		ERROR("Can't open database: %s\n", sqlite3_errmsg(ftsm_db));
	}
	else
	{
		TRACE("Opened database successfully\n");
	}

	return	0;
}

int	ftsm_sensors_info_db_create(char *name)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	
	sprintf(sql, "CREATE TABLE %s ("\
			"	ID		TEXT 	PRIMARY KEY NOT NULL,"\
			"	TYPE	TEXT	NOT NULL,"\
			"	NAME	TEXT,"\
			"	SN		TEXT);", name);

	ret = sqlite3_exec(ftsm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("Sensors information DB creation failed.\n");	
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("Sensors information DB creation done successfully\n");	
	}
	return	0;
}

static int cb_sensors_list(void *data, int argc, char **argv, char **azColName)
{
	int	i;

	for(i = 0 ; i < argc ; i++)
	{
		if (argv[i][0] == 'P')
		{
			printf("%s\n", argv[i]);	
		}
	}

	return	0;
}

int	ftsm_sensors_list(void)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	sprintf(sql, "select * from sensors order by ID ");

	ret = sqlite3_exec(ftsm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("Sensor list get completed successfully\n");	
	}
	return	0;
}

int	ftsm_sensors_add(char *id, char *type, char *name, char *sn)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	
	sprintf(sql, 
			"INSERT INTO sensors (ID, TYPE, NAME, SN) "\
			"	values ( %s, %s, %s, %s);", id, type, name, sn);

	ret = sqlite3_exec(ftsm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("Add new sensor has failed.\n");	
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("The new sensor(%s) has been added successfully\n", id);	
	}
	return	0;
}

int	ftsm_sensor_value_db_create(char *id)
{
	int		ret;
	char	sql[1024];
	char	*zErrMsg = NULL;

	sprintf(sql, "CREATE TABLE P%s ("\
			"	ID		INT 	PRIMARY KEY NOT NULL,"\
			"	TYPE	TEXT	NOT NULL,"\
			"	NAME	TEXT,"\
			"	SN		TEXT,"\
			"	VALUE	TEXT,"\
			"	TIME	DATE);", id);

	ret = sqlite3_exec(ftsm_db, sql, NULL, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("Sensor(%s) Value DB creation done successfully\n", id);	
	}
	return	0;
}

static int cb_sensor_value_db_list(void *data, int argc, char **argv, char **azColName)
{
	int	i;

	for(i = 0 ; i < argc ; i++)
	{
		printf("%s\n", argv[i]);	
	}

	return	0;
}

int	ftsm_sensor_value_db_list(void)
{
	int		ret;
	char	*sql = "select name from sqlite_master where type='table' order by name";
	char	*zErrMsg = NULL;


	ret = sqlite3_exec(ftsm_db, sql, cb_sensor_value_db_list, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("Sensor value DB list read done successfully\n");	
	}
	return	0;
}

static int callback(void *data, int argc, char **argv, char **azColName)
{
	int	i;

	for(i = 0 ; i < argc ; i++)
	{
		printf("%s\n", argv[i]);	
	}

	return	0;
}

int	ftsm_test(void)
{
	int		ret;
	char	*sql;
	char	*zErrMsg = 0;

	TRACE("%s\n", __func__);
	sql = ".database";

	ret = sqlite3_exec(ftsm_db, sql, callback, 0, &zErrMsg);	
	if (ret != SQLITE_OK)
	{
		ERROR("SQL error : %s\n", zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		TRACE("Operation done successfully\n");	
	}
	return	0;
}

int ftsm_close(void)
{
	sqlite3_close(ftsm_db);
}
