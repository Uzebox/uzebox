/*
 *  UzeFS SD card driver. Derived from kernel/petitfatfs/diskio.c.
 *  Every function leaves the card deselected.
 */

#ifndef UZEFS_SD_H
#define UZEFS_SD_H

#include <stdint.h>
#include "uzefs.h"

/* UZEFS_OK or UZEFS_ERR_NOT_READY */
uzefs_res uzefs_sd_init(void);

/* Reads cnt bytes (1..512-ofs) at ofs of sector sect. With buf NULL each byte
** goes to uzefs_sd_handler. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_read(uint8_t *buf, uint32_t sect, uint16_t ofs, uint16_t cnt);
extern void (*uzefs_sd_handler)(uint8_t);

/* Times a multiple-block read of n + 1 sectors from sect: see uzefs_stream_gap
** in uzefs.h. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_gap(uint32_t sect, uint8_t n, uint16_t *min, uint16_t *max);

/* CMD18 at sect, the card left selected with the read open: see
** uzefs_stream_begin in uzefs.h. UZEFS_OK or UZEFS_ERR_DISK (deselected). */
uzefs_res uzefs_sd_stream_begin(uint32_t sect);

/* CMD12, its busy, then deselected: see uzefs_stream_end. */
uzefs_res uzefs_sd_stream_end(void);

/* CMD12, at most busy_max bytes of busy, CMD18 at sect: see
** uzefs_stream_restart. */
uzefs_res uzefs_sd_stream_restart(uint32_t sect, uint16_t busy_max);

/* The 16 bytes of the CID (which 0) or the CSD (which 1) into buf.
** UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_reg(uint8_t which, uint8_t *buf);

#if UZEFS_WRITE
/* Writes 512 bytes from buf, or 512 zeros with buf NULL. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_write(const uint8_t *buf, uint32_t sect);
#define   uzefs_sd_zero(sect) uzefs_sd_write(0, (sect))
#endif

#endif
