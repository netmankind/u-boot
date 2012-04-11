/*
 * (C) Copyright 2011, 2012
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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
 * Board specific code for the Emcraft K70-SOM board
 * on the Emcraft SOM-BSB Baseboard.
 */

#include <common.h>
#include <netdev.h>

#include <asm/arch/kinetis.h>
#include <asm/arch/kinetis_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Misc Control Register (SIM_MCR)
 */
/* Pin enable for all DDR I/O */
#define KINETIS_SIM_MCR_DDRPEN_MSK	(1 << 2)
/* DDR configuration select */
#define KINETIS_SIM_MCR_DDRCFG_BITS	5
/* LPDDR Half Strength */
#define KINETIS_SIM_MCR_DDRCFG_LPDDR_HALF \
	(0 << KINETIS_SIM_MCR_DDRCFG_BITS)

/*
 * DDR1/2/LP SDRAM Memory Controller (DDRMC) register map
 */
struct kinetis_ddr_regs {
	u32 cr[64];	/* Control Registers */
	u32 rsv0[32];	/* Reserved: DDR + 0x100 .. DDR + 0x180 */
	u32 rcr;	/* RCR Control Register */
	u32 rsv1[10];	/* Reserved: DDR + 0x184 .. DDR + 0x1AC */
	u32 pad_ctrl;	/* I/O Pad Control Register */
};

/*
 * DDRMC registers base
 */
#define KINETIS_DDR_BASE		(KINETIS_AIPS1PERIPH_BASE + 0x0002E000)
#define KINETIS_DDR			((volatile struct kinetis_ddr_regs *) \
					KINETIS_DDR_BASE)

/*
 * DDR Control Register 0 (DDR_CR00)
 */
/* DRAM Class */
#define KINETIS_DDR_CR00_DDRCLS_BITS	8
/* LPDDR */
#define KINETIS_DDR_CR00_DDRCLS_LPDDR	(1 << KINETIS_DDR_CR00_DDRCLS_BITS)
#define KINETIS_DDR_CR00_START_MSK	(1 << 0)
/*
 * DDR Control Register 2 (DDR_CR02)
 */
#define KINETIS_DDR_CR02_INITAREF_BITS	24
#define KINETIS_DDR_CR02_TINIT_BITS	0
/*
 * DDR Control Register 3 (DDR_CR03)
 */
#define KINETIS_DDR_CR03_TCCD_BITS	24
#define KINETIS_DDR_CR03_WRLAT_BITS	16
#define KINETIS_DDR_CR03_LATGATE_BITS	8
#define KINETIS_DDR_CR03_LATLIN_BITS	0
/*
 * DDR Control Register 4 (DDR_CR04)
 */
#define KINETIS_DDR_CR04_TRASMIN_BITS	24
#define KINETIS_DDR_CR04_TRC_BITS	16
#define KINETIS_DDR_CR04_TRRD_BITS	8
#define KINETIS_DDR_CR04_TBINT_BITS	0
/*
 * DDR Control Register 5 (DDR_CR05)
 */
#define KINETIS_DDR_CR05_TMRD_BITS	24
#define KINETIS_DDR_CR05_TRTP_BITS	16
#define KINETIS_DDR_CR05_TRP_BITS	8
#define KINETIS_DDR_CR05_TWTR_BITS	0
/*
 * DDR Control Register 6 (DDR_CR06)
 */
#define KINETIS_DDR_CR06_TRASMAX_BITS	8
#define KINETIS_DDR_CR06_TMOD_BITS	0
/*
 * DDR Control Register 7 (DDR_CR07)
 */
#define KINETIS_DDR_CR07_CCAPEN_MSK	(1 << 24)
#define KINETIS_DDR_CR07_TCKESR_BITS	8
#define KINETIS_DDR_CR07_CLKPW_BITS	0
/*
 * DDR Control Register 8 (DDR_CR08)
 */
#define KINETIS_DDR_CR07_TDAL_BITS	24
#define KINETIS_DDR_CR07_TWR_BITS	16
#define KINETIS_DDR_CR07_TRASDI_BITS	8
#define KINETIS_DDR_CR08_TRAS_MSK	(1 << 0)
/*
 * DDR Control Register 9 (DDR_CR09)
 */
#define KINETIS_DDR_CR09_BSTLEN_BITS	24
#define KINETIS_DDR_CR09_BSTLEN_8W	(3 << KINETIS_DDR_CR09_BSTLEN_BITS)
#define KINETIS_DDR_CR09_TDLL_BITS	0
/*
 * DDR Control Register 10 (DDR_CR10)
 */
#define KINETIS_DDR_CR10_TRPAB_BITS	24
#define KINETIS_DDR_CR10_TCPD_BITS	8
#define KINETIS_DDR_CR10_TFAW_BITS	0
/*
 * DDR Control Register 11 (DDR_CR11)
 */
#define KINETIS_DDR_CR11_TREFEN_MSK	(1 << 24)
/*
 * DDR Control Register 12 (DDR_CR12)
 */
