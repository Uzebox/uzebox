/*
 *  UzeFS, a small FAT16/FAT32 file library for the Uzebox. See spec-uzefs.md.
 *
 *  - Root directory only, 8.3 names only (no long names, no subfolders).
 *  - Build options are in uzefs_conf.h, set with -D from the Makefile.
 *  - Every call returns a uzefs_res: UZEFS_OK (0) or one UZEFS_ERR_*.
 *  - Reads need no buffer. Writes need a 512-byte sector buffer that the host
 *    lends with uzefs_lend(); UzeFS has none of its own. A write-type call with
 *    nothing lent returns UZEFS_ERR_NOBUF and changes nothing.
 *  - A handle that met a disk error is failed: every later call on it returns
 *    UZEFS_ERR_DISK until it is closed.
 *  - SPI: the driver raises the SPI RAM chip select before each card command,
 *    so no SPI RAM sequence may be held open across a uzefs_* call.
 */

#ifndef UZEFS_H
#define UZEFS_H

#include <stdint.h>
#include "uzefs_conf.h"

typedef uint8_t uzefs_res;

#define UZEFS_OK               0
#define UZEFS_ERR_DISK         1   /* the card refused or failed a command */
#define UZEFS_ERR_NOT_READY    2   /* no card, or it did not initialise */
#define UZEFS_ERR_NO_FS        3   /* no FAT16 volume (or FAT32 with UZEFS_FAT32) */
#define UZEFS_ERR_CORRUPT      4   /* a chain points outside the volume, at a free cluster, or ends early */
#define UZEFS_ERR_NOT_MOUNTED  5   /* called before a successful uzefs_mount */
#define UZEFS_ERR_NO_FILE      6   /* name not found */
#define UZEFS_ERR_EXISTS       7   /* UZEFS_O_EXCL, or a rename target, already exists */
#define UZEFS_ERR_BAD_NAME     8   /* not a valid 8.3 name */
#define UZEFS_ERR_DENIED       9   /* read-only file written/deleted/renamed; a directory or label */
#define UZEFS_ERR_LOCKED      10   /* the file is open in a conflicting way */
#define UZEFS_ERR_TOO_MANY    11   /* every one of the UZEFS_MAX_FILES slots is open */
#define UZEFS_ERR_DIR_FULL    12   /* no free root directory entry */
#define UZEFS_ERR_FULL        13   /* no free cluster */
#define UZEFS_ERR_BAD_HANDLE  14   /* handle out of range or not open */
#define UZEFS_ERR_MODE        15   /* the handle's mode forbids it, or a bad flag mix */
#define UZEFS_ERR_RANGE       16   /* seek past the end without UZEFS_SEEK_EXTEND */
#define UZEFS_ERR_NOBUF       17   /* write-type call with no buffer lent */

/* Open modes, OR-ed together. READ and/or WRITE is required; CREATE, EXCL,
** TRUNC and APPEND need WRITE. Otherwise UZEFS_ERR_MODE. */
#define UZEFS_O_READ    0x01   /* reading allowed */
#define UZEFS_O_WRITE   0x02   /* writing allowed */
#define UZEFS_O_CREATE  0x04   /* create the file if it is missing */
#define UZEFS_O_EXCL    0x08   /* with CREATE: fail with UZEFS_ERR_EXISTS if it exists */
#define UZEFS_O_TRUNC   0x10   /* cut an existing file to 0 bytes */
#define UZEFS_O_APPEND  0x20   /* start at the end (UZEFS_APPEND builds only) */

/* FAT attribute bits, as in uzefs_info.attr */
#define UZEFS_AM_RDO  0x01     /* read-only */
#define UZEFS_AM_HID  0x02     /* hidden */
#define UZEFS_AM_SYS  0x04     /* system */
#define UZEFS_AM_VOL  0x08     /* volume label: never listed or opened */
#define UZEFS_AM_DIR  0x10     /* directory: listed, but cannot be opened */
#define UZEFS_AM_ARC  0x20     /* archive: set on every file UzeFS writes */

/* One directory entry, filled by uzefs_stat and uzefs_readdir. Caller-owned,
** 22 bytes. With UZEFS_INFO_NAME_ONLY only name and attr are filled. */
typedef struct {
	char     name[13];      /* "NAME.EXT", NUL-terminated; no dot without an extension */
	uint8_t  attr;          /* UZEFS_AM_* bits */
	uint32_t size;          /* bytes */
	uint16_t date, time;    /* FAT packed, last modified */
} uzefs_info;

/* A directory listing's position. Caller-owned, 5 bytes; only uzefs_opendir
** and uzefs_readdir should touch it. */
typedef struct {
	uint32_t sector;        /* sector of the next entry to look at; 0 at the end */
	uint8_t  index;         /* 0..15 within that sector; 16 is the next sector */
} uzefs_dir;


/*--- Volume --------------------------------------------------------------*/

/* Initialises the card and mounts its first FAT partition (MBR or bare
** volume). Forgets every open file and whatever the buffers held, without
** writing anything: call uzefs_unmount first if files may be dirty. Mount again
** after a card swap -- writing with another card's geometry corrupts it. */
uzefs_res uzefs_mount(void);

