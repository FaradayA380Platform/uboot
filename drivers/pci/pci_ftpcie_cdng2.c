/*
 * Driver for the Cadence PCI-E Gen2 root complex controller
 *
 *  (C) Copyright 2015 Faraday Technology
 *  Mark Fu-Tsung <mark_hs@faraday-tech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <pci.h>

#include <faraday/ftpcie_cdng2.h>

#include <asm/io.h>
#include <asm/types.h> /* u32, u16.... used by pci.h */

#define REGION0 0
#define REGION1 1
#define REGION2 2

struct ftpcie_cdng2_data {
	unsigned int index;
	void *sysc_base;
	void *pcie_axislave_base;
	void *pcie_glue_base;
	void *pcie_reg_base;
	void *pcie_reg_local_mgmt_base;
	void *pcie_reg_addr_trans_base;
	void *pcie_sedes_base;
};

static int num_pcie_ports = 0;

void ConfigAddrTrans(unsigned int id, unsigned int no_of_valid_bits, unsigned int trans_type, void *pcie_addr_trans_base, int region)
{
//	printf("%s pcie_addr_trans_base=0x%p\n", __func__, pcie_addr_trans_base);

	if(region == 0){
		if(id == 0)
			writel(0x10000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION0_ADDR0);
		else{
			if(num_pcie_ports == 1)
				writel(0x20000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION0_ADDR0);
			else if(num_pcie_ports == 2)
				writel(0x20100000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION0_ADDR0);
		}
		writel(0                    , pcie_addr_trans_base + REGION0_ADDR1);
		writel((1<<23)|trans_type   , pcie_addr_trans_base + REGION0_DESC0);
		writel(0                    , pcie_addr_trans_base + REGION0_DESC1);
		writel(0                    , pcie_addr_trans_base + REGION0_DESC2);
	}else if(region == 1){
		if(id == 0)
			writel(0x11000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION1_ADDR0);
		else	{
			if(num_pcie_ports == 1)
				writel(0x21000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION1_ADDR0);
			else if(num_pcie_ports == 2)
				writel(0x21100000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION1_ADDR0);
		}
		writel(0                    , pcie_addr_trans_base + REGION1_ADDR1);
		writel((1<<23)|trans_type   , pcie_addr_trans_base + REGION1_DESC0);
		writel(0                    , pcie_addr_trans_base + REGION1_DESC1);
		writel(0                    , pcie_addr_trans_base + REGION1_DESC2);

		if(id == 0)
			writel(0x12000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION2_ADDR0);
		else{
			if(num_pcie_ports == 1)
				writel(0x22000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION2_ADDR0);
			else if(num_pcie_ports == 2)
				writel(0x22100000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION2_ADDR0);
		}
		writel(0                    , pcie_addr_trans_base + REGION2_ADDR1);
		writel((1<<23)|trans_type   , pcie_addr_trans_base + REGION2_DESC0);
		writel(0                    , pcie_addr_trans_base + REGION2_DESC1);
		writel(0                    , pcie_addr_trans_base + REGION2_DESC2);

		if(id == 0)
			writel(0x13000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION3_ADDR0);
		else	{
			if(num_pcie_ports == 1)
				writel(0x23000000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION3_ADDR0);
			else if(num_pcie_ports ==2)
				writel(0x23100000|(no_of_valid_bits-1), pcie_addr_trans_base + REGION3_ADDR0);
		}
		writel(0                    , pcie_addr_trans_base + REGION3_ADDR1);
		writel((1<<23)|trans_type   , pcie_addr_trans_base + REGION3_DESC0);
		writel(0                    , pcie_addr_trans_base + REGION3_DESC1);
		writel(0                    , pcie_addr_trans_base + REGION3_DESC2);
	}
}

void ConfigAddrTrans_Config(struct ftpcie_cdng2_data *priv)
{
	unsigned int no_of_valid_bits, trans_type;

//	printf("configaddrtransconfig start\n");

	no_of_valid_bits = 20;	//	28;
	trans_type = 0xa;

	ConfigAddrTrans(priv->index, no_of_valid_bits, trans_type, priv->pcie_reg_addr_trans_base, REGION0);
}

void ConfigAddrTrans_Mem(struct ftpcie_cdng2_data *priv)
{
	unsigned int no_of_valid_bits, trans_type;

//	printf("configaddrtransmem start\n");

	no_of_valid_bits = 26;	//	20;
	trans_type = 0x2;

	ConfigAddrTrans(priv->index, no_of_valid_bits, trans_type, priv->pcie_reg_addr_trans_base, REGION1);
}

