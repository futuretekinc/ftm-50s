#ifndef	__FTDM_H__
#define	__FTDM_H__

#define	FTDM_ID_MAX	64

typedef	char	FTDM_ID[FTDM_ID_MAX];

int	ftdm_open(char *filename);
int ftdm_close(void);

int	ftdm_sensor_is_exist(FTDM_ID id, int *exist);
int	ftdm_sensor_count(int *count);
int	ftdm_sensor_add(FTDM_ID id, char *type, char *name, char *sn);
int	ftdm_sensor_list_get(int start, int max_count, FTDM_ID *id_list, int *count);
int	ftdm_sensor_get(FTDM_ID id, char *type, int type_len, char *name, int name_len, char *sn, int sn_len);
int	ftdm_sensor_set(FTDM_ID id, char *name, char *sn);

int	ftdm_data_add(FTDM_ID id, char *value, char *time);
int	ftdm_test(void);

#endif