/* Syncs and closes every open file, writes a kept sector, then forgets the
** volume. UZEFS_ERR_NOBUF, closing nothing, if a dirty file needs a buffer
** and none is lent. */
uzefs_res uzefs_unmount(void);

#if UZEFS_WRITE
/* Lends UzeFS a 512-byte buffer for write-type calls. keep = 0: used only
** inside a call; the host may use it freely between calls. keep = 1 (tier 2,
** UZEFS_KEEP builds; otherwise treated as 0): UzeFS owns it until
** uzefs_unlend, and it may hold a sector not yet written. Lending again first
** does what uzefs_unlend does and returns its result. buf NULL lends nothing.
** Data given to uzefs_write, or a buffer given to uzefs_read, must not lie
** inside the lent buffer. */
uzefs_res uzefs_lend(uint8_t *buf, uint8_t keep);

/* Writes a kept sector that has changes waiting, forgets what the kept sector
** and the read buffer hold, and takes the buffer back. The buffer is returned
** even when the write fails (UZEFS_ERR_DISK). Call it before writing to the
** card other than through UzeFS. */
uzefs_res uzefs_unlend(void);
#endif


/*--- Files ---------------------------------------------------------------*/

/* Opens name (8.3, case-insensitive: stored in upper case) with UZEFS_O_*
** mode flags, and stores the handle, 0..UZEFS_MAX_FILES-1, in *fh.
** A writer excludes every other handle on the file, and a reader excludes
** writers (UZEFS_ERR_LOCKED). Opening with WRITE needs a lent buffer. A
** created file's entry is on the card when this returns. */
uzefs_res uzefs_open(uint8_t *fh, const char *name, uint8_t mode);

/* Syncs (for a handle that wrote) and frees the slot. The slot is freed even
** if the sync fails, unless that failure is UZEFS_ERR_NOBUF. */
uzefs_res uzefs_close(uint8_t fh);

/* Reads up to n bytes at the current position into buf and stores the count
** read in *done. At the end of the file it returns UZEFS_OK with *done < n. */
uzefs_res uzefs_read(uint8_t fh, uint8_t *buf, uint16_t n, uint16_t *done);

/* Moves the position to byte pos (0 = start, uzefs_size = end). Past the end:
** with UZEFS_SEEK_EXTEND and a handle opened for writing, the file is extended
** with zeros (UZEFS_ERR_MODE on a handle opened for reading only); without
** UZEFS_SEEK_EXTEND it is UZEFS_ERR_RANGE.
** Walks the cluster chain from the start, so it costs a FAT read per cluster. */
uzefs_res uzefs_seek(uint8_t fh, uint32_t pos);

uint32_t  uzefs_tell(uint8_t fh);   /* the current position; 0 for a bad handle */
uint32_t  uzefs_size(uint8_t fh);   /* the file's size in bytes; 0 for a bad handle */
uint8_t   uzefs_eof(uint8_t fh);    /* 1 at or past the end, and for a bad handle */
#define   uzefs_rewind(fh) uzefs_seek((fh), 0)

#if UZEFS_WRITE
/* Writes the directory entry (size, first cluster, date) if it changed, and a
** kept sector, so that everything written so far is on the card. */
uzefs_res uzefs_sync(uint8_t fh);

/* Writes n bytes from buf at the current position, growing the file and
** allocating clusters as needed, and stores the count written in *done.
** UZEFS_ERR_FULL when the card fills: *done then says how much made it.
** The size reaches the directory at sync/close (or every call with
** UZEFS_SIZE_EVERY_WRITE). */
uzefs_res uzefs_write(uint8_t fh, const uint8_t *buf, uint16_t n, uint16_t *done);

/* Cuts the file at the current position and frees every cluster after it. */
uzefs_res uzefs_truncate(uint8_t fh);

/* Deletes a closed file and frees its clusters. Needs a lent buffer.
** UZEFS_ERR_DENIED for a read-only file or a directory, UZEFS_ERR_LOCKED if
** it is open. */
uzefs_res uzefs_delete(const char *name);

/* Renames a closed file in place; its data does not move. UZEFS_ERR_EXISTS if
** to exists, otherwise as uzefs_delete. */
uzefs_res uzefs_rename(const char *from, const char *to);
#endif


/*--- Directory -----------------------------------------------------------*/

/* Finds name and fills *fi. fi may be NULL: an existence check. */
uzefs_res uzefs_stat(const char *name, uzefs_info *fi);

#if UZEFS_DIR
/* Positions *d at the start of the root directory. */
uzefs_res uzefs_opendir(uzefs_dir *d);

/* Fills *fi with the next entry and advances *d. At the end it returns
** UZEFS_OK with fi->name[0] == 0 -- the end is not an error. fi NULL rewinds
** *d, as uzefs_opendir. Deleted entries, long-name parts and the volume label
** are skipped; hidden, system and directory entries are listed, so filter on
** fi->attr. Entries come in on-disk order, unsorted. */
uzefs_res uzefs_readdir(uzefs_dir *d, uzefs_info *fi);
#endif