#define KINETIS_DDR_CR12_TREF_BITS	16
#define KINETIS_DDR_CR12_TRFC_BITS	0
/*
 * DDR Control Register 14 (DDR_CR14)
 */
#define KINETIS_DDR_CR14_TXSR_BITS	16
#define KINETIS_DDR_CR14_TPDEX_BITS	0
/*
 * DDR Control Register 15 (DDR_CR15)
 */
#define KINETIS_DDR_CR15_TXSNR_BITS	0
/*
 * DDR Control Register 16 (DDR_CR16)
 */
#define KINETIS_DDR_CR16_QKREF_MSK	(1 << 0)
/*
 * DDR Control Register 20 (DDR_CR20)
 */
#define KINETIS_DDR_CR20_CKSRX_BITS	16
#define KINETIS_DDR_CR20_CKSRE_BITS	8
/*
 * DDR Control Register 21 (DDR_CR21)
 */
#define KINETIS_DDR_CR21_MR1DAT0_BITS	16
#define KINETIS_DDR_CR21_MR0DAT0_BITS	0
/*
 * DDR Control Register 22 (DDR_CR22)
 */
#define KINETIS_DDR_CR22_MR3DAT0_BITS	16
#define KINETIS_DDR_CR22_MR2DAT0_BITS	0
/*
 * DDR Control Register 25 (DDR_CR25)
 */
#define KINETIS_DDR_CR25_APREBIT_BITS	24
#define KINETIS_DDR_CR25_COLSIZ_BITS	16
#define KINETIS_DDR_CR25_ADDPINS_BITS	8
#define KINETIS_DDR_CR25_BNK8_MSK	(1 << 0)
/*
 * DDR Control Register 26 (DDR_CR26)
 */
#define KINETIS_DDR_CR26_BNKSPT_MSK	(1 << 24)
#define KINETIS_DDR_CR26_ADDCOL_MSK	(1 << 16)
#define KINETIS_DDR_CR26_CMDAGE_BITS	8
#define KINETIS_DDR_CR26_AGECNT_BITS	0
/*
 * DDR Control Register 27 (DDR_CR27)
 */
#define KINETIS_DDR_CR27_SWPEN_MSK	(1 << 24)
#define KINETIS_DDR_CR27_RWEN_MSK	(1 << 16)
#define KINETIS_DDR_CR27_PRIEN_MSK	(1 << 8)
#define KINETIS_DDR_CR27_PLEN_MSK	(1 << 0)
/*
 * DDR Control Register 28 (DDR_CR28)
 */
#define KINETIS_DDR_CR28_CSMAP_MSK	(1 << 0)
/*
 * DDR Control Register 30 (DDR_CR30)
 */
#define KINETIS_DDR_CR30_INTSTAT_DRAMINIT_MSK	(1 << 10)
#define KINETIS_DDR_CR30_RSYNCRF_MSK		(1 << 0)
/*
 * DDR Control Register 34 (DDR_CR34)
 */
#define KINETIS_DDR_CR34_ODTWRCS_MSK	(1 << 8)
#define KINETIS_DDR_CR34_ODTRDC_MSK	(1 << 0)
/*
 * DDR Control Register 37 (DDR_CR37)
 */
#define KINETIS_DDR_CR37_R2WSAME_BITS	8
/*
 * DDR Control Register 38 (DDR_CR38)
 */
#define KINETIS_DDR_CR38_P0WRCNT_BITS	16
/*
 * DDR Control Register 39 (DDR_CR39)
 */
#define KINETIS_DDR_CR39_WP0_BITS	24
#define KINETIS_DDR_CR39_RP0_BITS	16
#define KINETIS_DDR_CR39_P0RDCNT_BITS	0
/*
 * DDR Control Register 40 (DDR_CR40)
 */
#define KINETIS_DDR_CR40_P1WRCNT_BITS	8
#define KINETIS_DDR_CR40_P0TYP_BITS	0
/*
 * DDR Control Register 41 (DDR_CR41)
 */
#define KINETIS_DDR_CR41_WP1_BITS	24
#define KINETIS_DDR_CR41_RP1_BITS	16
#define KINETIS_DDR_CR41_P1RDCNT_BITS	0
/*
 * DDR Control Register 42 (DDR_CR42)
 */
#define KINETIS_DDR_CR42_P2WRCNT_BITS	8
#define KINETIS_DDR_CR42_P1TYP_BITS	0
/*
 * DDR Control Register 43 (DDR_CR43)
 */
#define KINETIS_DDR_CR43_WP2_BITS	24
#define KINETIS_DDR_CR43_RP2_BITS	16
#define KINETIS_DDR_CR43_P2RDCNT_BITS	0
/*
 * DDR Control Register 44 (DDR_CR44)
 */
#define KINETIS_DDR_CR44_P2TYP_BITS	0
/*
 * DDR Control Register 45 (DDR_CR45)
 */
#define KINETIS_DDR_CR45_P0PRI3_BITS	24
#define KINETIS_DDR_CR45_P0PRI2_BITS	16
#define KINETIS_DDR_CR45_P0PRI1_BITS	8
#define KINETIS_DDR_CR45_P0PRI0_BITS	0
/*
 * DDR Control Register 46 (DDR_CR46)
 */
