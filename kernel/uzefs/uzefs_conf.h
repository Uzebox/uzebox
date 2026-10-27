/*
 *  UzeFS build configuration.
 *
 *  Every option is #ifndef-guarded, so a project sets it from its Makefile
 *  with -DUZEFS_xxx=n and never edits this file; the values below are the
 *  defaults. Unused API functions cost no flash under -Wl,-gc-sections, so
 *  these switches are for code *inside* the calls that are used. Full
 *  reference: spec-uzefs.md section 8 (options), section 5 (memory tiers).
 *
 *  RAM: 29 bytes for the volume + 26 per file slot, plus the options' own
 *  figures below. Writes need a 512-byte buffer lent by the host
 *  (uzefs_lend); UzeFS owns no sector buffer of its own.
 */

#ifndef UZEFS_CONF_H
#define UZEFS_CONF_H

/* Files open at once, >= 1. Handles are 0..UZEFS_MAX_FILES-1. 26 bytes of RAM
** each. uzefs_open returns UZEFS_ERR_TOO_MANY when every slot is taken. */
#ifndef UZEFS_MAX_FILES
#define UZEFS_MAX_FILES        4
#endif

/* 1: read-write. 0: read-only -- mount, open for reading, read, seek, stat and
** directory listing only; uzefs_lend/unlend, write, sync, truncate, delete and
** rename do not exist, and UZEFS_KEEP is forced to 0. */
#ifndef UZEFS_WRITE
#define UZEFS_WRITE            1
#endif

/* 1: FAT32 volumes mount too (partition types 0x0B/0x0C), decided per card at
** mount, so one build takes FAT16 or FAT32. About 1.4 KB, most of it 32-bit
** cluster numbers everywhere; +15 bytes of RAM. 0: FAT16 only, and a FAT32
** card fails to mount with UZEFS_ERR_NO_FS. */
#ifndef UZEFS_FAT32
#define UZEFS_FAT32            0
#endif

/* With UZEFS_FAT32 only. 1: a create in a full FAT32 root directory (a cluster
** chain) links it one more zeroed cluster. 0: refused with UZEFS_ERR_DIR_FULL,
** as a full FAT16 root is. 0 saves about 190 bytes. */
#ifndef UZEFS_ROOT_GROW
#define UZEFS_ROOT_GROW        1
#endif

/* With UZEFS_FAT32 only. 1: the first FAT change after a mount marks the FSInfo
** sector's free-cluster count unknown (0xFFFFFFFF), so a PC counts the free
** space again. 0: that count goes stale and a PC shows it wrong until chkdsk.
** 0 saves about 210 bytes and 4 of RAM. */
#ifndef UZEFS_FSINFO
#define UZEFS_FSINFO           1
#endif

/* 1: SDHC/SDXC cards too (CMD8 at init, block addressing when CCS is set).
** 0: standard-capacity cards only (byte addressing), a little smaller. */
#ifndef UZEFS_SDHC
#define UZEFS_SDHC             1
#endif

/* 1: the card's init, its commands and the mount are a version 5 bootloader's
** (5.0.06 and later, at 0xF000; ../bootld5.h): link uzefs_bl.c, not
** uzefs_sd.c. About 800 bytes less. The mount then needs a buffer lent
** (uzefs_lend), into which the bootloader reads the boot sector, and takes the
** bootloader's limits: the card's first partition, starting under sector
** 65,536, with two FATs. With no such bootloader the mount is
** UZEFS_ERR_NOT_READY. Needs UZEFS_WRITE (the lent buffer). */
#ifndef UZEFS_BOOTLOADER
#define UZEFS_BOOTLOADER       0
#endif

/* Tier 1: a static read buffer of this many bytes, 0 = none, at most 255
** (16..128 is sensible). A read shorter than it -- each FAT entry followed by
** a seek included -- is served from it and refilled from the card when needed.
** Costs 7 + n bytes of RAM; does nothing for writes. */
#ifndef UZEFS_RDBUF
#define UZEFS_RDBUF            0
#endif

/* Tier 2. 1: uzefs_lend(buf, 1) lets UzeFS keep a sector in the lent buffer
** between calls, written back only when another sector needs the buffer or at
** sync, close, unmount, lend and unlend. Many small writes become one sector
** write (a 6 KB Basic SAVE: 1,569 frames at tier 0, 172 here). +5 bytes of RAM.
** With both tiers on, the kept sector serves reads while lent with keep. */
#ifndef UZEFS_KEEP
#define UZEFS_KEEP             0
#endif

/* The kept sector holds writes, so a read-only build has none; the read
** buffer's fill count is one byte. */
#if !UZEFS_WRITE
#undef  UZEFS_KEEP
#define UZEFS_KEEP             0
#endif
#if UZEFS_RDBUF > 255
#error "UZEFS_RDBUF must be 255 or less"
#endif
#if UZEFS_BOOTLOADER && !UZEFS_WRITE
#error "UZEFS_BOOTLOADER needs UZEFS_WRITE: the mount reads into the lent buffer"
#endif

/* 1: uzefs_opendir / uzefs_readdir, listing the root directory. 0: neither
** exists. */
#ifndef UZEFS_DIR
#define UZEFS_DIR              1
#endif

/* 1: uzefs_open honours UZEFS_O_APPEND (a seek to the end after opening).
** 0: open refuses UZEFS_O_APPEND with UZEFS_ERR_MODE, and a host that never
** calls uzefs_seek does not link it. */
#ifndef UZEFS_APPEND
#define UZEFS_APPEND           1
#endif

/* 1: uzefs_readdir and uzefs_stat fill only the name and attributes of their
** uzefs_info; size, date and time are left as they were (for a host that lists
** names only). 0: every field is filled. */
#ifndef UZEFS_INFO_NAME_ONLY
#define UZEFS_INFO_NAME_ONLY   0
#endif

/* 1: uzefs_seek past the end of a file opened for writing extends it with
** zeros, as POSIX lseek + write would. 0: such a seek is UZEFS_ERR_RANGE. */
#ifndef UZEFS_SEEK_EXTEND
#define UZEFS_SEEK_EXTEND      1
#endif

/* 1: declares uzefs_getfree. Not implemented in uzefs.c yet: calling it fails
** to link. */
#ifndef UZEFS_GETFREE
#define UZEFS_GETFREE          0
#endif

/* 1: every uzefs_write that wrote anything ends with a uzefs_sync, so the
** directory entry's size is right after each call and a card pulled mid-file
** loses less. Costs a directory sector read and write per call. 0: the size
** reaches the card at sync and close only. */
#ifndef UZEFS_SIZE_EVERY_WRITE
#define UZEFS_SIZE_EVERY_WRITE 0
#endif

/* What the weak default uzefs_time() returns, stamped on created and written
** files: FAT packed date << 16 | time, here 2026-01-01 00:00. A host with a
** clock defines its own uzefs_time() instead. */
#ifndef UZEFS_FIXED_DATE
#define UZEFS_FIXED_DATE       ((((2026UL - 1980UL) << 9) | (1UL << 5) | 1UL) << 16)
#endif

#endif