static int pci_ftpcie_cdng2_read_config(struct udevice *bus, pci_dev_t bdf,
	                                    uint offset, ulong *valuep, enum pci_size_t size)
{
	struct ftpcie_cdng2_data *priv = dev_get_priv(bus);
	unsigned int status = 0;
	unsigned int request_id;
	volatile unsigned int tmp;

	if(bdf) {
		*valuep = 0xFFFF;
		return status;
	}

//	printf("offset = %lx,size = %lx\n", offset, size);

	ConfigAddrTrans_Config(priv);

	tmp = PCI_BUS(bdf)<<20 | PCI_DEV(bdf)<<15 | PCI_FUNC(bdf)<<12;

	request_id = (unsigned int)priv->pcie_axislave_base | tmp;
//	printf("R\n");
	switch (size)
	{
		case PCI_SIZE_8: 
			*valuep = readb(request_id + offset); 
			//printf("1. offset = %08X,\tval = %08X\n", offset , *val);
			break;
		case PCI_SIZE_16: 
			*valuep = readw(request_id + offset);
			//printf("2. offset = %08X,\tval = %08X\n", offset , *val);;
			break;
		case PCI_SIZE_32: 
			*valuep = readl(request_id + offset); 
			//printf("4. offset = %08X,\tval = %08X\n", offset , *val);
			break;
		default: 
			*valuep = 0;
			status = -EINVAL;
			goto rd_config_out;
	}

rd_config_out:
	ConfigAddrTrans_Mem(priv);
//	printf("rdconfig done\n");
	return status;
}

static int pci_ftpcie_cdng2_write_config(struct udevice *bus, pci_dev_t bdf,
                                         uint offset, ulong value, enum pci_size_t size)
{
	struct ftpcie_cdng2_data *priv = dev_get_priv(bus);
	unsigned int status = 0;
	unsigned int request_id;
	unsigned int tmp;

	if(bdf)
		return status;

//	printf("offset = %lx,size = %lx\n", offset, size);

	ConfigAddrTrans_Config(priv);

	tmp = PCI_BUS(bdf)<<20 | PCI_DEV(bdf)<<15 | PCI_FUNC(bdf)<<12;

	request_id = (unsigned int)priv->pcie_axislave_base | tmp;
//	printf("W\n");
	switch (size) 
	{
		case PCI_SIZE_8: 
			writeb((unsigned char)value, request_id + offset); 
			break;
		case PCI_SIZE_16: 
			writew((unsigned long)value, request_id + offset); 
			break;
		case PCI_SIZE_32: 
			writel(value, request_id + offset); 
			break;
		default: 
			status = -EINVAL;
			goto wr_config_out;
	}

wr_config_out:
	ConfigAddrTrans_Mem(priv);
//	printf("wconfig done\n");
	return status;
}

static const struct dm_pci_ops pci_ftpcie_cdng2_ops = {
	.read_config	= pci_ftpcie_cdng2_read_config,
	.write_config	= pci_ftpcie_cdng2_write_config,
};

