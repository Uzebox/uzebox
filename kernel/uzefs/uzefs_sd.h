/*
 *  UzeFS SD card driver. Derived from kernel/petitfatfs/diskio.c.
 *  Every function leaves the card deselected.
 */

#ifndef UZEFS_SD_H
#define UZEFS_SD_H

#include <stdint.h>
#include "uzefs.h"

#if UZEFS_BOOTLOADER
#include <bootlib.h>

/* uzefs_bl.c: the card initialised and its first FAT volume found by a version
** 5 bootloader's FAT_Init, into *v (bootlib.h: csize, fatp, datap, rootp, and
** SDC_FLAGS_FAT32 in flags), the volume's boot sector left in v->bufp (512
** bytes, set by the caller). UZEFS_OK, UZEFS_ERR_NOT_READY (no card, or no
** version 5.0.06+ bootloader), UZEFS_ERR_DISK or UZEFS_ERR_NO_FS. */
uzefs_res uzefs_sd_mount(sdc_struct_t *v);
#else
/* UZEFS_OK or UZEFS_ERR_NOT_READY */
uzefs_res uzefs_sd_init(void);
#endif

/* Reads cnt bytes (1..512-ofs) at ofs of sector sect. With buf NULL each byte
** goes to uzefs_sd_handler. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_read(uint8_t *buf, uint32_t sect, uint16_t ofs, uint16_t cnt);
extern void (*uzefs_sd_handler)(uint8_t);

/* Times a multiple-block read of n + 1 sectors from sect: see uzefs_stream_gap
** in uzefs.h. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_gap(uint32_t sect, uint8_t n, uint16_t *min, uint16_t *max);

/* From byte ofs of sector sect on, in one multiple-block read across as many
** sectors as they take: how many of the next n little-endian words (4 bytes,
** or 2 with wide 0) count up v, v+1, v+2..., into *k. A 4-byte word's top 4
** bits are left out (FAT32 reserves them). A file in one piece has its FAT
** entries so. UZEFS_OK or UZEFS_ERR_DISK. */
uzefs_res uzefs_sd_count(uint32_t sect, uint16_t ofs, uint32_t v, uint32_t n, uint8_t wide,
                         uint32_t *k);

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
