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
			strcpy(pdata->head.product_id, "FTM-50S01A0001");

			return	key;
		}
	}

	return	0;

}

key_t ft_som_init(key_t key)
{
	if ((key == 0) || (pdata = XSD_Open(key, sizeof(FT_SOM_DATA))) == 0)
	{
		if ((key = ft_som_create()) == 0)
		{
			fprintf(stderr, "Can't create shared memory!\n");	
			return	0;
		}
	}


	return	key;
}

int	ft_som_print(void)
{
	if (NULL != pdata)
	{
	
		printf("DEVID : %s\n", pdata->head.model);	
	}
	return	0;
}
