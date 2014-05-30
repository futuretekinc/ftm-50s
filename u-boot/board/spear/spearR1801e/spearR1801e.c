/*
 * (C) Copyright 2010
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
 *
 * (C) Copyright 2011
 * Armando Visconti, ST Micoelectronics, armando.visconti@st.com.
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
#include <linux/mtd/st_smi.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_SPEARR1801E;
	gd->bd->bi_boot_params = CONFIG_BOOT_PARAMS_ADDR;

#if defined(CONFIG_ST_SMI)
	smi_init();
#endif
	return 0;
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
	return fsmc_nand_init(nand);
#else
	return -1;
#endif
}

#if defined(CONFIG_CMD_NET)
int board_eth_init(bd_t *bis)
{
	int ret = 0;
#if defined(CONFIG_DESIGNWARE_ETH)
	char *devname;
	u16 phyid1, phyid2;
	u32 interface = PHY_INTERFACE_MODE_MII;
#if defined(CONFIG_DW_AUTONEG)
	interface = PHY_INTERFACE_MODE_GMII;
#endif
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;

#ifdef CONFIG_MII
	devname = miiphy_get_current_dev();

	miiphy_read(devname, CONFIG_DW0_PHY, PHY_PHYIDR1, &phyid1);
	miiphy_read(devname, CONFIG_DW0_PHY, PHY_PHYIDR2, &phyid2);

	if (phyid1 == 0x0022 && (phyid2 & 0xfff0) == 0x1610) {
		/*
		 * Note: Adjust timing within Micrel PHY, otherwise link
		 * doesn't work
		 */
		miiphy_write(devname, CONFIG_DW0_PHY, 0x0b, 0x8104);
		miiphy_write(devname, CONFIG_DW0_PHY, 0x0c, 0x7700);
	}
#endif /* CONFIG_MII */
#endif /* CONFIG_DESIGNWARE_ETH */
	return ret;
}
#endif
