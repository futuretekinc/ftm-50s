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

extern int	board_power_hold(int on);

int do_power(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2)
	{
		goto error;
	}

	if (strcmp(argv[1], "on") == 0)
	{
		board_power_hold(1);
	}
	else if (strcmp(argv[1], "off") == 0)
	{
		board_power_hold(0);
	}
	else
	{
		goto error;	
	}

	return	0;	
error:
	return	1;
}


U_BOOT_CMD(
	power,	CONFIG_SYS_MAXARGS,	0,	do_power,
	"perform board power control",
	     "    - power control\n"
);


