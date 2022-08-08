struct init_seq pwrup_8gbps_bist[] = {
{0x0002,0x2},
{0x8002,0x1},
{0x0055,0x6F},
{0xe002,0x2},
{0x8002,0x1},
{0xE065,0xb7},
{0xE066,0xb7},
{0xE067,0x6},
{0xE068,0x6},
{0xE069,0x1a},
{0xE06a,0x1a},
{0xE06b,0x1},
{0xE06c,0x1},
{0xE06d,0x33},
{0xE06e,0x5},
{0x8065,0xaa},
{0x8066,0x0},
{0x8067,0x3f},
{0x8068,0xb7},
{0x8069,0xb7},
{0x806a,0x6},
{0x806b,0x6},
{0x806c,0x1a},
{0x806d,0x1a},
{0x806e,0x3},
{0x806f,0x1f},
{0x8070,0x10},
{0x8071,0x0},
{0x8072,0x10},
{0x8073,0x0},
{0x8074,0xFF},
{0x8075,0xC3},
{0x8076,0xF5},
{0x8077,0xd9},
{0x8078,0xF2},
{0x8079,0xFc},
{0x807a,0xFe},
{0x807b,0xFF},
{0x807c,0xFF},
{0x807d,0xFF},
{0x807e,0xFF},
{0x807f,0xdb},
{0x8080,0xE9},
{0x8081,0xc2},
{0x8082,0xF2},
{0x8083,0xFc},
{0x8084,0xFe},
{0x8085,0xF2},
{0x8086,0xF2},
{0x8087,0xFF},
{0x8088,0xFF},
{0x8089,0xdb},
{0x808a,0xe9},
{0x808b,0xc2},
{0x808c,0xF2},
{0x808d,0xFc},
{0x808e,0xFe},
{0x808f,0xF2},
{0x8090,0xF2},
{0x8091,0xFF},
{0x8092,0xFF},
{0x8093,0xFF},
{0x8094,0xF2},
{0x8095,0x3F},
{0x8096,0xc},
{0x8097,0x35},
{0x8098,0x22},
{0x8099,0x2},
{0x809a,0x1},
{0x809b,0x0},
{0x809c,0x0},
{0x809d,0x4},
{0x809e,0x0},
{0x809f,0x0},
{0x80a0,0x4},
{0x80a1,0x4},
{0x80a2,0x0},
{0x80a3,0x0},
{0x80a4,0x4},
{0xe002,0xe},
{0xe056,0xf6},
{0x805F,0x43},
{0x8060,0xf7},
{0xe055,0x4c},
{0x800D,0x00},
{0x8051,0xd},
{0xe057,0x13},
{0xe004,0x1},
{0xe003,0x1},
{0x8003,0x1},
{0x8050,0x1},
{0x8002,0x3 },
{0xe002,0x3},
{0xe008,0x2},
{0xe006,0x33},
{0xe005,0x44},
{0x8008,0x57},
{0xe004,0x10},
{0xe003,0x10},
{0x8003,0x10},
{0xe015,0xd3},
{0xe016,0x22},
{0xe017,0x4},
{0x800d,0x10},
{0x80a5,0x3},
/*Some override register*/
{0x0002,0xe},
{0x0056,0xf6},
{0x805F,0x43},
{0x0055,0x4c},
{0x800D,0x00},
{0x8051,0x9},
{0x0057,0x13},
{0x0004,0x1},
{0x0003,0x1},
{0x8003,0x1},
{0x8050,0x1},
{0x0008,0x2},
{0x8002,0x3 },
{0x0002,0x3},
{0x8008,0x57},
{0x0004,0x10},
{0x0003,0x10},
{0x8003,0x10},
{0x0015,0xd3},
{0x0016,0x20},
{0x0017,0x00},
{0x8060,0xf7},
{0x800d,0x10 },
/*Enable BIST and Internal loopback*/
{0x8050,0x01},
{0x0008,0x02},
{0x0007,0x20},			/*TX Bist*/
{0x0008,0x13},		    /*RX Bist*/
/*Error Injection*/
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
{0x8050,0x21},
{0x8050,0x01},
};