void SetSerDesPhy(struct ftpcie_cdng2_data *priv)
{
	printf("Faraday PCI driver SetSerDesPhy\n");

	writel(0x28, priv->pcie_glue_base + PCIE_SERDES_CTRL3_OFFSET); // Set pipe datawidth to 32bit

	writel(0x01, priv->pcie_glue_base + PCIE_SERDES_CTRL5_OFFSET); // Deassert PCIe0 serdes Power on reset

	writel(0xBF, priv->pcie_sedes_base + 0x0065*4);
	writel(0x97, priv->pcie_sedes_base + 0x0066*4);
	writel(0x02, priv->pcie_sedes_base + 0x0067*4);
	writel(0x02, priv->pcie_sedes_base + 0x0068*4);
	writel(0x5E, priv->pcie_sedes_base + 0x0069*4);
	writel(0x4A, priv->pcie_sedes_base + 0x006a*4);
	writel(0x01, priv->pcie_sedes_base + 0x006b*4);
	writel(0x01, priv->pcie_sedes_base + 0x006c*4);
	writel(0x01, priv->pcie_sedes_base + 0x006d*4);
	writel(0x05, priv->pcie_sedes_base + 0x006e*4);

	writel(0x88, priv->pcie_sedes_base + 0x8065*4);
	writel(0x00, priv->pcie_sedes_base + 0x8066*4);
	writel(0x0C, priv->pcie_sedes_base + 0x8067*4);
	writel(0xBF, priv->pcie_sedes_base + 0x8068*4);
	writel(0x97, priv->pcie_sedes_base + 0x8069*4);
	writel(0x02, priv->pcie_sedes_base + 0x806a*4);
	writel(0x02, priv->pcie_sedes_base + 0x806b*4);
	writel(0x5E, priv->pcie_sedes_base + 0x806c*4);
	writel(0x4A, priv->pcie_sedes_base + 0x806d*4);
	writel(0x00, priv->pcie_sedes_base + 0x806e*4);
	writel(0x0C, priv->pcie_sedes_base + 0x806f*4);

	writel(0x04, priv->pcie_sedes_base + 0x8070*4);
	writel(0x00, priv->pcie_sedes_base + 0x8071*4);
	writel(0x04, priv->pcie_sedes_base + 0x8072*4);
	writel(0x00, priv->pcie_sedes_base + 0x8073*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8074*4);
	writel(0xF3, priv->pcie_sedes_base + 0x8075*4);
	writel(0xFD, priv->pcie_sedes_base + 0x8076*4);
	writel(0xF7, priv->pcie_sedes_base + 0x8077*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8078*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8079*4);
	writel(0xFC, priv->pcie_sedes_base + 0x807a*4);
	writel(0xFF, priv->pcie_sedes_base + 0x807b*4);
	writel(0xFF, priv->pcie_sedes_base + 0x807c*4);
	writel(0xFF, priv->pcie_sedes_base + 0x807d*4);
	writel(0xFF, priv->pcie_sedes_base + 0x807e*4);
	writel(0xFB, priv->pcie_sedes_base + 0x807f*4);

	writel(0xF1, priv->pcie_sedes_base + 0x8080*4);
	writel(0xE6, priv->pcie_sedes_base + 0x8081*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8082*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8083*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8084*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8085*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8086*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8087*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8088*4);
	writel(0xFB, priv->pcie_sedes_base + 0x8089*4);
	writel(0xF1, priv->pcie_sedes_base + 0x808a*4);
	writel(0xE6, priv->pcie_sedes_base + 0x808b*4);
	writel(0xFC, priv->pcie_sedes_base + 0x808c*4);
	writel(0xFF, priv->pcie_sedes_base + 0x808d*4);
	writel(0xFC, priv->pcie_sedes_base + 0x808e*4);
	writel(0xFC, priv->pcie_sedes_base + 0x808f*4);

	writel(0xFC, priv->pcie_sedes_base + 0x8090*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8091*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8092*4);
	writel(0xFF, priv->pcie_sedes_base + 0x8093*4);
	writel(0xFC, priv->pcie_sedes_base + 0x8094*4);
	writel(0x3F, priv->pcie_sedes_base + 0x8095*4);
	writel(0x00, priv->pcie_sedes_base + 0x8096*4);
	writel(0x3F, priv->pcie_sedes_base + 0x8097*4);
	writel(0x00, priv->pcie_sedes_base + 0x8098*4);
	writel(0x03, priv->pcie_sedes_base + 0x8099*4);
	writel(0x01, priv->pcie_sedes_base + 0x809a*4);
	writel(0x09, priv->pcie_sedes_base + 0x809b*4);
	writel(0x05, priv->pcie_sedes_base + 0x809c*4);
	writel(0x04, priv->pcie_sedes_base + 0x809d*4);
	writel(0x00, priv->pcie_sedes_base + 0x809e*4);
	writel(0x00, priv->pcie_sedes_base + 0x809f*4);

	writel(0x14, priv->pcie_sedes_base + 0x80a0*4);    //ori:0x08->0x14
	writel(0x04, priv->pcie_sedes_base + 0x80a1*4);
	writel(0x00, priv->pcie_sedes_base + 0x80a2*4);
	writel(0x00, priv->pcie_sedes_base + 0x80a3*4);
	writel(0x04, priv->pcie_sedes_base + 0x80a4*4);

	writel(0x22, priv->pcie_sedes_base + 0x80D5*4);
	writel(0x03, priv->pcie_sedes_base + 0x80D1*4);
	writel(0x23, priv->pcie_sedes_base + 0x80D2*4);

	writel(0xFC, priv->pcie_sedes_base + 0x324);
	writel(0xFF, priv->pcie_sedes_base + 0x328);
	writel(0xFF, priv->pcie_sedes_base + 0x32c);
	writel(0xFF, priv->pcie_sedes_base + 0x330);
	writel(0xFF, priv->pcie_sedes_base + 0x334);
}

