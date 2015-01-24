/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * System Recovery support
 */
#include <linux/ctype.h>
#include <common.h>
#include <command.h>
#include <post.h>
#include <nand.h>

#define	INVALID_UINT	0xFFFFFFFF
#define	SYS_INFO_MAGIC	0x1F3D5B79
#define	SYSINFO_MAX		2
#define	KERNEL_MAX		2
#define	ROOTFS_MAX		2
typedef struct
{	
	uint			magic;
	uint			index;
	uint			primary_kernel;
	uint			primary_fs;
	image_header_t	rootfs[ROOTFS_MAX];
	uint			crc32;
}	sys_info_t;
	

#define	PRINTF(verbose, ...) {if (verbose) printf(__VA_ARGS__);}
#define	PUTS(verbose, ...) {if (verbose) puts(__VA_ARGS__);}

extern int  _nand_erase(ulong offset, size_t size, int clean, int quite);
extern int  _nand_read(ulong offset, size_t size, void *buff);
extern int  _nand_write(ulong offset, size_t size, void *buff);

static int	kernel_check_and_recovery(void);
static int	rootfs_check_and_recovery(void);
static int	check_kernel(void *loc, int verbose);
static int	check_rootfs(int index, int vervose);
static int 	recovery_rootfs(sys_info_t	*sysinfo);
static int	copy_image(void *dest, void *src, unsigned long len);
static uint	getenv_uint(char *name);
static uint	string_to_version(char *s);
static uint	image_get_version(const image_header_t *hdr);
static int	set_kernel_loc(uint loc);
static int	save_rootfs(cmd_tbl_t * cmdtp, int flag, int index, uint addr, uint len);
static int	save_sys_info(sys_info_t *info);
static int	load_sys_info(sys_info_t *info);
static void mem_dump(void *addr, int len);


extern int nand_curr_device;
extern int board_power_hold(int on);

int do_recovery(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	board_power_hold(1);	
	kernel_check_and_recovery();
	rootfs_check_and_recovery();
	board_power_hold(0);	
}

int do_savefs(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int	ret;

	if (argc < 3)
	{
		goto error;
	}

	if (strcmp(argv[1], "rootfs") == 0)
	{
		uint addr;
		uint index;
		uint len  = getenv_uint("rootfs_size");
		if (len == 0)
		{
			puts ("RootFS size is invalid!\n");
			goto error;	
		}

		index = simple_strtoul(argv[2], NULL, 16);
		if (index >= ROOTFS_MAX)
		{
			printf("RootFS index is out of range(0 .. %d)!\n", ROOTFS_MAX);
			goto error;
		}

		if (argc == 4)
		{
			addr = simple_strtoul(argv[3], NULL, 16);	
		}
		else
		{
			addr = getenv_uint("fileaddr");	
		}

		if (addr == 0)
		{
			puts("RootFS image location is invalid.\n");
			goto error;	
		}

		board_power_hold(1);	
		ret = save_rootfs(cmdtp, flag, index, addr, len);
		board_power_hold(0);	
		if (ret  != 0)
		{
			goto error;	
		}
	}

	return	0;	
error:
	return	1;
}

int	kernel_check_and_recovery(void)
{
	sys_info_t	sysinfo;
	uint	kernel_loc;
	uint	kernel_size;
	uint	auto_recovery = 0;
	int		i;

	if (load_sys_info(&sysinfo) != 0)
	{
		puts("ERROR : SysInfo is invalid!\n");
		goto error;	
	}

	for(i = 0 ; i < KERNEL_MAX ; i++)
	{
		char	name[64];

		sprintf(name, "kernel_%d_loc", (sysinfo.primary_kernel + i) % KERNEL_MAX);

		kernel_loc= getenv_uint(name);
		if (check_kernel((void *)kernel_loc, 1) == 0)
		{
			break;
		}
	}

	if (i == KERNEL_MAX)
	{
		goto error;	
	}

	auto_recovery=getenv_uint("auto_recovery");
	if ((i != 0) && (auto_recovery == 1))
	{
		char	name[64];
		uint	kernel_p_loc;

		sprintf(name, "kernel_%d_loc", sysinfo.primary_kernel);
		kernel_p_loc = getenv_uint(name);
		kernel_size	= getenv_uint("kernel_size");
		if ((kernel_p_loc != INVALID_UINT) && (kernel_size != INVALID_UINT))
		{
			copy_image((void *)kernel_p_loc, (void *)kernel_loc, kernel_size);
			kernel_loc = kernel_p_loc;
		}
	}
	else
	{
		goto error;
	}

	set_kernel_loc(kernel_loc);

	return	0;
error:
	return	1;

}

