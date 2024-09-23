/*
 * Generated file - do not edit.
 */
#define FL_QUADDEVICE_DEFAULT \
{ \
    -1,                     /* default parameters */ \
    256,                    /* page size */ \
    0,                      /* num pages */ \
    3,                      /* address size */ \
    3,                      /* log2 clock divider */ \
    0x9F,                   /* QSPI_RDID */ \
    0,                      /* id dummy bytes */ \
    3,                      /* id size in bytes */ \
    0x00000000,             /* device id */ \
    0x20,                   /* QSPI_SE */ \
    4096,                   /* Sector erase is always 4KB */ \
    0x06,                   /* QSPI_WREN */ \
    0x04,                   /* QSPI_WRDI */ \
    PROT_TYPE_NONE,         /* no protection */ \
    {{0,0},{0x00,0x00}},    /* QSPI_SP, QSPI_SU */ \
    0x02,                   /* QSPI_PP */ \
    0xEB|(0x0B<<8),         /* QSPI_READ_QUAD */ \
    1,                      /* 1 read dummy byte */ \
    SECTOR_LAYOUT_REGULAR,  /* sane sectors */ \
    {4096,{0,{0}}},         /* regular sector sizes */ \
    0x05,                   /* QSPI_RDSR */ \
    0x01,                   /* QSPI_WRSR */ \
    0x01,                   /* QSPI_WIP_BIT_MASK */ \
    0xFFFFFFFF,             /* id "don't care" mask */ \
    0,                      /* quad enable bit */ \
}
