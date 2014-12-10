#ifndef	__FTSM_H__
#define	__FTSM_H__

int	ftsm_open(char *filename);
int	ftsm_sensors_info_db_create(char *name);
int	ftsm_sensors_add(char *id, char *type, char *name, char *sn);
int	ftsm_sensor_value_db_create(char *id);
int	ftsm_sensor_value_db_list(void);
int	ftsm_test(void);
int ftsm_close(void);

#endif