int	rootfs_check_and_recovery(void)
{
	sys_info_t	sysinfo;
	int			i;


	if (load_sys_info(&sysinfo) != 0)
	{
		puts("ERROR : SysInfo is invalid!\n");
		goto error;	
	}

	for(i = 0 ; i < ROOTFS_MAX ; i++)
	{
		
		int index = (sysinfo.primary_fs + i) % ROOTFS_MAX;	
		if (i == 0)
		{
			printf("Check primary rootfs : ");
		}
		else
		{
			printf("Check secondary rootfs : ");
		}
		if (check_rootfs(index, 0) == 0)
		{
			char		buff[64];

			printf("[SUCCESS]\n");

			sprintf(buff, "rootfs_%d_mtd", index);
			setenv ("rootfs_mtd", getenv(buff));
			break;
		}
		printf("[FAILED]\n");
	}

	if (i == ROOTFS_MAX)
	{
		puts("ERROR : All rootfs is incorrect.\n");
		goto error;	
	}

	if (i != 0)
	{
		puts("Primary rootfs is invalid!\n");	

		uint	auto_recovery = 0;
		auto_recovery=getenv_uint("auto_recovery");
		if (auto_recovery == 1)
		{
			char		buff[64];
			puts("Primary rootfs is configured auto recovery mode!\n");	
			puts("Recovery start!.\n");
			if (recovery_rootfs(&sysinfo) == 0)
			{
				puts("Primary rootfs recovery is completed !.\n");
				sprintf(buff, "rootfs_%d_mtd", sysinfo.primary_fs);
				setenv ("rootfs_mtd", getenv(buff));
			
			}
			else
			{
				puts("Primary rootfs recovery is failed!.\n");
			}
		}
	}

	return	0;
error:
	return	1;

}

uint	getenv_uint(char *name)
{
	char	*val;

	val = getenv(name);
	if (val == 0)
	{
		goto error;
	}

	return	simple_strtoul(val, NULL, 16);
error:

	return	INVALID_UINT;
}

int check_kernel(void *loc, int printout)
{
	if (genimg_get_format (loc) != IMAGE_FORMAT_LEGACY)
	{
		goto error;
	}
		
	if (!image_check_hcrc (loc))
	{
		goto error;
	}

	if (printout)
	{
		printf ("Legacy Image at %08X:\n", (uint)loc);
		image_print_contents (loc);

		puts ("   Verifying Checksum ... ");
	}
	if (!image_check_dcrc (loc)) 
	{
		if (printout)
		{
			puts ("Bad Data CRC\n");
		}
		goto error;
	} 

	if (printout)
	{
		puts ("OK\n");
	}

	return 0;

error:
	if (printout)
	{
		puts ("ERROR: Invalid kernel image!\n");
	}

	return	1;
}

