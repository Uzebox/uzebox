/*
 *  The version 5 bootloader's entry points (5.0.06 and later), called directly.
 *
 *  A version 5 bootloader has a table of entry points at 0xF000 (its source is
 *  demos/Bootloader5, sdlib.s); a program can use its SD card and FAT code for
 *  nothing. These are inline calls: nothing is linked but what a program calls.
 *  bootlib.s is the same interface with a fallback for a console with no such
 *  bootloader, which costs the code these save; here bl5_present() must be true
 *  before any other call, or the call runs into erased flash and resets.
 *
 *  The SPI bus: the bootloader's routines wait on SPIF, which the SPI RAM's
 *  routines leave set; clear it first (read SPSR, then SPDR), raise the SPI RAM's
 *  chip select (PA4), and give the SPI RAM its clock back afterwards.
 *  The card's routines leave the bus at the card's clock, 1/8 of the CPU's.
 *
 *  sdc_struct_t (bootlib.h) is the bootloader's state: bufp a 512-byte buffer.
 */

#ifndef BOOTLD5_H
#define BOOTLD5_H


#define BL5_SIGNATURE   0xF002		/* B0 07 10 AD, then the version: 0x50, 0x06 and later */
#define BL5_SD_COMMAND  0xF00E
#define BL5_SD_RELEASE  0xF010
#define BL5_SD_INIT     0xF012
#define BL5_SD_READ     0xF016		/* with a retry */
#define BL5_FAT_INIT    0xF01C
#define BL5_FAT_READ    0xF020
#define BL5_FAT_NEXT    0xF024
#define BL5_FAT_ROOT    0xF028
#define BL5_FAT_CLUSTER 0xF02A
#define BL5_FAT_FILE    0xF02C

#ifndef __ASSEMBLER__	/* the addresses alone for assembly */
#include <stdint.h>
#include <avr/pgmspace.h>
#include "bootlib.h"

/* True if a version 5.0.06 or later bootloader is at 0xF000 */
static inline uint8_t bl5_present(void)
{
	return pgm_read_dword(BL5_SIGNATURE) == 0xAD1007B0UL &&
	       ((uint16_t)pgm_read_byte(BL5_SIGNATURE + 4) << 8 | pgm_read_byte(BL5_SIGNATURE + 5)) >= 0x5006;
}

/* A command (CRC7 made), its R1 answer returned; the card selected until bl5_sd_release() */
static inline uint8_t bl5_sd_command(uint8_t cmd, uint32_t arg)
{
	return ((uint8_t (*)(uint8_t, uint32_t))(BL5_SD_COMMAND / 2))(cmd, arg);
}

/* The card deselected, and a byte clocked */
static inline void bl5_sd_release(void)
{
	((void (*)(void))(BL5_SD_RELEASE / 2))();
}

/* The card initialised (CRC checking on), SDHC in flags: 0, or 1-5 (bootlib.h, SDC_Init) */
static inline uint8_t bl5_sd_init(sdc_struct_t *d)
{
	return ((uint8_t (*)(sdc_struct_t *))(BL5_SD_INIT / 2))(d);
}

/* A whole sector into d->bufp, its CRC checked: 0, or 1-4 (bootlib.h, SDC_Read_Sector) */
static inline uint8_t bl5_sd_read(sdc_struct_t *d, uint32_t sector)
{
	return ((uint8_t (*)(sdc_struct_t *, uint32_t))(BL5_SD_READ / 2))(d, sector);
}

/* The card and its first FAT16 or FAT32 volume: 0, or 1-7 (bootlib.h, FS_Init). The volume's
** boot sector is left in d->bufp. */
static inline uint8_t bl5_fat_init(sdc_struct_t *d)
{
	return ((uint8_t (*)(sdc_struct_t *))(BL5_FAT_INIT / 2))(d);
}

/* The selected file's current sector into d->bufp: 0, or a read's error */
static inline uint8_t bl5_fat_read(sdc_struct_t *d)
{
	return ((uint8_t (*)(sdc_struct_t *))(BL5_FAT_READ / 2))(d);
}

/* The selected file's next sector: 0, or 1 at its end */
static inline uint8_t bl5_fat_next(sdc_struct_t *d)
{
	return ((uint8_t (*)(sdc_struct_t *))(BL5_FAT_NEXT / 2))(d);
}

/* The root directory selected, at its start */
static inline void bl5_fat_root(sdc_struct_t *d)
{
	((void (*)(sdc_struct_t *))(BL5_FAT_ROOT / 2))(d);
}

/* A file selected by its first cluster, at its start */
static inline void bl5_fat_cluster(sdc_struct_t *d, uint32_t cluster)
{
	((void (*)(sdc_struct_t *, uint32_t))(BL5_FAT_CLUSTER / 2))(d, cluster);
}

/* A 32-byte directory entry's first cluster, 0 if it is not a file's */
static inline uint32_t bl5_fat_file(sdc_struct_t *d, uint8_t *entry)
{
	return ((uint32_t (*)(sdc_struct_t *, uint8_t *))(BL5_FAT_FILE / 2))(d, entry);
}

#endif /* __ASSEMBLER__ */
#endif
