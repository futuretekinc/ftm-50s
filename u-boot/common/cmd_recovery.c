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

#define	INVALID_LOC	0xFFFFFFFF
#define	SYS_INFO_MAGIC	0x1F3D5B79
typedef struct
{	
	uint			magic;
	uint			index;
	image_header_t	rootfs_p;
	image_header_t	rootfs_s;
	uint			crc32;
}	sys_info_t;
	

extern int  _nand_erase(ulong offset, size_t size, int clean, int quite);
extern int  _nand_read(ulong offset, size_t size, void *buff);
extern int  _nand_write(ulong offset, size_t size, void *buff);

static int	kernel_check_and_recovery(void);
static int	rootfs_check_and_recovery(void);
static int	check_kernel(void *loc, int printout);
static int	check_rootfs(int primary, int printout);
static int	copy_image(void *dest, void *src, unsigned long len);
static int  copy_rootfs(int backup);
static uint	getenv_uint(char *name);
static uint	string_to_version(char *s);
static uint	image_get_version(const image_header_t *hdr);
static int	set_kernel_loc(uint loc);
static int	save_rootfs(cmd_tbl_t * cmdtp, int flag, uint addr, uint len, int primary);
static int	save_sys_info(sys_info_t *info);
static int	load_sys_info(sys_info_t *info);

extern int nand_curr_device;

int do_recovery(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	kernel_check_and_recovery();
	rootfs_check_and_recovery();
}