int check_rootfs(int index, int verbose)
{
	sys_info_t	sysinfo;
	void		*buff= (void*)CONFIG_SYS_LOAD_ADDR;	
	ulong		rootfs_loc;
	ulong		rootfs_size;
	char		name[64];
	nand_info_t *nand;

	if (load_sys_info(&sysinfo) != 0)
	{
		PUTS(verbose, "System information loading failed!\n");
		goto error;
	}

	sprintf(name, "rootfs_%d_loc", index);
	rootfs_loc= getenv_uint(name);
	if(rootfs_loc == INVALID_UINT)
	{
		PRINTF(verbose,"ERROR: can't find location[name = 0x%08x]\n", rootfs_loc);	
		goto error;
	}

	memcpy(buff, &sysinfo.rootfs[index], sizeof(image_header_t));
	if (genimg_get_format (buff) != IMAGE_FORMAT_LEGACY)
	{
		PUTS(verbose,"ERROR: Invalid image format!\n");
		goto error;
	}
		
	if (!image_check_hcrc (buff))
	{
		PUTS(verbose,"ERROR: Invalid head crc!\n");
		goto error;
	}

	
	/* the following commands operate on the current device */
	if (nand_curr_device < 0 || nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[nand_curr_device].name) 
	{  
		PUTS(verbose,"ERROR: no devices available\n");
		goto error;	
	}

	rootfs_size = ((image_header_t *)buff)->ih_size;

	nand = &nand_info[nand_curr_device];
	if (nand_read_skip_bad(nand, rootfs_loc, (size_t*)&rootfs_size, &((u_char *)buff)[sizeof(image_header_t)]) != 0)
	{
		PUTS(verbose,"ERROR: nand read failed\n");
		goto error;	
	}

	if (!image_check_dcrc (buff)) 
	{
		PUTS(verbose,"ERROR: Invalid rootfs image\n");
		goto error;
	} 

	PRINTF(verbose, "Root FS at %08X:\n", (uint)rootfs_loc);
	image_print_contents (buff);

	return 0;

error:
	return	1;
}

int recovery_rootfs(sys_info_t	*sysinfo)
{
	void*	buff;
	ulong 	primary_loc;
	ulong	secondary_loc;
	uint  	rootfs_size;
	nand_info_t *nandinfo;
	nand_erase_options_t opts;

	if (sysinfo->primary_fs == 0)
	{
		primary_loc = getenv_uint("rootfs_0_loc");
		secondary_loc = getenv_uint("rootfs_1_loc");
	}
	else
	{
		primary_loc = getenv_uint("rootfs_1_loc");
		secondary_loc = getenv_uint("rootfs_0_loc");
	}
	rootfs_size= getenv_uint("rootfs_size");


	buff = (void *)0x800000;
	nandinfo= &nand_info[nand_curr_device];


	opts.length = rootfs_size;
	opts.offset = primary_loc;
	opts.quiet	= 0;
	opts.jffs2  = 0;
	opts.scrub  = 0;

	if ((nand_read_skip_bad(nandinfo, secondary_loc, &rootfs_size, buff) != 0) ||
		(nand_erase_opts(nandinfo, &opts) != 0) ||
		(nand_write_skip_bad(nandinfo, primary_loc, &rootfs_size, buff) != 0))
	{
		return	1;	
	}
		
	return	0;
}

int copy_image(void *dest, void *src, unsigned long len)
{
	unsigned long dest_start, dest_end;
	void *buff;
	
	buff = malloc(len);
	if (buff == NULL)
	{
		return	1;	
	}


	memcpy(buff, src, len);

	dest_start = (uint)dest;
	dest_end = ((uint)(dest + len + 0xFFFF) & 0xFFFF0000) - 1;
	if (flash_sect_protect (0, dest_start, dest_end)) 
	{
		printf("dest_start = %08lx, dest_end = %08lx\n", dest_start, dest_end);
		puts("ERROR: can't unprotected!\n");
		goto error;
	}

	
	if (flash_sect_erase(dest_start, dest_end))
	{
		puts("ERROR: Flash erase failed!\n");
		goto error;	
	}


	if (flash_write(buff, dest_start, len))
	{
		goto error;	
	}

	if (flash_sect_protect (1, dest_start, dest_end)) 
	{
		puts("ERROR: can't protected!\n");
		goto error;
	}

	free(buff);
	return	0;

error:
	free(buff);
	return	1;
}

