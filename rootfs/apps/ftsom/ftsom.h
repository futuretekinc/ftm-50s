#ifndef	__FTSOM_H__
#define __FTSOM_H__

typedef	struct _ft_som_head
{
	char	model[256];
	char	product_id[256];
}	FT_SOM_HEAD;

typedef	struct _ft_som_obj
{
	int		enabled;
	char	id[32];
	char	type[32];
	char	value[32];
	char	lastValue[32];
	char	lastTime[64];
}	FT_SOM_OBJ;

typedef	struct _ft_som_data
{
	FT_SOM_HEAD		head;
	int				obj_count;
	FT_SOM_OBJ		objs[64];
}	FT_SOM_DATA;

int		ft_som_create(void);
key_t	ft_som_init(key_t key);
int		ft_som_print(void);

#endif

