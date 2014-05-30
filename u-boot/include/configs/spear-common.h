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

#ifndef _SPEAR_COMMON_H
#define _SPEAR_COMMON_H
/*
 * Common configurations used for both spear3xx as well as spear6xx
 */

#if !defined(CONFIG_SPEAR_USBTTY)
/* Ethernet driver configuration */
#define CONFIG_MII
#define CONFIG_DESIGNWARE_ETH
#define CONFIG_DW_SEARCH_PHY
#define CONFIG_DW0_PHY				1
#define CONFIG_NET_MULTI
#define CONFIG_PHY_RESET_DELAY			(10000)		/* in usec */
#define CONFIG_DW_AUTONEG			1
#endif

/* USBD driver configuration */
#if defined(CONFIG_SPEAR_USBTTY)
#define CONFIG_DW_UDC
#define CONFIG_USB_DEVICE
#define CONFIG_USBD_HS
#define CONFIG_USB_TTY

#define CONFIG_USBD_PRODUCT_NAME		"SPEAr SoC"
#define CONFIG_USBD_MANUFACTURER		"ST Microelectronics"

#endif

#define CONFIG_EXTRA_ENV_USBTTY			"usbtty=cdc_acm\0"

/* I2C driver configuration */
#define CONFIG_HARD_I2C
#define CONFIG_DW_I2C
#define CONFIG_SYS_I2C_SPEED			400000
#define CONFIG_SYS_I2C_SLAVE			0x02

#define CONFIG_I2C_CHIPADDRESS			0x50

/* Timer, HZ specific defines */
#define CONFIG_SYS_HZ				(1000)

/* Flash configuration */
#if defined(CONFIG_FLASH_PNOR)
#define CONFIG_SPEAR_EMI			1
#else
#define CONFIG_ST_SMI				1
#endif

#if defined(CONFIG_ST_SMI)

#define CONFIG_SYS_MAX_FLASH_BANKS		2
#define CONFIG_SYS_FLASH_BASE			(0xF8000000)
#define CONFIG_SYS_CS1_FLASH_BASE		(0xF9000000)
#define CONFIG_SYS_FLASH_BANK_SIZE		(0x01000000)
#define CONFIG_SYS_FLASH_ADDR_BASE		{CONFIG_SYS_FLASH_BASE, \
						CONFIG_SYS_CS1_FLASH_BASE}
#define CONFIG_SYS_MAX_FLASH_SECT		128

#define CONFIG_SYS_FLASH_ERASE_TOUT		(3 * CONFIG_SYS_HZ)
#define CONFIG_SYS_FLASH_WRITE_TOUT		(3 * CONFIG_SYS_HZ)

#endif

/*
 * Serial Configuration (PL011)
 * CONFIG_PL01x_PORTS is defined in specific files
 */
#define CONFIG_PL011_SERIAL
#define CONFIG_PL011_CLOCK			(48 * 1000 * 1000)
#define CONFIG_CONS_INDEX			0
#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE		{ 9600, 19200, 38400, \
						57600, 115200 }

#define CONFIG_SYS_LOADS_BAUD_CHANGE

/*
 * USB/EHCI
 */
#if !defined(CONFIG_SPEAR_USBTTY)
#define CONFIG_USB_EHCI				/* Enable EHCI USB support */
#define CONFIG_USB_EHCI_SPEAR
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB				/* Enable USB Commands */
#endif

#if defined(CONFIG_USB_STORAGE)
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#endif

/* NAND FLASH Configuration */
#define CONFIG_NAND_FSMC			1
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_MTD_NAND_VERIFY_WRITE		1
#define CONFIG_SYS_NAND_ONFI_DETECTION		1
#define CONFIG_SYS_NAND_QUIET_TEST		1

/*
 * Command support defines
 */
#define CONFIG_CMD_I2C
#define CONFIG_CMD_NAND
#define CONFIG_CMD_ENV
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_RUN
#define CONFIG_CMD_SAVES

#if !defined(CONFIG_SPEAR_USBTTY)
#define CONFIG_CMD_NET
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#endif

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <config_cmd_default.h>

#if defined(CONFIG_SPEAR_USBTTY)
#undef CONFIG_CMD_NET
#endif
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_LOADS

/*
 * Default Environment Varible definitions
 */
#if defined(CONFIG_SPEAR_USBTTY)
#define CONFIG_BOOTDELAY			-1
#else
#define CONFIG_BOOTDELAY			1
#endif

#define CONFIG_ENV_OVERWRITE

/*
 * U-Boot Environment placing definitions.
 */
#if defined(CONFIG_ENV_IS_IN_FLASH)
#ifdef CONFIG_ST_SMI
/*
 * Environment is in serial NOR flash
 */
#define CONFIG_SYS_MONITOR_LEN			0x00040000
#define CONFIG_ENV_SECT_SIZE			0x00010000
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "

#define CONFIG_BOOTCOMMAND			"bootm 0xf8050000"

#elif defined(CONFIG_SPEAR_EMI)
/*
 * Environment is in parallel NOR flash
 */
#define CONFIG_SYS_MONITOR_LEN			0x00060000
#define CONFIG_ENV_SECT_SIZE			0x00020000
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "

