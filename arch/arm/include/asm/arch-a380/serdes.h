#ifndef __SERDESHEADER_H_
#define __SERDESHEADER_H_

struct init_seq {
	unsigned int addr;
	unsigned int value;
};

/* This struct describes R/WDMA descriptor structure */
struct pcie_dma_channel {
	unsigned int ctrl;
	unsigned int status;
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int nxt_addr;
};

//end pcie



typedef struct ethdr {
	uint8_t		dst[6];
	uint8_t		src[6];
	uint16_t	proto;
} __attribute__ ((packed)) ethdr_t;

typedef struct arphdr {
	uint16_t	hwtype;
	uint16_t	proto;
	uint8_t		hsize;
	uint8_t		psize;
	uint16_t	opcode;
	uint8_t		smac[6];
	uint32_t	saddr;
	uint8_t		tmac[6];
	uint32_t	taddr;
} __attribute__ ((packed)) arphdr_t;
#endif
