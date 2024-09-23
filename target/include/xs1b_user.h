/*
 *
 * Copyright XMOS - (c) 2007 - 2018
 *
 * AUTOGENERATED - DO NOT EDIT
 * by lib_xmosutils/Scripts/CreateXS1.pl
 *
 */
#ifndef _xs1b_user_h_
#define _xs1b_user_h_
#define XS1_SETC_MODE_SHIFT 0x0
#define XS1_SETC_MODE_SIZE 0x3
#define XS1_SETC_MODE_MASK (((1 << XS1_SETC_MODE_SIZE) - 1) << XS1_SETC_MODE_SHIFT)
#define XS1_SETC_MODE(x) (((x) & XS1_SETC_MODE_MASK) >> XS1_SETC_MODE_SHIFT)
#define XS1_SETC_MODE_SET(x, v) (((x) & ~XS1_SETC_MODE_MASK) | (((v) << XS1_SETC_MODE_SHIFT) & XS1_SETC_MODE_MASK))
#define XS1_SETC_VALUE_SHIFT 0x3
#define XS1_SETC_VALUE_SIZE 0x9
#define XS1_SETC_VALUE_MASK (((1 << XS1_SETC_VALUE_SIZE) - 1) << XS1_SETC_VALUE_SHIFT)
#define XS1_SETC_VALUE(x) (((x) & XS1_SETC_VALUE_MASK) >> XS1_SETC_VALUE_SHIFT)
#define XS1_SETC_VALUE_SET(x, v) (((x) & ~XS1_SETC_VALUE_MASK) | (((v) << XS1_SETC_VALUE_SHIFT) & XS1_SETC_VALUE_MASK))
#define XS1_SETC_LMODE_SHIFT 0xc
#define XS1_SETC_LMODE_SIZE 0x4
#define XS1_SETC_LMODE_MASK (((1 << XS1_SETC_LMODE_SIZE) - 1) << XS1_SETC_LMODE_SHIFT)
#define XS1_SETC_LMODE(x) (((x) & XS1_SETC_LMODE_MASK) >> XS1_SETC_LMODE_SHIFT)
#define XS1_SETC_LMODE_SET(x, v) (((x) & ~XS1_SETC_LMODE_MASK) | (((v) << XS1_SETC_LMODE_SHIFT) & XS1_SETC_LMODE_MASK))
#define XS1_NUM_NETWORKS 0x4
#define XS1_RES_TYPE_PORT 0x0
#define XS1_RES_TYPE_TIMER 0x1
#define XS1_RES_TYPE_CHANEND 0x2
#define XS1_RES_TYPE_SYNC 0x3
#define XS1_RES_TYPE_THREAD 0x4
#define XS1_RES_TYPE_LOCK 0x5
#define XS1_RES_TYPE_CLKBLK 0x6
#define XS1_RES_TYPE_COPROC 0x7
#define XS1_RES_TYPE_PS 0xb
#define XS1_RES_TYPE_CONFIG 0xc
#define XS1_RES_TYPE_INSTRUCTION 0xd
#define XS1_NUM_RES_TYPES 0x8
#define XS1_CLK_REF 0x1
#define XS1_CLK_XCORE 0x101
#define XS1_CLKBLK_REF 0x6
#define XS1_CLKBLK_1 0x106
#define XS1_CLKBLK_2 0x206
#define XS1_CLKBLK_3 0x306
#define XS1_CLKBLK_4 0x406
#define XS1_CLKBLK_5 0x506
#define XS1_ET_LINK_ERROR 0x1
#define XS1_ET_ILLEGAL_PC 0x2
#define XS1_ET_ILLEGAL_INSTRUCTION 0x3
#define XS1_ET_ILLEGAL_RESOURCE 0x4
#define XS1_ET_LOAD_STORE 0x5
#define XS1_ET_ILLEGAL_PS 0x6
#define XS1_ET_ARITHMETIC 0x7
#define XS1_ET_ECALL 0x8
#define XS1_ET_RESOURCE_DEP 0x9
#define XS1_ET_KCALL 0xf
#define XS1_ET_NONE 0x0
#define XS1_RES_ID_INVALID 0x0
#define XS1_NUM_LOCKS 0x4
#define XS1_NUM_SYNCS 0x7
#define XS1_NUM_TIMERS 0xa
#define XS1_NUM_THREADS 0x8
#define XS1_NUM_CHANENDS 0x20
#define XS1_NUM_CLKBLKS 0x6
#define XS1_CT_START_TRANSACTION 0x0
#define XS1_CT_END 0x1
#define XS1_CT_PAUSE 0x2
#define XS1_CT_ACK 0x3
#define XS1_CT_NACK 0x4
#define XS1_CT_READN 0x10
#define XS1_CT_READ1 0x11
#define XS1_CT_READ2 0x12
#define XS1_CT_READ4 0x13
#define XS1_CT_READ8 0x14
#define XS1_CT_WRITEN 0x15
#define XS1_CT_WRITE1 0x16
#define XS1_CT_WRITE2 0x17
#define XS1_CT_WRITE4 0x18
#define XS1_CT_WRITE8 0x19
#define XS1_CT_CALL 0x1a
#define XS1_CT_WRITEC 0xc0
#define XS1_CT_READC 0xc1
#define XS1_CT_PSCTRL 0xc2
#define XS1_CT_SSCTRL 0xc3
#define XS1_SETC_INUSE_OFF 0x0
#define XS1_SETC_INUSE_ON 0x8
#define XS1_SETC_COND_NONE 0x1
#define XS1_SETC_COND_FULL 0x1
#define XS1_SETC_COND_AFTER 0x9
#define XS1_SETC_COND_EQ 0x11
#define XS1_SETC_COND_NEQ 0x19
#define XS1_SETC_IE_MODE_EVENT 0x2
#define XS1_SETC_IE_MODE_INTERRUPT 0xa
#define XS1_SETC_DRIVE_DRIVE 0x3
#define XS1_SETC_DRIVE_PULL_DOWN 0xb
#define XS1_SETC_DRIVE_PULL_UP 0x13
#define XS1_SETC_RUN_STOPR 0x7
#define XS1_SETC_RUN_STARTR 0xf
#define XS1_SETC_RUN_CLRBUF 0x17
#define XS1_SETC_MS_MASTER 0x1007
#define XS1_SETC_MS_SLAVE 0x100f
#define XS1_SETC_BUF_NOBUFFERS 0x2007
#define XS1_SETC_BUF_BUFFERS 0x200f
#define XS1_SETC_RDY_NOREADY 0x3007
#define XS1_SETC_RDY_STROBED 0x300f
#define XS1_SETC_RDY_HANDSHAKE 0x3017
#define XS1_SETC_SDELAY_NOSDELAY 0x4007
#define XS1_SETC_SDELAY_SDELAY 0x400f
#define XS1_SETC_PORT_DATAPORT 0x5007
#define XS1_SETC_PORT_CLOCKPORT 0x500f
#define XS1_SETC_PORT_READYPORT 0x5017
#define XS1_SETC_INV_NOINVERT 0x6007
#define XS1_SETC_INV_INVERT 0x600f
#define XS1_NUM_1BIT_GPIO_PORTS 0x10
#define XS1_NUM_4BIT_GPIO_PORTS 0x6
#define XS1_NUM_8BIT_GPIO_PORTS 0x4
#define XS1_NUM_16BIT_GPIO_PORTS 0x2
#define XS1_NUM_32BIT_GPIO_PORTS 0x1
#define XS1_NUM_1BIT_PORTS 0x10
#define XS1_NUM_4BIT_PORTS 0x6
#define XS1_NUM_8BIT_PORTS 0x4
#define XS1_NUM_16BIT_PORTS 0x4
#define XS1_NUM_32BIT_PORTS 0x2
#endif /* _xs1b_user_h_ */
