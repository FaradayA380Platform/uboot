#ifndef __PLAT_NAND_H
#define __PLAT_NAND_H

struct platform_nand_flash {
	uint8_t col_addrcycles;
	uint8_t row_addrcycles;
	uint16_t ecc_bits_dat;
	uint16_t ecc_bits_oob;
};

void get_nandc_info_strap(struct nand_chip *chip, struct platform_nand_flash *plat_nand);

#endif