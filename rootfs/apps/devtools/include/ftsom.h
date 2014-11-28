#ifndef	__FTSOM_H__
#define __FTSOM_H__

#define	FT_SOM_OBJ_MAX		64

#define	FT_SOM_TYPE_LEN		32
#define	FT_SOM_NAME_LEN		32	
#define	FT_SOM_VALUE_LEN	32
#define	FT_SOM_TIME_LEN		32

typedef unsigned long		FT_OBJECT_ID;

typedef	struct _ft_som_head
{
	char	model[256];
	char	devid[256];
}	FT_SOM_HEAD;

typedef	struct _ft_som_obj
{
	int				enabled;
	FT_OBJECT_ID	id;
	char			type[FT_SOM_TYPE_LEN + 1];
	char			name[FT_SOM_NAME_LEN + 1];
	char			value[FT_SOM_VALUE_LEN + 1];
	char			last_value[FT_SOM_VALUE_LEN + 1];
	char			last_time[FT_SOM_TIME_LEN + 1];
}	FT_SOM_OBJ;

typedef	struct _ft_som_data
{
	FT_SOM_HEAD		head;
	int				obj_count;
	FT_SOM_OBJ		objs[FT_SOM_OBJ_MAX];
}	FT_SOM_DATA;

int		ft_som_create(void);
key_t 	ft_som_init(char *key_file);
int		ft_som_get_devid(char *buff, int len);
int		ft_som_set_devid(char *devid);
int		ft_som_get_model(char *buff, int len);
int		ft_som_set_model(char *model);
int		ft_som_obj_is_exist(FT_OBJECT_ID id);
int		ft_som_obj_add(FT_OBJECT_ID id);
int		ft_som_obj_count(unsigned long type);
int		ft_som_obj_type_set(FT_OBJECT_ID id, char *type);
int		ft_som_obj_type_get(FT_OBJECT_ID id, char *type, int type_len);
int		ft_som_obj_name_set(FT_OBJECT_ID id, char *name);
int		ft_som_obj_name_get(FT_OBJECT_ID id, char *name, int name_len);
int		ft_som_value_get(FT_OBJECT_ID id, char *buff, int buff_len, char *time, int time_len);
int		ft_som_value_set(FT_OBJECT_ID id, char *value, char *time);
int		ft_som_print(void);

key_t 	ft_som_key_load(char *path);
int 	ft_som_key_save(char *path, key_t key);
#endif

