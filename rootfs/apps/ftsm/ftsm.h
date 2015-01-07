#ifndef __FSM_H__
#define __FSM_H__

typedef struct _FTSM_EP_INFO
{
	unsigned int	code;
	char			*title;
	struct {
		char		*count;
		char		*id;
		char		*type;
		char		*name;
		char		*sn;
		char		*state;
		char		*value;
		char		*last_value;
		char		*last_time;
		char		*interval;
	} oid;

	char			*buff;
}	FTSM_EP_INFO;

int	ftsm_init(char *filename);
int	ftsm_finish(void);


#endif