#define KINETIS_DDR_CR46_P1PRI0_BITS	24
#define KINETIS_DDR_CR46_P0PRIRLX_BITS	8
#define KINETIS_DDR_CR46_P0ORD_BITS	0
/*
 * DDR Control Register 47 (DDR_CR47)
 */
#define KINETIS_DDR_CR47_P1ORD_BITS	24
#define KINETIS_DDR_CR47_P1PRI3_BITS	16
#define KINETIS_DDR_CR47_P1PRI2_BITS	8
#define KINETIS_DDR_CR47_P1PRI1_BITS	0
/*
 * DDR Control Register 48 (DDR_CR48)
 */
#define KINETIS_DDR_CR48_P2PRI1_BITS	24
#define KINETIS_DDR_CR48_P2PRI0_BITS	16
#define KINETIS_DDR_CR48_P1PRIRLX_BITS	0
/*
 * DDR Control Register 49 (DDR_CR49)
 */
#define KINETIS_DDR_CR49_P2ORD_BITS	16
#define KINETIS_DDR_CR49_P2PRI3_BITS	8
#define KINETIS_DDR_CR49_P2PRI2_BITS	0
/*
 * DDR Control Register 50 (DDR_CR50)
 */
#define KINETIS_DDR_CR50_P2PRIRLX_BITS		0
/*
 * DDR Control Register 52 (DDR_CR52)
 */
#define KINETIS_DDR_CR52_RDDTENBAS_BITS		24
#define KINETIS_DDR_CR52_PHYRDLAT_BITS		8
#define KINETIS_DDR_CR52_PYWRLTBS_BITS		0
/*
 * DDR Control Register 53 (DDR_CR53)
 */
#define KINETIS_DDR_CR53_CTRLUPDMX_BITS		16
/*
 * DDR Control Register 54 (DDR_CR54)
 */
#define KINETIS_DDR_CR54_PHYUPDTY1_BITS		16
#define KINETIS_DDR_CR54_PHYUPDTY0_BITS		0
/*
 * DDR Control Register 55 (DDR_CR55)
 */
#define KINETIS_DDR_CR55_PHYUPDTY3_BITS		16
#define KINETIS_DDR_CR55_PHYUPDTY2_BITS		0
/*
 * DDR Control Register 56 (DDR_CR56)
 */
#define KINETIS_DDR_CR56_WRLATADJ_BITS		24
#define KINETIS_DDR_CR56_RDLATADJ_BITS		16
#define KINETIS_DDR_CR56_PHYUPDRESP_BITS	0
/*
 * DDR Control Register 57 (DDR_CR57)
 */
#define KINETIS_DDR_CR57_ODTALTEN_MSK		(1 << 24)
#define KINETIS_DDR_CR57_CLKENDLY_BITS		16
#define KINETIS_DDR_CR57_CMDDLY_BITS		0
/*
 * RCR Control Register (DDR_RCR)
 */
#define KINETIS_DDR_RCR_RST_MSK		(1 << 30)
/*
 * I/O Pad Control Register (DDR_PAD_CTRL)
 */
/* Required to enable ODT and configure ODT resistor value in the pad */
#define KINETIS_DDR_PAD_CTRL_PAD_ODT_CS0_BITS	24
/* 75 Ohms */
#define KINETIS_DDR_PAD_CTRL_CS0_75 \
	(1 << KINETIS_DDR_PAD_CTRL_PAD_ODT_CS0_BITS)
/* SPARE_DLY_CTRL[1:0] is used to control the delay chain #0 */
#define KINETIS_DDR_PAD_CTRL_SPARE_DLY_CTRL_BITS	0
/* 10 buffers */
#define KINETIS_DDR_PAD_CTRL_DELAY_10BUF \
	(3 << KINETIS_DDR_PAD_CTRL_SPARE_DLY_CTRL_BITS)
/* This bit should always be written into PAD_CTRL (undocumented feature) */
#define KINETIS_DDR_PAD_CTRL_FIXED_MSK		(1 << 9)

/*
 * Miscellaneous Control Module (MCM) register map
 */
struct kinetis_mcm_regs {
	u32 rsv0[2];
	u16 plasc;	/* Crossbar switch (AXBS) slave configuration */
	u16 plamc;	/* Crossbar switch (AXBS) master configuration */
	u32 cr;		/* Control register */
};

/*
 * MCM registers base
 */
#define KINETIS_MCM_BASE		0xE0080000
#define KINETIS_MCM			((volatile struct kinetis_mcm_regs *) \
					KINETIS_MCM_BASE)

/*
 * Control register (MCM_CR)
 */
/* DDR address size translation */
#define KINETIS_MCM_CR_DDRSIZE_BITS	20
/* DDR size is 128 Mbytes */
#define KINETIS_MCM_CR_DDRSIZE_128MB	(1 << KINETIS_MCM_CR_DDRSIZE_BITS)