void cdn_pcie_linkup(struct ftpcie_cdng2_data *priv)
{
	// PCIe PLL Initialization
	writel(0xA11, priv->sysc_base + PCIE_PLL_CTRL_OFFSET); // PCIe PLL enable

	// PCIe CLK Initialization
	writel(0x17, priv->sysc_base + PCIE_CLK_CTRL_OFFSET);   // PCIe core clock gate enable and deassert pcie div rst

	SetSerDesPhy(priv);
}

void ResetCard(struct ftpcie_cdng2_data *priv)
{
	u32 tmp;

	writel(0x07, priv->pcie_glue_base + PCIE_SERDES_CTRL5_OFFSET); // Deassert PCIe0 serdes pipe reset

	// PCIe Controller Reset Deassert
	writel(0x01, priv->pcie_glue_base + PIPE_RST_CTRL_OFFSET);     // Deassert PCIe0 controller PIPE_RESET_N
	writel(0x03, priv->pcie_glue_base + PCIE_RST_CTRL2_OFFSET);    // Deassert PCIe0 controller MGMT_RESET_N and MGMT_STICKY_RESET_N
	writel(0x07, priv->pcie_glue_base + PCIE_RST_CTRL2_OFFSET);    // Deassert PCIe0 controller RESET_N

	tmp = readl(priv->pcie_reg_base+ 0xe8) | 0x10;
	writel(tmp , priv->pcie_reg_base + 0xe8);
}

void EnableLinkTraining(struct ftpcie_cdng2_data *priv)
{
	// Link Training Enable
	writel(0x3, priv->pcie_glue_base + PCIE_SB_CTRL4_OFFSET);
}

void WaitL0Stat(struct ftpcie_cdng2_data *priv)
{
	volatile u32 data;

	printf("Faraday PCI driver Wait Link to L0\n");

	data = readl(priv->pcie_glue_base + PCIE_LINK_STATUS_OFFSET);

	data &= 0x1f0000;

	// Waiting for Linking to L0 State
	while(1) 
	{
		data = readl(priv->pcie_glue_base + PCIE_LINK_STATUS_OFFSET);
		if((data & 0x1f0000) == 0x100000 )  // Ltssm state
			break;
	}

	printf("Faraday PCI driver Link to L0\n");
}

static void ftpcie_cdng2_preinit(struct ftpcie_cdng2_data *priv)
{
	printf("Faraday PCI driver PreInit Controller %d\n", priv->index);

	cdn_pcie_linkup(priv);
	ResetCard(priv);
	EnableLinkTraining(priv);
	WaitL0Stat(priv);

	writel(0x147, priv->pcie_reg_base + CMD_STS_OFFSET);

	//Set BAR SIZE, Width, and cacheable
	writel(1<<31|((0x4<<6)|28), priv->pcie_reg_local_mgmt_base + RC_BAR_CFG_OFFSET);

	writel(0x80000000         , priv->pcie_reg_base + RC_BAR0);

	writel(0x8000001d         , priv->pcie_reg_addr_trans_base + 0x800);
}

static int pci_ftpcie_cdng2_ofdata_to_platdata(struct udevice *dev)
{
	struct ftpcie_cdng2_data *priv = dev_get_priv(dev);

	priv->index = num_pcie_ports++;
	priv->sysc_base = (void *)dev_get_addr_index(dev, 0);
	priv->pcie_axislave_base = (void *)dev_get_addr_index(dev, 1);
	priv->pcie_glue_base = (void *)dev_get_addr_index(dev, 2);
	priv->pcie_reg_base = (void *)dev_get_addr_index(dev, 3);
	priv->pcie_reg_local_mgmt_base = (void *)dev_get_addr_index(dev, 4);
	priv->pcie_reg_addr_trans_base = (void *)dev_get_addr_index(dev, 5);
	priv->pcie_sedes_base = (void *)dev_get_addr_index(dev, 6);

	return 0;
}

static int pci_ftpcie_cdng2_probe(struct udevice *dev)
{
	struct ftpcie_cdng2_data *priv = dev_get_priv(dev);

	ftpcie_cdng2_preinit(priv);

	return 0;
}

static const struct udevice_id pci_ftpcie_cdng2_ids[] = {
	{ .compatible = "faraday,pci_ftpcie_cdng2" },
	{ }
};

U_BOOT_DRIVER(pci_ftpcie_cdng2) = {
	.name = "pci_ftpcie_cdng2",
	.id	= UCLASS_PCI,
	.of_match = pci_ftpcie_cdng2_ids,
	.ofdata_to_platdata = pci_ftpcie_cdng2_ofdata_to_platdata,
	.ops = &pci_ftpcie_cdng2_ops,
	.probe = pci_ftpcie_cdng2_probe,
	.priv_auto_alloc_size = sizeof(struct ftpcie_cdng2_data),
};