uint string_to_version(char *s)
{
	uint	version;
	char *e;
	int i;

	if (s == NULL)
		return(0);

	for (version=0, i=0; i<4; ++i) {
		uint val = s ? simple_strtoul(s, &e, 10) : 0;
		version <<= 8;
		version |= (val & 0xFF);
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}

	return version;
}

static uint	image_get_version(const image_header_t *hdr)
{
	char *ptr = strchr((char *)hdr->ih_name, '-');
	char buff[16];
	int	n;

	if (ptr == NULL)
	{
		return	0;	
	}

	while((*ptr != 0) && !isdigit(*ptr))
	{
		ptr++;
	}

	if (*ptr == 0)
	{
		return	0;	
	}

	memset(buff, 0, sizeof(buff));
	n = 0;
	while((*ptr != 0) && (isdigit(*ptr) || (*ptr == '.')))
	{
		buff[n++] = *ptr;
		ptr++;
	}

	return string_to_version(buff);
}

static int set_kernel_loc(uint loc)
{
	char	buff[64];

	sprintf(buff, "0x%08x", (unsigned int)loc);
	setenv("kernel_loc", buff); 

	return	0;
}

int	load_sys_info(sys_info_t *info)
{
	sys_info_t *lastinfo = 0;
	int			i;

	for(i = 0 ; i < SYSINFO_MAX ; i++)
	{
		sys_info_t *base = (sys_info_t *)(CONFIG_SYS_INFO_BASE + 0x10000 * i);
		if (base->crc32 == crc32(0, (const uchar *)base, sizeof(sys_info_t) - sizeof(uint)))
		{
			if ((lastinfo == 0) || (lastinfo->index < base->index))
			{
				lastinfo = base;	
			}
		}
	}

	if (lastinfo == 0)
	{
		goto error;
	}

	memcpy(info, lastinfo, sizeof(sys_info_t));

	return	0;

error:
	return	1;
}

int	save_sys_info(sys_info_t *info)
{
	sys_info_t *base = 0;

	info->index++;
	info->crc32 = crc32(0, (const unsigned char *)info, sizeof(sys_info_t) - sizeof(uint));

	base = (sys_info_t *)(CONFIG_SYS_INFO_BASE + 0x10000 * (info->index % SYSINFO_MAX));


	copy_image(base, info, sizeof(sys_info_t));

	return	0;
}

int	save_rootfs(cmd_tbl_t * cmdtp, int flag, int index, uint addr, uint len)
{
	sys_info_t	sys_info;
	char		buf[64];
	uint		offset, size;
	int			i;

	if (load_sys_info(&sys_info) != 0)
	{
		memset(&sys_info, 0, sizeof(sys_info));
		sys_info.magic = SYS_INFO_MAGIC;
	}

	if (index >= ROOTFS_MAX)
	{
		goto error;		
	}

	sprintf(buf, "rootfs_%d_loc", index);
	offset = getenv_uint(buf);	
	size = getenv_uint("rootfs_size");	

	if (offset == INVALID_UINT || size == INVALID_UINT)
	{
		goto error;	
	}
	
	_nand_erase(offset, size, 1, 0);
	_nand_write(offset, size, (void *)(addr + sizeof(image_header_t)));

	memcpy(&sys_info.rootfs[index], (void *)addr, sizeof(image_header_t));
	sys_info.primary_fs = index;


	save_sys_info(&sys_info);

	return	0;

error:
	return	1;
}

void mem_dump(void *addr, int len)
{
	int	i;

	printf("memdump(0x%08x, 0x%08x)\n", addr, len);
	for(i = 0 ; i < len ; i++)
	{
		printf("%02x ", ((uchar *)addr)[i]);
		if ((i+1) % 8 == 0)
		{
			printf("\n");	
		}
	}

}

/***************************************************/

U_BOOT_CMD(
	recovery,	CONFIG_SYS_MAXARGS,	0,	do_recovery,
	"perform board recovery",
	     "    - system recovery\n"
);


U_BOOT_CMD(
	savefs,	CONFIG_SYS_MAXARGS,	0,	do_savefs,
	"save Root FS",
	     "    - save Root File System images\n"
);

