/*
 * (C) Copyright 2009
 * Ryan Chen, ST Micoelectronics, ryan.chen@st.com.
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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

#include <common.h>
#include <miiphy.h>
#include <netdev.h>
#include <nand.h>
#include <asm/io.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spr_defs.h>
#include <asm/arch/spr_misc.h>

#define PLGPIO_SEL_6_9	0xb3000024
#define PLGPIO_IO_6_9	0xb3000034
#define PLGPIO_SEL_31	0xb3000024
#define PLGPIO_IO_31	0xb3000034
#define PLGPIO_SEL_35	0xb3000028
#define PLGPIO_IO_35	0xb3000038
#define PLGPIO_SEL_36	0xb3000028
#define PLGPIO_IO_36	0xb3000038
#define PLGPIO_SEL_41	0xb3000028
#define PLGPIO_IO_41	0xb3000038
#define PLGPIO_SEL_42	0xb3000028
#define PLGPIO_IO_42	0xb3000038
#define PLGPIO_SEL_69	0xb300002C
#define PLGPIO_IO_69	0xb300003C
#define PLGPIO_SEL_70	0xb300002C
#define PLGPIO_IO_70	0xb300003C
#define PLGPIO_SEL_73	0xb300002C
#define PLGPIO_IO_73	0xb300003C
#define PLGPIO_SEL_76	0xb300002C
#define PLGPIO_IO_76	0xb300003C
#define PLGPIO_31	(0x1 << 31)
#define PLGPIO_35	(0x1 << 3)
#define PLGPIO_36	(0x1 << 4)
#define PLGPIO_41	(0x1 << 9)
#define PLGPIO_42	(0x1 << 10)
#define PLGPIO_69	(0x1 << 5)
#define PLGPIO_70	(0x1 << 6)
#define PLGPIO_73	(0x1 << 9)
#define PLGPIO_76	(0x1 << 12)
#define PLGPIO_6_9	((0x1 << 6) | (0x1 << 9))

static void GPS_reset(void)
{
	/* PLGPIO73 is used to reset GPS module */
	writel(readl(PLGPIO_IO_73) & ~PLGPIO_73, PLGPIO_IO_73);
	writel(readl(PLGPIO_SEL_73) | PLGPIO_73, PLGPIO_SEL_73);
	writel(readl(PLGPIO_IO_73) | PLGPIO_73, PLGPIO_IO_73);
}

static void phy_reset(void)
{
	/* PLGPIO76 is used to reset phy */
	writel(readl(PLGPIO_IO_6_9) & ~PLGPIO_6_9, PLGPIO_IO_6_9);
	writel(readl(PLGPIO_SEL_6_9) | PLGPIO_6_9, PLGPIO_SEL_6_9);
	writel(readl(PLGPIO_IO_6_9) | PLGPIO_6_9, PLGPIO_IO_6_9);
}

static void NF_write_protect_off(void)
{
	/* PLGPIO69 is used to write protection off of nand flash */
	writel(readl(PLGPIO_IO_69) | PLGPIO_69, PLGPIO_IO_69);
	writel(readl(PLGPIO_SEL_69) | PLGPIO_69, PLGPIO_SEL_69);
}

static void telit_init(void)
{
	writel(readl(PLGPIO_IO_35) & ~PLGPIO_35, PLGPIO_IO_35);
	writel(readl(PLGPIO_SEL_35) | PLGPIO_35, PLGPIO_SEL_35);
	writel(readl(PLGPIO_IO_36) | PLGPIO_36, PLGPIO_IO_36);
	writel(readl(PLGPIO_SEL_36) | PLGPIO_36, PLGPIO_SEL_36);
	writel(readl(PLGPIO_IO_41) | PLGPIO_41, PLGPIO_IO_41);
	writel(readl(PLGPIO_SEL_41) | PLGPIO_41, PLGPIO_SEL_41);
	writel(readl(PLGPIO_IO_42) | PLGPIO_42, PLGPIO_IO_42);
	writel(readl(PLGPIO_SEL_42) | PLGPIO_42, PLGPIO_SEL_42);
	writel(readl(PLGPIO_IO_70) & ~PLGPIO_70, PLGPIO_IO_70);
	writel(readl(PLGPIO_SEL_70) | PLGPIO_70, PLGPIO_SEL_70);
	
}

int	telit_stat(void)
{
	static int stat = 0;
	printf("%08x %08x\n", readl(PLGPIO_IO_35), readl(PLGPIO_SEL_35));
	if (stat)
	{
		writel(readl(PLGPIO_IO_35) | PLGPIO_35, PLGPIO_IO_35);
		writel(readl(PLGPIO_SEL_35) | PLGPIO_35, PLGPIO_SEL_35);
	}
	else
	{
		writel(readl(PLGPIO_IO_35) & ~PLGPIO_35, PLGPIO_IO_35);
		writel(readl(PLGPIO_SEL_35) | PLGPIO_35, PLGPIO_SEL_35);
	}

	stat = !stat;
}

int board_init(void)
{
	writel(readl(0xB300000C) & ~(1 << 11), 0xB300000C);
	writel(readl(0xB3000024) | (1 << 7), 0xB3000024);
	writel(readl(0xB3000044) | (1 << 7), 0xB3000044);
	GPS_reset();
	phy_reset();
	NF_write_protect_off();
	telit_init();
	return spear_board_init(MACH_TYPE_SPEAR320_FTM);
}

/*
 * board_nand_init - Board specific NAND initialization
 * @nand:	mtd private chip structure
 *
 * Called by nand_init_chip to initialize the board specific functions
 */

int board_nand_init(struct nand_chip *nand)
{
#if defined(CONFIG_NAND_FSMC)
	struct misc_regs *const misc_regs_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		return fsmc_nand_init(nand);
	}
#endif
	return -1;
}

int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_MACB)
	struct misc_regs *const misc_regs_p =
		(struct misc_regs *)CONFIG_SPEAR_MISCBASE;
#endif
	int ret = 0;
#if defined(CONFIG_DESIGNWARE_ETH)
	u32 interface = PHY_INTERFACE_MODE_MII;
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;
#endif
#if defined(CONFIG_MACB)
	/* Enable AMEM clock for memory port access */
	writel(readl(&misc_regs_p->amem_cfg_ctrl) | 0x1,
			&misc_regs_p->amem_cfg_ctrl);

	if (macb_eth_initialize(0, (void *)CONFIG_SYS_MACB1_BASE,
				CONFIG_MACB0_PHY) >= 0)
		ret++;
#endif
	return ret;
}
