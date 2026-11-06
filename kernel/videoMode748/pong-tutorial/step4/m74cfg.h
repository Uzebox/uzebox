/* Mode 748 settings for the Pong tutorial */
#ifndef M74CFG_H
#define M74CFG_H

#define M74_SPR_ENABLE   1        /* use sprites */
#define M74_MSK_ENABLE   0        /* no sprite masks */
#define M74_RECTB_OFF    0xE000   /* recolour table: the Makefile puts it here */

#define RT_BASE          64U      /* RAM tiles for sprites start at 64 * 32 = 0x0800 */
#define RT_MAX           24U      /* and there are 24 of them (up to 0x0B00) */

#endif
