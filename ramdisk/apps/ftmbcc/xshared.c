#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "xshared.h"
#include "xftmbcc.h"

#define	QUEUE_SIZE	10

typedef	struct
{
	char	buf[128];
}	X_SHARED_DATA;

struct flock lock, unlock;

int XSD_OpenLock(int fd, int index)
{
	lock.l_start	=	index;
	lock.l_type		=	F_WRLCK;
	lock.l_len		=	1;
	lock.l_whence	=	SEEK_SET;
	return	fcntl(fd, F_SETLKW, &lock);
}

int	XSD_CloseLock(int fd, int index)
{
	unlock.l_start	=	index;
	unlock.l_type	=	F_UNLCK;
	unlock.l_len	=	1;
	unlock.l_whence	=	SEEK_SET;
	return	fcntl(fd, F_SETLK, &unlock);
}

void XSD_InitLock(void)
{
	lock.l_start	=	0;
	lock.l_type		=	F_WRLCK;
	lock.l_len		=	1;
	lock.l_whence	=	SEEK_SET;
}

void XSD_InitUnlock(void)
{
	unlock.l_start	=	0;
	unlock.l_type	=	F_UNLCK;
	unlock.l_len	=	1;
	unlock.l_whence	=	SEEK_SET;
}

void *XSD_Create(key_t key, int size);
void *XSD_Open(key_t key, int size);

static const XSD_INFO	lpSharedMemInfo[] =
{
	{
		.xKey		= XFTMBCC_SHARED_MEM_ID,
		.nSize		= sizeof(XFTMBCC_SHARED_MEM),
	}
};

static const int	nSharedMemInfoCount = sizeof(lpSharedMemInfo) / sizeof(XSD_INFO);
static void *lpSharedMemBase[sizeof(lpSharedMemInfo) / sizeof(XSD_INFO)] = 
{
	NULL,
};


int	XSD_Init(void)
{
	int	i;

	for(i = 0 ; i < nSharedMemInfoCount ; i++)
	{
		if (XSD_IsExist(i) == 0)
		{
			lpSharedMemBase[i] = XSD_Open(lpSharedMemInfo[i].xKey, lpSharedMemInfo[i].nSize);
		}
		else
		{
			lpSharedMemBase[i]= XSD_Create(lpSharedMemInfo[i].xKey, lpSharedMemInfo[i].nSize);
		}

		xTrace("Shared memory base: %08x\n", (unsigned int)lpSharedMemBase[i]);
	}

	return	0;
}

int	XSD_IsExist(int nID)
{
	int	nSharedMemID;

	if (nID < nSharedMemInfoCount)
	{
		nSharedMemID = shmget(lpSharedMemInfo[nID].xKey, lpSharedMemInfo[nID].nSize, 0666);
		if (nSharedMemID >= 0)
		{
			return	0;
		}
	}
	return	-1;	
}

void	*XSD_GetMemAddr(int nID)
{
	if (nID >= nSharedMemInfoCount)
	{
		return	0;	
	}

	xTrace("ID: %04d, MemBase: %08x\n", nID, (unsigned int)lpSharedMemBase[nID]);
	return	lpSharedMemBase[nID];
}

void *XSD_Create(key_t key, int size)
{
	int		nID;
	void	*pAddr;

	nID = shmget(key, size, 0666 | IPC_CREAT | IPC_EXCL);
	if (nID < 0)
	{
		xError("shared memory creation failed\n");
		return	NULL;	
	}

	xError("It's crated shared memory [%08x:%08x:%d]\n", nID, key, size);
	pAddr = shmat(nID, NULL, 0);
	if (pAddr == NULL)
	{
		xError("A system chooses a suitable (unused) address at which to attach the segment\n");
	}
	else
	{ 
		memset(pAddr, 0, size);
	}

	return	pAddr;
}

void *XSD_Open(key_t key, int size)
{
	int		nID;

	nID = shmget(key, size, 0666);
	if (nID < 0)
	{
		xError("It's not found shared memory [%08x:%d]\n", key, size);
		return	NULL;	
	}

	xError("It's found shared memory [%08x:%08x:%d]\n", nID, key, size);
	return	shmat(nID, NULL, 0);
}