#define CONFIG_BOOTCOMMAND			"cp.b 0x50080000 0x1600000 " \
						"0x4C0000; bootm 0x1600000"
#endif

#define CONFIG_ENV_ADDR				(CONFIG_SYS_FLASH_BASE + \
						CONFIG_SYS_MONITOR_LEN)
#elif defined(CONFIG_ENV_IS_IN_NAND)
/*
 * Environment is in NAND
 */

#define CONFIG_ENV_OFFSET			0x60000
#define CONFIG_ENV_RANGE			0x10000
#define CONFIG_FSMTDBLK				"/dev/mtdblock7 "

#if defined(CONFIG_SPEAR310) || defined(CONFIG_SPEAR320_HMI) || defined(CONFIG_SPEAR320_FTM)
#define CONFIG_OSBOOTOFF			"0x200000 "

#else
#define CONFIG_OSBOOTOFF			"0x80000 "
#endif

#define CONFIG_BOOTCOMMAND			"nand read.jffs2 0x1600000 " \
						CONFIG_OSBOOTOFF "0x4C0000; " \
						"bootm 0x1600000"

#elif defined(CONFIG_ENV_IS_NOWHERE)
/*
 * Environment is in NAND/FLASH, saveenv command supported
 */

#define CONFIG_ENV_RANGE			0x10000
#if defined(CONFIG_SPEAR_EMI)
#define CONFIG_ENV_SECT_SIZE			0x20000
#else
#define CONFIG_ENV_SECT_SIZE			0x10000
#endif
#endif

#ifdef CONFIG_SPEAR310
#undef CONFIG_FSMTDBLK

#if defined(CONFIG_ENV_IS_IN_FLASH)
#define CONFIG_FSMTDBLK				"/dev/mtdblock7 "

#elif defined(CONFIG_ENV_IS_IN_NAND)
#define CONFIG_FSMTDBLK				"/dev/mtdblock11 "
#endif

#endif /* CONFIG_SPEAR310 */

#ifdef CONFIG_FSMTDBLK
#define ROOT_FSMTD				"root="CONFIG_FSMTDBLK \
						"rootfstype=jffs2"
#else
#define ROOT_FSMTD				""
#endif

#define CONFIG_BOOTARGS				"console=ttyAMA0,115200 " \
						"mem=128M "  \
						ROOT_FSMTD

#define CONFIG_NFSBOOTCOMMAND						\
	"bootp; "							\
	"setenv bootargs root=/dev/nfs rw "				\
	"nfsroot=$(serverip):$(rootpath) "				\
	"ip=$(ipaddr):$(serverip):$(gatewayip):"			\
			"$(netmask):$(hostname):$(netdev):off "		\
			"console=ttyAMA0,115200 $(othbootargs);"	\
	"bootm; "

#define CONFIG_RAMBOOTCOMMAND						\
	"setenv bootargs root=/dev/ram rw "				\
		"console=ttyAMA0,115200 $(othbootargs);"		\
	"bootm; "

#define CONFIG_ENV_SIZE				0x02000
#define CONFIG_SYS_MONITOR_BASE			TEXT_BASE
#define CONFIG_MONITOR_IS_IN_RAM		1

/* Miscellaneous configurable options */
#define CONFIG_ARCH_CPU_INIT			1
#define CONFIG_DISPLAY_CPUINFO			1
#define CONFIG_BOOT_PARAMS_ADDR			0x00000100
#define CONFIG_CMDLINE_TAG			1
#define CONFIG_SETUP_MEMORY_TAGS		1
#define CONFIG_MISC_INIT_R			1
#define CONFIG_ZERO_BOOTDELAY_CHECK		1
#define CONFIG_AUTOBOOT_KEYED			1
#define CONFIG_AUTOBOOT_STOP_STR		" "
#define CONFIG_AUTOBOOT_PROMPT			\
		"Hit SPACE in %d seconds to stop autoboot.\n", bootdelay

#define CONFIG_SYS_MEMTEST_START		0x00800000
#define CONFIG_SYS_MEMTEST_END			0x04000000
#define CONFIG_SYS_MALLOC_LEN			(1024*1024)
#define CONFIG_SYS_GBL_DATA_SIZE		128
#define CONFIG_IDENT_STRING			"-SPEAr"
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT			"u-boot> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_CBSIZE			256
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE + \
						sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS			32
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE
#define CONFIG_SYS_LOAD_ADDR			0x00800000
#define CONFIG_SYS_CONSOLE_INFO_QUIET		1
#define CONFIG_SYS_64BIT_VSPRINTF		1

#define CONFIG_SYS_FLASH_EMPTY_INFO		1
#define CONFIG_EXTRA_ENV_UNLOCK			"unlock=yes\0"
#define CONFIG_EXTRA_ENV_SETTINGS		CONFIG_EXTRA_ENV_USBTTY	\
						CONFIG_EXTRA_ENV_UNLOCK

/* Stack sizes */
#define CONFIG_STACKSIZE			(128*1024)

#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ			(4*1024)
#define CONFIG_STACKSIZE_FIQ			(4*1024)
#endif

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS			1
#define PHYS_SDRAM_1				0x00000000
#define PHYS_SDRAM_1_MAXSIZE			0x08000000

#endif
