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
#define PLGPIO_SEL_39	0xb3000028
#define PLGPIO_IO_39	0xb3000038
#define PLGPIO_SEL_41	0xb3000028
#define PLGPIO_IO_41	0xb3000038
#define PLGPIO_SEL_42	0xb3000028
#define PLGPIO_IO_42	0xb3000038
#define PLGPIO_SEL_43	0xb3000028
#define PLGPIO_IO_43	0xb3000038
#define PLGPIO_SEL_44	0xb3000028
#define PLGPIO_IO_44	0xb3000038
#define PLGPIO_SEL_45	0xb3000028
#define PLGPIO_IO_45	0xb3000038
#define PLGPIO_SEL_46	0xb3000028
#define PLGPIO_IO_46	0xb3000038
#define PLGPIO_SEL_51	0xb3000028
#define PLGPIO_IO_51	0xb3000038
#define PLGPIO_SEL_61	0xb3000028
#define PLGPIO_IO_61	0xb3000038
#define PLGPIO_SEL_69	0xb300002C
#define PLGPIO_IO_69	0xb300003C
#define PLGPIO_SEL_70	0xb300002C
#define PLGPIO_IO_70	0xb300003C
#define PLGPIO_SEL_73	0xb300002C
#define PLGPIO_IO_73	0xb300003C
#define PLGPIO_SEL_76	0xb300002C
#define PLGPIO_IO_76	0xb300003C
#define PLGPIO_SEL_99	0xb3000030
#define PLGPIO_IO_99	0xb3000040
#define PLGPIO_SEL_101	0xb3000030
#define PLGPIO_IO_101	0xb3000040
#define PLGPIO_31	(0x1 << 31)
#define PLGPIO_35	(0x1 << 3)
#define PLGPIO_36	(0x1 << 4)
#define PLGPIO_41	(0x1 << 9)
#define PLGPIO_42	(0x1 << 10)
#define PLGPIO_43	(0x1 << 11)
#define PLGPIO_44	(0x1 << 12)
#define PLGPIO_45	(0x1 << 13)
#define PLGPIO_46	(0x1 << 14)
#define PLGPIO_51	(0x1 << 19)
#define PLGPIO_61	(0x1 << 29)
#define PLGPIO_69	(0x1 << 5)
#define PLGPIO_70	(0x1 << 6)
#define PLGPIO_73	(0x1 << 9)
#define PLGPIO_76	(0x1 << 12)
#define PLGPIO_99	(0x1 << 3)
#define PLGPIO_99	(0x1 << 3)
#define PLGPIO_101	(0x1 << 5)
#define PLGPIO_6_9	((0x1 << 6) | (0x1 << 9))

#define	PORT_ID(x)	((x) + 8)

static void phy_reset(void)
{
	/* PLGPIO76 is used to reset phy */
	writel(readl(PLGPIO_IO_6_9) & ~PLGPIO_6_9, PLGPIO_IO_6_9);
	writel(readl(PLGPIO_SEL_6_9) | PLGPIO_6_9, PLGPIO_SEL_6_9);
	writel(readl(PLGPIO_IO_6_9) | PLGPIO_6_9, PLGPIO_IO_6_9);
}

int board_init(void)
{
	writel(readl(0xB300000C) & ~(1 << 11), 0xB300000C);
	writel(readl(0xB300000C) & ~(1 << 1), 0xB300000C);
	writel(readl(0xB300000C) & ~(1 << 0), 0xB300000C);
	writel(readl(0xB3000024) | (1 << 7), 0xB3000024);
	writel(readl(0xB3000044) | (1 << 7), 0xB3000044);

	writel(readl(0xB3000028) | (1 << 7), 0xB3000028);
	writel(readl(0xB3000048) | (1 << 7), 0xB3000048);

	phy_reset();

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
	int	ret;
	struct misc_regs *const misc_regs_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		board_power_hold(1);
		ret = fsmc_nand_init(nand);
		board_power_hold(0);

		return	ret;
	}
#endif
	return -1;
}

int board_eth_init(bd_t *bis)
{
	int i;
	unsigned short reg_short = 0x8000;

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


	miiphy_write("mii0", 0x0F, 0x0A, 0x8000);

	reg_short = 0x8000;
	while(reg_short & 0x8000)
	{
		miiphy_read("mii0", 0x0F, 0x0A, &reg_short);
	}

	miiphy_read("mii0", 0x08, 0x03, &reg_short) ;

	for(i = 0 ; i < 4 ; i++)
	{
		miiphy_write("mii0", i, PHY_BMCR, PHY_BMCR_RESET);
		reg_short = PHY_BMCR_RESET;

		while (reg_short & PHY_BMCR_RESET)
		{
			miiphy_read("mii0", i, PHY_BMCR, &reg_short);// Be here until Phy_Reset bit gets clear
		}
	}

	for(i = 0 ; i < 6 ; i++)
	{
		miiphy_read("mii0", PORT_ID(i), 0x04, &reg_short);
		miiphy_write("mii0", PORT_ID(i), 0x04, reg_short & ~0x0003);
	}

	miiphy_write("mii0", PORT_ID(0), 0x06, 0x0010);
	miiphy_write("mii0", PORT_ID(1), 0x06, 0x002C);
	miiphy_write("mii0", PORT_ID(2), 0x06, 0x002A);
	miiphy_write("mii0", PORT_ID(3), 0x06, 0x0026);
	miiphy_write("mii0", PORT_ID(4), 0x06, 0x0001);
	miiphy_write("mii0", PORT_ID(5), 0x06, 0x000E);

	for(i = 0 ; i < 6 ; i++)
	{
		miiphy_read("mii0", PORT_ID(i), 0x04, &reg_short);
		miiphy_write("mii0", PORT_ID(i), 0x04, reg_short | 0x0003);
	}

	for(i = 0 ; i < 4 ; i++)
	{
		miiphy_write("mii0", i, 0x16, 0x4A35);
	}

	return ret;
}

int board_power_hold(int on)
{
	if (on)
	{
		writel(readl(0xB3000038) | (1 << 14), 0xB3000038);
	}
	else
	{
		writel(readl(0xB3000038) & ~(1 << 14), 0xB3000038);
	}

	return	0;
	
}