#if UZEFS_GETFREE
/* Meant to count free clusters by reading the whole FAT and give the cluster
** size in KB. DECLARED ONLY: uzefs.c has no body for it yet, so a build that
** sets UZEFS_GETFREE and calls it fails to link. */
uzefs_res uzefs_getfree(uint32_t *clusters, uint16_t *cluster_kb);
#endif


/*--- Streaming -----------------------------------------------------------*/

/* Counts the runs of consecutive clusters that hold the file, and stores the
** count in *n: 1 for a file in one piece, which a multiple-block read can
** take from its first sector to its last; 2 or more for a fragmented one; 0
** for an empty file. More than 65,535 is stored as 65,535. The handle's
** position does not move. It follows the cluster chain from the start, a FAT
** read per cluster as uzefs_seek to the end does, so a read buffer or a lent
** sector speeds it up the same way. A count of 1 also lets uzefs_extent answer
** without reading the FAT, until the file is next written.
** UZEFS_ERR_BAD_HANDLE, UZEFS_ERR_DISK, UZEFS_ERR_CORRUPT. */
uzefs_res uzefs_fragments(uint8_t fh, uint16_t *n);

/* Where byte pos of the file is on the card: *sect, the card sector holding
** it, and *run, how many sectors from there on hold the file without a break,
** up to its last sector. A multiple-block read from *sect may take *run
** sectors. The handle's position does not move. On a file uzefs_fragments
** found in one piece it reads nothing; otherwise it follows the chain to pos
** and on to the end of that run, a FAT read per cluster.
** UZEFS_ERR_BAD_HANDLE, UZEFS_ERR_RANGE (pos at or past the end),
** UZEFS_ERR_DISK, UZEFS_ERR_CORRUPT. */
uzefs_res uzefs_extent(uint8_t fh, uint32_t pos, uint32_t *sect, uint32_t *run);

/* Starts a multiple-block read (CMD18) at card sector sect and returns with
** the card selected and the read open, for a host that clocks the SPI itself
** (SPCR on, F_CPU / 2). From there the card sends, for each sector: 0xFF for
** as long as it needs, the 0xFE token, 512 data bytes, 2 CRC bytes. The host
** sends 0xFF and may stop the clock between any two bytes (SPI off, card still
** selected), but must not deselect the card: some cards then stop answering.
** No other uzefs call and no SPI RAM until uzefs_stream_end.
** UZEFS_ERR_NOT_MOUNTED, UZEFS_ERR_DISK (the card refused; it is deselected). */
uzefs_res uzefs_stream_begin(uint32_t sect);

/* Ends the read (CMD12) wherever it is, waits out the card's busy, and
** deselects the card. Turns the SPI back on if the host turned it off. Call it
** with no byte being clocked. UZEFS_OK, or UZEFS_ERR_DISK if the busy did not
** end. */
uzefs_res uzefs_stream_end(void);

/* Moves the open read to card sector sect: ends it (CMD12) wherever it is and
** starts another (CMD18) there, returning as uzefs_stream_begin does. It takes
** a few dozen bytes plus the card's busy after CMD12, which it waits out for at
** most busy_max bytes (a byte is 0.6 to 1 us at 28.6 MHz), so a host that must
** have the bus back by a deadline can bound it. Call it with no byte being
** clocked; it turns the SPI back on if the host turned it off. UZEFS_OK;
** UZEFS_ERR_DISK if the busy outlasted busy_max (the card is still selected:
** end with uzefs_stream_end, which waits longer) or the card refused CMD18
** (deselected). */
uzefs_res uzefs_stream_restart(uint32_t sect, uint16_t busy_max);

/* Measures this card's gap between consecutive sectors of a multiple-block
** read (CMD18), for a host that will stream with one. The gap is every byte
** that is not data between two sectors: 2 CRC bytes, the 0xFF bytes sent
** while the card fetches the next sector, and the 0xFE token -- so 3 at
** least. It reads n + 1 sectors from the start of the data area (n 1..255;
** 0 counts as 1), writes nothing, and stores the shortest and longest of the
** n gaps in *min and *max.
** The card's delay is a time, so the count holds for the driver's SPI clock
** (F_CPU / 2) clocked without a pause: an interrupt taken inside a gap makes
** that gap read short, which is why *max is the figure to plan with. Cards
** differ, and one card can now and then take far longer than its usual gap,
** so measure several. Called with interrupts off the count is exact, and
** *min == *max then says the card is steady. That takes about 0.45 ms a gap
** at 28.6 MHz (64 gaps: 30 ms) on top of the card's own access time.
** UZEFS_ERR_NOT_MOUNTED, UZEFS_ERR_DISK. */
uzefs_res uzefs_stream_gap(uint8_t n, uint16_t *min, uint16_t *max);


/*--- Host hook -----------------------------------------------------------*/

/* The time stamped on created and modified files, FAT packed: date << 16 |
** time. Weak: the library's default returns UZEFS_FIXED_DATE; a host with a
** clock defines its own. */
uint32_t  uzefs_time(void);

#endif
