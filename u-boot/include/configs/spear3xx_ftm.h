/*
 * (C) Copyright 2009
 * Vipin Kumar, STMicroelectronics, <vipin.kumar@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#if defined(CONFIG_MK_spear300)
#define CONFIG_SPEAR3XX				1
#define CONFIG_SPEAR300				1
#elif defined(CONFIG_MK_spear310)
#define CONFIG_SPEAR3XX				1
#define CONFIG_SPEAR310				1
#elif defined(CONFIG_MK_spear320)
	#if defined(CONFIG_MK_hmi)
	#define CONFIG_SPEAR3XX			1
	#define CONFIG_SPEAR320_HMI		1
	#elif defined(CONFIG_MK_ftm)
	#define CONFIG_SPEAR3XX			1
	#define CONFIG_SPEAR320_FTM		1
	#else
	#define CONFIG_SPEAR3XX			1
	#define CONFIG_SPEAR320 		1
	#endif
#endif

#define CONFIG_ENV_IS_IN_FLASH			1

#if defined(CONFIG_MK_usbtty)
#define CONFIG_SPEAR_USBTTY			1

/*
 * To support saveenv command
 */
#endif

#include <configs/spear-common.h>

#define CONFIG_SYS_INFO_BASE	0xF87E0000

#undef	CONFIG_USB_STORAGE

#if !defined(CONFIG_SPEAR_USBTTY)
/* Ethernet driver configuration */
#define CONFIG_DW_ALTDESCRIPTOR			1

#if defined(CONFIG_SPEAR310)
#define CONFIG_ETH_MDIO_HOOK			1
#define CONFIG_MACB				1
#define CONFIG_MACB0_PHY			0x01
#define CONFIG_MACB1_PHY			0x03
#define CONFIG_MACB2_PHY			0x05
#define CONFIG_MACB3_PHY			0x07

#elif defined(CONFIG_SPEAR320)
#define CONFIG_ETH_MDIO_HOOK			1
#define CONFIG_MACB				1
#define CONFIG_MACB0_PHY			0x01
#define CONFIG_MACB1_PHY			0x02

#elif defined(CONFIG_SPEAR320_HMI)
#define CONFIG_ETH_MDIO_HOOK			1
#define CONFIG_MACB				1
#define CONFIG_MACB0_PHY			0x1
#define CONFIG_MACB1_PHY			0x0

#elif defined(CONFIG_SPEAR320_FTM)
#define CONFIG_ETH_MDIO_HOOK			1
#define CONFIG_MACB				1
#define CONFIG_MACB0_PHY			0x00
#define CONFIG_MACB1_PHY			0x01

#endif
#endif

#define CONFIG_ETHADDR		00:40:5c:01:01:01
#define CONFIG_ETH1ADDR		00:40:5c:01:01:02
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.2.250
#define CONFIG_SERVERIP		192.168.2.1
#define CONFIG_GATEWAYIP	192.168.2.1

/* Serial Configuration (PL011) */
#define CONFIG_SYS_SERIAL0			0xD0000000

#if defined(CONFIG_SPEAR300)
#define CONFIG_PL01x_PORTS			{(void *)CONFIG_SYS_SERIAL0}

#elif defined(CONFIG_SPEAR310)

#if (CONFIG_CONS_INDEX)
#undef  CONFIG_PL011_CLOCK
#define CONFIG_PL011_CLOCK			(83 * 1000 * 1000)
#endif

#define CONFIG_SYS_SERIAL1			0xB2000000
#define CONFIG_SYS_SERIAL2			0xB2080000
#define CONFIG_SYS_SERIAL3			0xB2100000
#define CONFIG_SYS_SERIAL4			0xB2180000
#define CONFIG_SYS_SERIAL5			0xB2200000
#define CONFIG_PL01x_PORTS			{(void *)CONFIG_SYS_SERIAL0, \
						(void *)CONFIG_SYS_SERIAL1, \
						(void *)CONFIG_SYS_SERIAL2, \
						(void *)CONFIG_SYS_SERIAL3, \
						(void *)CONFIG_SYS_SERIAL4, \
						(void *)CONFIG_SYS_SERIAL5 }
#elif defined(CONFIG_SPEAR320) || defined(CONFIG_SPEAR320_HMI) || defined(CONFIG_SPEAR320_FTM)

#if (CONFIG_CONS_INDEX)
#undef  CONFIG_PL011_CLOCK
#define CONFIG_PL011_CLOCK			(83 * 1000 * 1000)
#endif

#define CONFIG_SYS_SERIAL1			0xA3000000
#define CONFIG_SYS_SERIAL2			0xA4000000
#define CONFIG_PL01x_PORTS			{(void *)CONFIG_SYS_SERIAL0, \
						(void *)CONFIG_SYS_SERIAL1, \
						(void *)CONFIG_SYS_SERIAL2 }
#endif

#if defined(CONFIG_SPEAR_EMI)

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER

#if defined(CONFIG_SPEAR310)
#define CONFIG_SYS_FLASH_PROTECTION		1
#define CONFIG_SYS_FLASH_BASE			0x50000000
#define CONFIG_SYS_CS1_FLASH_BASE		0x60000000
#define CONFIG_SYS_CS2_FLASH_BASE		0x70000000
#define CONFIG_SYS_CS3_FLASH_BASE		0x80000000
#define CONFIG_SYS_CS4_FLASH_BASE		0x90000000
#define CONFIG_SYS_CS5_FLASH_BASE		0xA0000000
#define CONFIG_SYS_FLASH_BANKS_LIST		{ CONFIG_SYS_FLASH_BASE,   \
						CONFIG_SYS_CS1_FLASH_BASE, \
						CONFIG_SYS_CS2_FLASH_BASE, \
						CONFIG_SYS_CS3_FLASH_BASE, \
						CONFIG_SYS_CS4_FLASH_BASE, \
						CONFIG_SYS_CS5_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS		6

#elif defined(CONFIG_SPEAR320) || defined(CONFIG_SPEAR320_HMI) || defined(CONFIG_SPEAR320_FTM)
#define CONFIG_SYS_FLASH_PROTECTION		1
#define CONFIG_SYS_FLASH_BASE			0x44000000
#define CONFIG_SYS_CS1_FLASH_BASE		0x45000000
#define CONFIG_SYS_CS2_FLASH_BASE		0x46000000
#define CONFIG_SYS_CS3_FLASH_BASE		0x47000000
#define CONFIG_SYS_FLASH_BANKS_LIST		{ CONFIG_SYS_FLASH_BASE,   \
						CONFIG_SYS_CS1_FLASH_BASE, \
						CONFIG_SYS_CS2_FLASH_BASE, \
						CONFIG_SYS_CS3_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS		4

#endif

#define CONFIG_SYS_MAX_FLASH_SECT		(127 + 8)
#define CONFIG_SYS_FLASH_QUIET_TEST		1

#endif

/* NAND flash configuration */
#define CONFIG_SYS_FSMC_NAND_8BIT		1

//#define CONFIG_MTD_DEBUG        1
//#define CONFIG_MTD_DEBUG_VERBOSE 3

#define	CONFIG_CMD_RECOVERY

#if defined(CONFIG_SPEAR300)
#define CONFIG_SYS_NAND_BASE			(0x80000000)

#elif defined(CONFIG_SPEAR310)
#define CONFIG_SYS_NAND_BASE			(0x40000000)

#elif defined(CONFIG_SPEAR320) || defined(CONFIG_SPEAR320_HMI) || defined(CONFIG_SPEAR320_FTM)
#define CONFIG_SYS_NAND_BASE			(0x50000000)

#endif

#undef	CONFIG_EXTRA_ENV_SETTINGS		
#define	CONFIG_EXTRA_ENV_SETTINGS	\
	"model=FTM-50S\0"\
	"devid=FTM-50S01A0001\0"\
	"uboot_p_loc=0xf8010000\0"\
	"kernel_p_loc=0xf8050000\0"\
	"kernel_s_loc=0xf83d0000\0"\
	"kernel_size=0x00380000\0"\
	"rootfs_p_loc=0x00000000\0"\
	"rootfs_s_loc=0x02000000\0"\
	"rootfs_size=0x01E00000\0"\
	"overlay_p_loc=0x04000000\0"\
	"overlay_s_loc=0x06000000\0"\
	"overlay_size=0x02000000\0"\
	"user_p_loc=0x08000000\0"\
	"flashfiles=ftm-50s-new\0"\
	"rf_uboot=run rf_uboot_dn rf_uboot_p\0"\
	"rf_uboot_dn=tftp \$(flashfiles)u-boot.img\0"\
	"rf_uboot_p=protect off 1:1-3\;erase 1:1-3\;cp.b $(fileaddr) \$(uboot_p_loc) \$(filesize)\;protect on 1:1-3\0"\
	"rf_kernel=run rf_kernel_dn rf_kernel_p rf_kernel_s\0"\
	"rf_kernel_dn=tftp \$(flashfiles)uImage\0"\
	"rf_kernel_p=erase 1:5-60\;cp.b \$(fileaddr) \$(kernel_p_loc) \$(kernel_size)\0"\
	"rf_kernel_s=erase 1:61-116\;cp.b \$(fileaddr) \$(kernel_s_loc) \$(kernel_size)\0"\
	"rf_rootfs=run rf_rootfs_dn rf_rootfs_p rf_rootfs_s\0"\
	"rf_rootfs_dn=tftp \$(flashfiles)rootfs.img\0"\
	"rf_rootfs_p=savefs rootfs p\0"\
	"rf_rootfs_s=savefs rootfs s\0"\
	"rf_overlay=nand erase \$(overlay_p_loc) \$(overlay_size)\;nand erase \$(overlay_s_loc) \$(overlay_size)\0"\
	"rf_user=nand erase \$(user_p_loc)\0"\
	"kernel_loc=0xf8050000\0"\
	"mtd_root=mtdblock5\0"\
	"auto_recovery=1\0"\
	"setbootargs=setenv bootargs console=ttyS0,115200 mem=128M root=/dev/\$(mtd_root) ro rootwait noinitrd init=/sbin/overlay_init ethaddr=\$(ethaddr) eth1addr=\$(eth1addr) model=\$(model) devid=\$(devid)\0"

#undef	CONFIG_BOOTARGS

#undef	CONFIG_RAMBOOTCOMMAND
#undef	CONFIG_NFSBOOTCOMMAND

#undef	CONFIG_BOOTCOMMAND
#define	CONFIG_BOOTCOMMAND	"recovery;run setbootargs;bootm $(kernel_loc)"

#endif  /* __CONFIG_H */