int do_savefs(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 3)
	{
		goto error;
	}

	if (strcmp(argv[1], "rootfs") == 0)
	{
		uint addr;
		uint len  = getenv_uint("rootfs_size");

		if (len == 0)
		{
			goto error;	
		}

		if (argc == 4)
		{
			addr = simple_strtoul(argv[4], NULL, 16);	
		}
		else
		{
			addr = getenv_uint("fileaddr");	
		}

		if (addr == 0)
		{
			goto error;	
		}

		if (argv[2][0] == 'p')
		{
			save_rootfs(cmdtp, flag, addr, len, 1);
		}
		else if (argv[2][0] == 's')
		{
			save_rootfs(cmdtp, flag, addr, len, 0);
		}
		else
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
	uint	kernel_loc;
	uint	kernel_p_loc;
	uint	kernel_s_loc;
	uint	kernel_size;
	uint	auto_recovery = 0;

	kernel_p_loc= getenv_uint("kernel_p_loc");
	kernel_s_loc= getenv_uint("kernel_s_loc");
	kernel_size	= getenv_uint("kernel_size");
	auto_recovery=getenv_uint("auto_recovery");

	if (check_kernel((void *)kernel_p_loc, 1) == 0)
	{
		kernel_loc = kernel_p_loc;
	}
	else if (check_kernel((void *)kernel_s_loc, 1) == 0)
	{
		if ((auto_recovery == 1) && (kernel_p_loc != INVALID_LOC))
		{
			copy_image((void *)kernel_p_loc, (void *)kernel_s_loc, kernel_size);
			kernel_loc = kernel_p_loc;
		}
		else
		{
			kernel_loc = kernel_s_loc;
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
	int		primary = 1;
	uint	rootfs_p_loc;
	uint	rootfs_s_loc;
	uint	auto_recovery = 0;
	int		backup = 0, restore = 0;

	rootfs_p_loc= getenv_uint("rootfs_p_loc");
	rootfs_s_loc= getenv_uint("rootfs_s_loc");
	auto_recovery =getenv_uint("auto_recovery");

	puts("Start Checking Primary RootFS\n");
	if (check_rootfs(1, 1) == 0)
	{
		puts("Primary RootFS is valid!\n");
	}
	else 
	{
		puts("Primary RootFS is invalid!\n");
		puts("Start Checking Secondary RootFS\n");
		if (check_rootfs(0, 1) == 0)
		{
			puts("Secondary RootFS is valid\n");
			if ((auto_recovery == 1) && (rootfs_p_loc != INVALID_LOC))
			{
				puts("Start restore(Secondary RootFS is copied to Primary RootFS)\n");
				copy_rootfs(1);
				puts("RootFS restore done.\n");
			}
			else
			{
				primary = 0;	
			}
		}
		else
		{
			puts("All RootFS is invalid!!!\n");
			goto error;
		}
	}

	if (primary)
	{
		setenv("mtd_root", "mtdblock5");
	}
	else
	{
		setenv("mtd_root", "mtdblock6");
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

	return	INVALID_LOC;
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

int check_rootfs(int primary, int printout)
{
	sys_info_t	sysinfo;
	void		*buff= (void*)CONFIG_SYS_LOAD_ADDR;	
	ulong		rootfs_loc;
	ulong		rootfs_size;
	nand_info_t *nand;

	if (primary)
	{
	}
	if (load_sys_info(&sysinfo) != 0)
	{
		puts("System information loading failed!\n");
		goto error;
	}

	if (primary)
	{
		rootfs_loc= getenv_uint("rootfs_p_loc");
		memcpy(buff, &sysinfo.rootfs_p, sizeof(image_header_t));
		if(rootfs_loc == INVALID_LOC)
		{
			puts("ERROR: can't primary location\n");	
		}
	}
	else
	{
		rootfs_loc= getenv_uint("rootfs_s_loc");
		memcpy(buff, &sysinfo.rootfs_s, sizeof(image_header_t));
		if(rootfs_loc == INVALID_LOC)
		{
			puts("ERROR: can't secondary location\n");	
		}
	}

	if (genimg_get_format (buff) != IMAGE_FORMAT_LEGACY)
	{
		puts("ERROR: Invalid image format!\n");
		goto error;
	}
		
	if (!image_check_hcrc (buff))
	{
		puts("ERROR: Invalid head crc!\n");
		goto error;
	}

	
	/* the following commands operate on the current device */
	if (nand_curr_device < 0 || nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[nand_curr_device].name) 
	{  
		puts("ERROR: no devices available\n");
		goto error;	
	}

	rootfs_size = ((image_header_t *)buff)->ih_size;

	nand = &nand_info[nand_curr_device];
	if (nand_read_skip_bad(nand, rootfs_loc, (size_t*)&rootfs_size, &((u_char *)buff)[sizeof(image_header_t)]) != 0)
	{
		puts("ERROR: nand read failed\n");
		goto error;	
	}

	if (!image_check_dcrc (buff)) 
	{
		puts("ERROR: Invalid rootfs image\n");
		goto error;
	} 

	printf ("Root FS at %08X:\n", (uint)rootfs_loc);
	image_print_contents (buff);

	return 0;

error:
	return	1;
}

int copy_image(void *dest, void *src, unsigned long len)
{
	unsigned long dest_start, dest_end;
	void *buff= (void*)CONFIG_SYS_LOAD_ADDR;	


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

	return	0;

error:
	return	1;
}

int copy_rootfs(int backup)
{
	sys_info_t	sys_info;
	void*		buff = (void*)CONFIG_SYS_LOAD_ADDR;
	uint		rootfs_p_loc= getenv_uint("rootfs_p_loc");
	uint		rootfs_s_loc= getenv_uint("rootfs_s_loc");
	uint		rootfs_size = getenv_uint("rootfs_size");

	if (backup)
	{
		if (_nand_read(rootfs_p_loc, rootfs_size,buff) != 0)
		{
			goto error;	
		}

		if (_nand_erase(rootfs_s_loc, rootfs_size, 1, 0) != 0 || 
			_nand_write(rootfs_s_loc, rootfs_size, buff) != 0)
		{
			goto error;	
		}

		load_sys_info(&sys_info);

		memcpy(&sys_info.rootfs_s, &sys_info.rootfs_p, sizeof(image_header_t));

		save_sys_info(&sys_info);
	
	}
	else
	{
		if (_nand_read(rootfs_s_loc, rootfs_size,buff) != 0)
		{
			goto error;	
		}

		if (_nand_erase(rootfs_p_loc, rootfs_size, 1, 0) != 0 || 
			_nand_write(rootfs_p_loc, rootfs_size, buff) != 0)
		{
			goto error;	
		}

		load_sys_info(&sys_info);

		memcpy(&sys_info.rootfs_p, &sys_info.rootfs_s, sizeof(image_header_t));

		save_sys_info(&sys_info);
	}

	return	0;

error:
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
	sys_info_t *base = 0;
	sys_info_t *base0 = (sys_info_t *)CONFIG_SYS_INFO_BASE;
	sys_info_t *base1 = (sys_info_t *)(CONFIG_SYS_INFO_BASE + 0x10000);

	if (base0->crc32 == crc32(0, (const unsigned char *)base0, sizeof(sys_info_t) - sizeof(uint)))
	{
		if (base1->crc32 == crc32(0, (const unsigned char *)base1, sizeof(sys_info_t) - sizeof(uint)))
		{
			if (base0->index < base1->index)
			{
				base = base1;
			}
			else
			{
				base = base0;	
			}
		}
		else
		{
			base = base0;	
		}
	}
	else
	{
		if (base1->crc32 == crc32(0, (const unsigned char *)base1, sizeof(sys_info_t) - sizeof(uint)))
		{
			base = base1;
		}
		else
		{
			goto error;
		}
	}

	memcpy(info, base, sizeof(sys_info_t));

	return	0;

error:
	return	1;
}

int	save_sys_info(sys_info_t *info)
{
	sys_info_t *base = 0;

	info->index++;
	info->crc32 = crc32(0, (const unsigned char *)info, sizeof(sys_info_t) - sizeof(uint));

	if (info->index % 2 == 0)
	{
		base = (sys_info_t *)CONFIG_SYS_INFO_BASE;
	}
	else
	{
		base = (sys_info_t *)(CONFIG_SYS_INFO_BASE + 0x10000);
	}


	copy_image(base, info, sizeof(sys_info_t));

	return	0;
}

int	save_rootfs(cmd_tbl_t * cmdtp, int flag, uint addr, uint len, int primary)
{
	sys_info_t	sys_info;
	char	buf[64];

	if (load_sys_info(&sys_info) != 0)
	{
		memset(&sys_info, 0, sizeof(sys_info));
		sys_info.magic = SYS_INFO_MAGIC;
	}

	if (primary)
	{
		uint	rootfs_p    = getenv_uint("rootfs_p_loc");	
		uint	rootfs_size = getenv_uint("rootfs_size");	

		memcpy(&sys_info.rootfs_p, (void *)addr, sizeof(image_header_t));
		_nand_erase(rootfs_p, rootfs_size, 1, 0);
		_nand_write(rootfs_p, rootfs_size, (void *)(addr + sizeof(image_header_t)));
	}
	else
	{
		uint	rootfs_s    = getenv_uint("rootfs_s_loc");	
		uint	rootfs_size = getenv_uint("rootfs_size");	

		memcpy(&sys_info.rootfs_s, (void *)addr, sizeof(image_header_t));
		_nand_erase(rootfs_s, rootfs_size, 1, 0);
		_nand_write(rootfs_s, rootfs_size, (void *)(addr + sizeof(image_header_t)));
	}

	save_sys_info(&sys_info);

	return	0;
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

