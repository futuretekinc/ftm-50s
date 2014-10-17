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
#include <common.h>
#include <command.h>
#include <post.h>

int check_image(void *loc, int printout);
int copy_image(void *dest, void *src, unsigned long len);

int do_recovery(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char * val;
	void * kernel_loc = 0;
	void * kernel_p_loc = 0;
	void * kernel_s_loc = 0;
	void * rootfs_p_loc = 0;
	void * rootfs_s_loc = 0;
	unsigned long	kernel_len;

	val = getenv("kernel_p_loc");
	if (val == 0)
	{
		goto error;
	}

	kernel_p_loc = (void *)simple_strtoul(val, NULL, 16);
	if (check_image(kernel_p_loc, 1) == 0)
	{
		kernel_loc = kernel_p_loc;	
	}
	else
	{
		puts ("Primary kernel image incorrect!\n");
		puts ("Kernel recovery processing start!\n");

		val = getenv("kernel_s_loc");
		if (val == 0)
		{
			goto error;
		}

		kernel_s_loc = (void *)simple_strtoul(val, NULL, 16);
		if (check_image(kernel_s_loc, 1) != 0)
		{
			goto error;
		}

		val = getenv("kernel_size");
		if (val == 0)
		{
			goto error;	
		}
		kernel_len = simple_strtoul(val, NULL, 16);
		copy_image(kernel_p_loc, kernel_s_loc, kernel_len);

		kernel_loc = kernel_p_loc;
	}

	
	return	0;
error:
	return	1;

}

int check_image(void *loc, int printout)
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
		printf ("Legacy Image at %08lX:\n", (ulong)loc);
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

int copy_image(void *dest, void *src, unsigned long len)
{
	unsigned long dest_start, dest_end;
	void *buff= (void*)CONFIG_SYS_LOAD_ADDR;	


	memcpy(buff, src, len);

	dest_start = (ulong)dest;
	dest_end = (ulong)dest + len - 1;
	if (flash_sect_protect (0, dest_start, dest_end)) 
	{
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

/***************************************************/

U_BOOT_CMD(
	recovery,	CONFIG_SYS_MAXARGS,	0,	do_recovery,
	"perform board recovery",
	     "    - system recovery\n"
);