/*
 * GPIO pin configuration table for K70-SOM
 *
 * This table does not list all GPIO pins that will be configured. See also
 * the code in `gpio_init()` and in the drivers (for example, the UART driver).
 */
static const struct kinetis_gpio_pin_config k70_som_gpio[] = {
#ifdef CONFIG_MCFFEC
	/* A.5 = RMII0_RXER */
	/*
	 * The pull-down resistor is not really necessary here, because this
	 * pin is always connected to the PHY (it is not connected by default
	 * on the TWR-K60N512 board.)
	 */
	{{KINETIS_GPIO_PORT_A,  5}, KINETIS_GPIO_CONFIG_PULLDOWN(4)},
	/* A.12 = RMII0_RXD1 */
	{{KINETIS_GPIO_PORT_A, 12}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.13 = RMII0_RXD0 */
	{{KINETIS_GPIO_PORT_A, 13}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.14 = RMII0_CRS_DV */
	{{KINETIS_GPIO_PORT_A, 14}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.15 = RMII0_TXEN */
	{{KINETIS_GPIO_PORT_A, 15}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.16 = RMII0_TXD0 */
	{{KINETIS_GPIO_PORT_A, 16}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.17 = RMII0_TXD1 */
	{{KINETIS_GPIO_PORT_A, 17}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* B.0 = RMII0_MDIO */
	{{KINETIS_GPIO_PORT_B,  0}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* B.1 = RMII0_MDC */
	{{KINETIS_GPIO_PORT_B,  1}, KINETIS_GPIO_CONFIG_MUX(4)},
#endif /* CONFIG_MCFFEC */

#ifdef CONFIG_CMD_NAND
	/* C.17 = NFC_CE0 (our flash is on chip select 0) */
	{{KINETIS_GPIO_PORT_C, 17}, KINETIS_GPIO_CONFIG_DSE(6)},

	/*
	 * NAND Flash control pins
	 */
	/* C.16 = NFC_RB */
	{{KINETIS_GPIO_PORT_C, 16}, KINETIS_GPIO_CONFIG_DSE(6)},
	/* D.8 = NFC_CLE */
	{{KINETIS_GPIO_PORT_D,  8}, KINETIS_GPIO_CONFIG_DSE(6)},
	/* D.9 = NFC_ALE */
	{{KINETIS_GPIO_PORT_D,  9}, KINETIS_GPIO_CONFIG_DSE(6)},
	/* D.10 = NFC_RE */
	{{KINETIS_GPIO_PORT_D, 10}, KINETIS_GPIO_CONFIG_DSE(6)},
	/* C.11 = NFC_WE */
	{{KINETIS_GPIO_PORT_C, 11}, KINETIS_GPIO_CONFIG_DSE(5)},

	/*
	 * NAND Flash data pins (the flash is 16-bit)
	 */
	/* D.5 = NFC_D0 */
	{{KINETIS_GPIO_PORT_D,  5}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* D.4 = NFC_D1 */
	{{KINETIS_GPIO_PORT_D,  4}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.10 = NFC_D2 */
	{{KINETIS_GPIO_PORT_C, 10}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.9 = NFC_D3 */
	{{KINETIS_GPIO_PORT_C,  9}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.8 = NFC_D4 */
	{{KINETIS_GPIO_PORT_C,  8}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.7 = NFC_D5 */
	{{KINETIS_GPIO_PORT_C,  7}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.6 = NFC_D6 */
	{{KINETIS_GPIO_PORT_C,  6}, KINETIS_GPIO_CONFIG_DSE(5)},
	/* C.5 = NFC_D7 */
	{{KINETIS_GPIO_PORT_C,  5}, KINETIS_GPIO_CONFIG_DSE(5)},
#endif /* CONFIG_CMD_NAND */
};

/*
 * Configure all necessary GPIO pins
 */
static void gpio_init(void)
{
	/*
	 * Configure GPIO pins using the `k70_som_gpio[]` table
	 */
	kinetis_gpio_config_table(
		k70_som_gpio, ARRAY_SIZE(k70_som_gpio));
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	/*
	 * Enable GPIO pins
	 */
	gpio_init();

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: K70-SOM Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * Configure board specific parts.
 */
int misc_init_r(void)
{
	/* TBD */
	return 0;
}

/*
 * DRAM Port 0 configuration
 */
#define KINETIS_DDR_RP0		1
#define KINETIS_DDR_WP0		1
#define KINETIS_DDR_P0RDCNT	32
#define KINETIS_DDR_P0WRCNT	32
#define KINETIS_DDR_P0ORD	1
#define KINETIS_DDR_P0PRI0	3
#define KINETIS_DDR_P0PRI1	3
#define KINETIS_DDR_P0PRI2	3
#define KINETIS_DDR_P0PRI3	3
#define KINETIS_DDR_P0PRIRLX	100

/*
 * DRAM Port 1 configuration
 */
#define KINETIS_DDR_RP1		1
#define KINETIS_DDR_WP1		1
#define KINETIS_DDR_P1RDCNT	32
#define KINETIS_DDR_P1WRCNT	32
#define KINETIS_DDR_P1ORD	1
#define KINETIS_DDR_P1PRI0	2
#define KINETIS_DDR_P1PRI1	2
#define KINETIS_DDR_P1PRI2	2
#define KINETIS_DDR_P1PRI3	2
#define KINETIS_DDR_P1PRIRLX	100

/*
 * DRAM Port 2 configuration
 */
#define KINETIS_DDR_RP2		1
#define KINETIS_DDR_WP2		1
#define KINETIS_DDR_P2RDCNT	32
#define KINETIS_DDR_P2WRCNT	32
#define KINETIS_DDR_P2ORD	2
#define KINETIS_DDR_P2PRI0	1
#define KINETIS_DDR_P2PRI1	1
#define KINETIS_DDR_P2PRI3	1
#define KINETIS_DDR_P2PRI2	1
#define KINETIS_DDR_P2PRIRLX	100

/*
 * Port types: synchronous, asynchronous.
 * See DDR registers bit fields: CR40[P0TYP], CR42[P1TYP], CR44[P2TYP].
 */
#define KINETIS_DDR_PORT_ASYNC	0
#define KINETIS_DDR_PORT_SYNC	3

/*
 * Actual port types
 */
#ifdef CONFIG_KINETIS_DDR_SYNC
#define KINETIS_DDR_P0TYP	KINETIS_DDR_PORT_SYNC
#define KINETIS_DDR_P1TYP	KINETIS_DDR_PORT_SYNC
#define KINETIS_DDR_P2TYP	KINETIS_DDR_PORT_SYNC
#else
#define KINETIS_DDR_P0TYP	KINETIS_DDR_PORT_ASYNC
#define KINETIS_DDR_P1TYP	KINETIS_DDR_PORT_ASYNC
#define KINETIS_DDR_P2TYP	KINETIS_DDR_PORT_ASYNC
#endif /* CONFIG_KINETIS_DDR_SYNC */

/*
 * Other SDRAM configuration options
 */
#define KINETIS_DDR_INITAREF		2
#define KINETIS_DDR_TINIT		50
#define KINETIS_DDR_TCCD		2
#define KINETIS_DDR_WRLAT		1
#define KINETIS_DDR_LATGATE	        6
#define KINETIS_DDR_LATLIN		6
#define KINETIS_DDR_TRASMIN	        5
#define KINETIS_DDR_TRC			7
#define KINETIS_DDR_TRRD		2
#define KINETIS_DDR_TBINT		1
#define KINETIS_DDR_TMRD		2
#define KINETIS_DDR_TRTP		2
#define KINETIS_DDR_TRP			2
#define KINETIS_DDR_TWTR		2
#define KINETIS_DDR_TRASMAX		8400	/* 70 us */
#define KINETIS_DDR_TMOD		0
#define KINETIS_DDR_TCKESR		1
#define KINETIS_DDR_CLKPW		1
#define KINETIS_DDR_TDAL		4
#define KINETIS_DDR_TWR			2
#define KINETIS_DDR_TRASDI		2
#define KINETIS_DDR_TDLL		0
#define KINETIS_DDR_TRPAB		2
#define KINETIS_DDR_TCPD		24000
#define KINETIS_DDR_TFAW		0
#define KINETIS_DDR_TREF		936	/* 7.8 us */
#define KINETIS_DDR_TRFC		12
#define KINETIS_DDR_TXSR		15
#define KINETIS_DDR_TPDEX		2
#define KINETIS_DDR_TXSNR		15
#define KINETIS_DDR_CKSRX		3
#define KINETIS_DDR_CKSRE		3

#define KINETIS_DDR_APREBIT		10
#define KINETIS_DDR_COLSIZ		1
#define KINETIS_DDR_ADDPINS		3

#define KINETIS_DDR_CMDAGE		255
#define KINETIS_DDR_AGECNT		255
#define KINETIS_DDR_R2WSAME		2
#define KINETIS_DDR_RDDTENBAS		2
#define KINETIS_DDR_PHYRDLAT		6
#define KINETIS_DDR_PYWRLTBS		2
#define KINETIS_DDR_CTRLUPDMX		968
#define KINETIS_DDR_PHYUPDTY1		968
#define KINETIS_DDR_PHYUPDTY0		968
#define KINETIS_DDR_PHYUPDTY3		968
#define KINETIS_DDR_PHYUPDTY2		968
#define KINETIS_DDR_WRLATADJ		2
#define KINETIS_DDR_RDLATADJ		3
#define KINETIS_DDR_PHYUPDRESP		968
#define KINETIS_DDR_CLKENDLY		1
#define KINETIS_DDR_CMDDLY		2

/*
 * LPDDR memory chip configuration options
 */
/* CAS Latency (CL) = 3 */
#define LPDDR_MR_CL		3
/* Burst Type: 0=Sequential */
#define LPDDR_MR_BURST_TYPE	0
/* Burst Length: 8 memory words */
#define LPDDR_MR_BL		3

/* Driver Strength: Half-strength */
#define LPDDR_EMR_STRENGTH	1
/* Partial-Array Self Refresh (PASR): Full array */
#define LPDDR_EMR_PASR		0

/*
 * Micron MT46H[...] LPDDR memory chip mode registers
 */
/* Mode Register (MR) */
#define KINETIS_DDR_MR0DAT0 \
	((LPDDR_MR_CL << 4) | (LPDDR_MR_BURST_TYPE << 3) | (LPDDR_MR_BL << 0))
/* Status Register */
#define KINETIS_DDR_MR1DAT0		0
/* Extended Mode Register (EMR2) */
#define KINETIS_DDR_MR2DAT0 \
	((LPDDR_EMR_STRENGTH << 5) | (LPDDR_EMR_PASR << 0))
/* Reserved */
#define KINETIS_DDR_MR3DAT0		0

/*
 * Setup external RAM.
 */
int dram_init(void)
{
#ifdef CONFIG_KINETIS_DDR
	/*
	 * Enable the clock on the DDR module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_DDR, 1);

	/*
	 * Enable DDR pads and set slew rate
	 */
	KINETIS_SIM->mcr =
		KINETIS_SIM_MCR_DDRCFG_LPDDR_HALF | KINETIS_SIM_MCR_DDRPEN_MSK;

	/*
	 * RCR reset
	 */
	KINETIS_DDR->rcr |= KINETIS_DDR_RCR_RST_MSK;

	/*
	 * Configure LPDDR SDRAM
	 */
	KINETIS_DDR->pad_ctrl =
		KINETIS_DDR_PAD_CTRL_FIXED_MSK |
		KINETIS_DDR_PAD_CTRL_DELAY_10BUF;
	KINETIS_DDR->cr[0] = KINETIS_DDR_CR00_DDRCLS_LPDDR;
	KINETIS_DDR->cr[2] =
		(KINETIS_DDR_INITAREF << KINETIS_DDR_CR02_INITAREF_BITS) |
		(KINETIS_DDR_TINIT << KINETIS_DDR_CR02_TINIT_BITS);
	KINETIS_DDR->cr[3] =
		(KINETIS_DDR_TCCD << KINETIS_DDR_CR03_TCCD_BITS) |
		(KINETIS_DDR_WRLAT << KINETIS_DDR_CR03_WRLAT_BITS) |
		(KINETIS_DDR_LATGATE << KINETIS_DDR_CR03_LATGATE_BITS) |
		(KINETIS_DDR_LATLIN << KINETIS_DDR_CR03_LATLIN_BITS);
	KINETIS_DDR->cr[4] =
		(KINETIS_DDR_TRASMIN << KINETIS_DDR_CR04_TRASMIN_BITS) |
		(KINETIS_DDR_TRC << KINETIS_DDR_CR04_TRC_BITS) |
		(KINETIS_DDR_TRRD << KINETIS_DDR_CR04_TRRD_BITS) |
		(KINETIS_DDR_TBINT << KINETIS_DDR_CR04_TBINT_BITS);
	KINETIS_DDR->cr[5] =
		(KINETIS_DDR_TMRD << KINETIS_DDR_CR05_TMRD_BITS) |
		(KINETIS_DDR_TRTP << KINETIS_DDR_CR05_TRTP_BITS) |
		(KINETIS_DDR_TRP << KINETIS_DDR_CR05_TRP_BITS) |
		(KINETIS_DDR_TWTR << KINETIS_DDR_CR05_TWTR_BITS);
	KINETIS_DDR->cr[6] =
		(KINETIS_DDR_TRASMAX << KINETIS_DDR_CR06_TRASMAX_BITS) |
		(KINETIS_DDR_TMOD << KINETIS_DDR_CR06_TMOD_BITS);
	KINETIS_DDR->cr[7] =
		KINETIS_DDR_CR07_CCAPEN_MSK |
		(KINETIS_DDR_TCKESR << KINETIS_DDR_CR07_TCKESR_BITS) |
		(KINETIS_DDR_CLKPW << KINETIS_DDR_CR07_CLKPW_BITS);
	KINETIS_DDR->cr[8] =
		(KINETIS_DDR_TDAL << KINETIS_DDR_CR07_TDAL_BITS) |
		(KINETIS_DDR_TWR << KINETIS_DDR_CR07_TWR_BITS) |
		(KINETIS_DDR_TRASDI << KINETIS_DDR_CR07_TRASDI_BITS);
	KINETIS_DDR->cr[9] =
		KINETIS_DDR_CR09_BSTLEN_8W |
		(KINETIS_DDR_TDLL << KINETIS_DDR_CR09_TDLL_BITS);
	KINETIS_DDR->cr[10] =
		(KINETIS_DDR_TRPAB << KINETIS_DDR_CR10_TRPAB_BITS) |
		(KINETIS_DDR_TCPD << KINETIS_DDR_CR10_TCPD_BITS) |
		(KINETIS_DDR_TFAW << KINETIS_DDR_CR10_TFAW_BITS);
	KINETIS_DDR->cr[11] =
		KINETIS_DDR_CR11_TREFEN_MSK;
	KINETIS_DDR->cr[12] =
		(KINETIS_DDR_TREF << KINETIS_DDR_CR12_TREF_BITS) |
		(KINETIS_DDR_TRFC << KINETIS_DDR_CR12_TRFC_BITS);
	KINETIS_DDR->cr[14] =
		(KINETIS_DDR_TXSR << KINETIS_DDR_CR14_TXSR_BITS) |
		(KINETIS_DDR_TPDEX << KINETIS_DDR_CR14_TPDEX_BITS);
	KINETIS_DDR->cr[15] =
		(KINETIS_DDR_TXSNR << KINETIS_DDR_CR15_TXSNR_BITS);
	KINETIS_DDR->cr[16] = KINETIS_DDR_CR16_QKREF_MSK;
	KINETIS_DDR->cr[20] =
		(KINETIS_DDR_CKSRX << KINETIS_DDR_CR20_CKSRX_BITS) |
		(KINETIS_DDR_CKSRE << KINETIS_DDR_CR20_CKSRE_BITS);
	KINETIS_DDR->cr[21] =
		(KINETIS_DDR_MR1DAT0 << KINETIS_DDR_CR21_MR1DAT0_BITS) |
		(KINETIS_DDR_MR0DAT0 << KINETIS_DDR_CR21_MR0DAT0_BITS);
	KINETIS_DDR->cr[22] =
		(KINETIS_DDR_MR3DAT0 << KINETIS_DDR_CR22_MR3DAT0_BITS) |
		(KINETIS_DDR_MR2DAT0 << KINETIS_DDR_CR22_MR2DAT0_BITS);
	KINETIS_DDR->cr[25] =
		(KINETIS_DDR_APREBIT << KINETIS_DDR_CR25_APREBIT_BITS) |
		(KINETIS_DDR_COLSIZ << KINETIS_DDR_CR25_COLSIZ_BITS) |
		(KINETIS_DDR_ADDPINS << KINETIS_DDR_CR25_ADDPINS_BITS);
	KINETIS_DDR->cr[26] =
		KINETIS_DDR_CR26_BNKSPT_MSK |
		KINETIS_DDR_CR26_ADDCOL_MSK |
		(KINETIS_DDR_CMDAGE << KINETIS_DDR_CR26_CMDAGE_BITS) |
		(KINETIS_DDR_AGECNT << KINETIS_DDR_CR26_AGECNT_BITS);
	KINETIS_DDR->cr[27] =
		KINETIS_DDR_CR27_SWPEN_MSK |
		KINETIS_DDR_CR27_RWEN_MSK |
		KINETIS_DDR_CR27_PRIEN_MSK |
		KINETIS_DDR_CR27_PLEN_MSK;
	KINETIS_DDR->cr[28] = KINETIS_DDR_CR28_CSMAP_MSK;
	KINETIS_DDR->cr[29] = 0;
	KINETIS_DDR->cr[30] = 0;
	KINETIS_DDR->cr[34] = 0;
	KINETIS_DDR->cr[37] =
		(KINETIS_DDR_R2WSAME << KINETIS_DDR_CR37_R2WSAME_BITS);
	KINETIS_DDR->cr[38] =
		(KINETIS_DDR_P0WRCNT << KINETIS_DDR_CR38_P0WRCNT_BITS);
	KINETIS_DDR->cr[39] =
		(KINETIS_DDR_WP0 << KINETIS_DDR_CR39_WP0_BITS) |
		(KINETIS_DDR_RP0 << KINETIS_DDR_CR39_RP0_BITS) |
		(KINETIS_DDR_P0RDCNT << KINETIS_DDR_CR39_P0RDCNT_BITS);
	KINETIS_DDR->cr[40] =
		(KINETIS_DDR_P1WRCNT << KINETIS_DDR_CR40_P1WRCNT_BITS) |
		(KINETIS_DDR_P0TYP << KINETIS_DDR_CR40_P0TYP_BITS);
	KINETIS_DDR->cr[41] =
		(KINETIS_DDR_WP1 << KINETIS_DDR_CR41_WP1_BITS) |
		(KINETIS_DDR_RP1 << KINETIS_DDR_CR41_RP1_BITS) |
		(KINETIS_DDR_P1RDCNT << KINETIS_DDR_CR41_P1RDCNT_BITS);
	KINETIS_DDR->cr[42] =
		(KINETIS_DDR_P2WRCNT << KINETIS_DDR_CR42_P2WRCNT_BITS) |
		(KINETIS_DDR_P1TYP << KINETIS_DDR_CR42_P1TYP_BITS);
	KINETIS_DDR->cr[43] =
		(KINETIS_DDR_WP2 << KINETIS_DDR_CR43_WP2_BITS) |
		(KINETIS_DDR_RP2 << KINETIS_DDR_CR43_RP2_BITS) |
		(KINETIS_DDR_P2RDCNT << KINETIS_DDR_CR43_P2RDCNT_BITS);
	KINETIS_DDR->cr[44] =
		(KINETIS_DDR_P2TYP << KINETIS_DDR_CR44_P2TYP_BITS);
	KINETIS_DDR->cr[45] =
		(KINETIS_DDR_P0PRI3 << KINETIS_DDR_CR45_P0PRI3_BITS) |
		(KINETIS_DDR_P0PRI2 << KINETIS_DDR_CR45_P0PRI2_BITS) |
		(KINETIS_DDR_P0PRI1 << KINETIS_DDR_CR45_P0PRI1_BITS) |
		(KINETIS_DDR_P0PRI0 << KINETIS_DDR_CR45_P0PRI0_BITS);
	KINETIS_DDR->cr[46] =
		(KINETIS_DDR_P1PRI0 << KINETIS_DDR_CR46_P1PRI0_BITS) |
		(KINETIS_DDR_P0PRIRLX << KINETIS_DDR_CR46_P0PRIRLX_BITS) |
		(KINETIS_DDR_P0ORD << KINETIS_DDR_CR46_P0ORD_BITS);
	KINETIS_DDR->cr[47] =
		(KINETIS_DDR_P1ORD << KINETIS_DDR_CR47_P1ORD_BITS) |
		(KINETIS_DDR_P1PRI3 << KINETIS_DDR_CR47_P1PRI3_BITS) |
		(KINETIS_DDR_P1PRI2 << KINETIS_DDR_CR47_P1PRI2_BITS) |
		(KINETIS_DDR_P1PRI1 << KINETIS_DDR_CR47_P1PRI1_BITS);
	KINETIS_DDR->cr[48] =
		(KINETIS_DDR_P2PRI1 << KINETIS_DDR_CR48_P2PRI1_BITS) |
		(KINETIS_DDR_P2PRI0 << KINETIS_DDR_CR48_P2PRI0_BITS) |
		(KINETIS_DDR_P1PRIRLX << KINETIS_DDR_CR48_P1PRIRLX_BITS);
	KINETIS_DDR->cr[49] =
		(KINETIS_DDR_P2ORD << KINETIS_DDR_CR49_P2ORD_BITS) |
		(KINETIS_DDR_P2PRI3 << KINETIS_DDR_CR49_P2PRI3_BITS) |
		(KINETIS_DDR_P2PRI2 << KINETIS_DDR_CR49_P2PRI2_BITS);
	KINETIS_DDR->cr[50] =
		(KINETIS_DDR_P2PRIRLX << KINETIS_DDR_CR50_P2PRIRLX_BITS);
	KINETIS_DDR->cr[52] =
		(KINETIS_DDR_RDDTENBAS << KINETIS_DDR_CR52_RDDTENBAS_BITS) |
		(KINETIS_DDR_PHYRDLAT << KINETIS_DDR_CR52_PHYRDLAT_BITS) |
		(KINETIS_DDR_PYWRLTBS << KINETIS_DDR_CR52_PYWRLTBS_BITS);
	KINETIS_DDR->cr[53] =
		(KINETIS_DDR_CTRLUPDMX << KINETIS_DDR_CR53_CTRLUPDMX_BITS);
	KINETIS_DDR->cr[54] =
		(KINETIS_DDR_PHYUPDTY1 << KINETIS_DDR_CR54_PHYUPDTY1_BITS) |
		(KINETIS_DDR_PHYUPDTY0 << KINETIS_DDR_CR54_PHYUPDTY0_BITS);
	KINETIS_DDR->cr[55] =
		(KINETIS_DDR_PHYUPDTY3 << KINETIS_DDR_CR55_PHYUPDTY3_BITS) |
		(KINETIS_DDR_PHYUPDTY2 << KINETIS_DDR_CR55_PHYUPDTY2_BITS);
	KINETIS_DDR->cr[56] =
		(KINETIS_DDR_WRLATADJ << KINETIS_DDR_CR56_WRLATADJ_BITS) |
		(KINETIS_DDR_RDLATADJ << KINETIS_DDR_CR56_RDLATADJ_BITS) |
		(KINETIS_DDR_PHYUPDRESP << KINETIS_DDR_CR56_PHYUPDRESP_BITS);
	KINETIS_DDR->cr[57] =
		(KINETIS_DDR_CLKENDLY << KINETIS_DDR_CR57_CLKENDLY_BITS) |
		(KINETIS_DDR_CMDDLY << KINETIS_DDR_CR57_CMDDLY_BITS);

	asm("nop");

	/*
	 * Start command processing in the memory controller
	 */
	KINETIS_DDR->cr[0] |= KINETIS_DDR_CR00_START_MSK;

	/*
	 * Wait for the DRAM to finish initialization
	 */
	while (!(KINETIS_DDR->cr[30] & KINETIS_DDR_CR30_INTSTAT_DRAMINIT_MSK));

	/*
	 * DDR address size translation
	 */
	KINETIS_MCM->cr |= KINETIS_MCM_CR_DDRSIZE_128MB;

	/*
	 * Fill in global info with description of DRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;
#endif /* CONFIG_KINETIS_DDR */

	return 0;
}

#ifdef CONFIG_MCFFEC
/*
 * Register the Ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return mcffec_initialize(bis);
}
#endif

