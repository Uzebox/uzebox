# Spec: UzeFS, a small FAT file library for Uzebox Basic

Status: **version 1 functions implemented and passing on the emulator** (mount,
read, seek, stat, listing, create, write, sync, close, truncate, delete,
rename), and passing on a 256 MB FAT16 SD card on the Uzebox with a clean
`chkdsk`, including cards pulled while files were created, grown, truncated,
deleted and renamed. The faster memory tiers (1, a small read buffer, and 2,
a kept sector) are built and give the same results and card contents as tier
0 on the emulator; on a real card only tier 2 has run, through Basic's SAVE.
**Uzebox Basic reads and writes its programs with UzeFS** (LOAD and SAVE),
tested on the emulator and on a Uzebox with a clean `chkdsk`; its other file
commands are not written yet. Version 1 covers SDSC cards with FAT16. **FAT32 is built since
2026-09-26** (`UZEFS_FAT32=1`, which Basic sets): one build mounts either type, and it passes on
a 4 KB-cluster FAT32 SDHC card on the Uzebox (the FAT32 bullets under 11.1–11.4).

UzeFS lets a Uzebox program create, write, append to, truncate, rename and delete
files on the SD card, and keep several of them open at once. Petit FatFs, which
Uzebox Basic used before, can only read files and overwrite them in place. FatFs
can do all of this but needs a permanent 512-byte buffer and more flash. UzeFS
does it in 8,271 bytes of flash (7,495 with `-mrelax -mcall-prologues`) and 133
bytes of permanent RAM with four file slots.


## 1. Goal, guiding principle, verdict

### 1.1 Guiding principle: RAM comes first

The ATmega644 has 4 KB of RAM, and Uzebox Basic leaves about 800 bytes of it for
the stack. So:

- **The default build uses the least RAM possible.**
- **Every trade of RAM for speed is opt-in**, either at compile time or by the
  host lending memory at run time (the memory model section).
- **No speed-up may raise the default RAM cost.** The one exception is the
  4-byte free-cluster hint, which stops allocation from rescanning the FAT from
  the start every time.
- Flash is also scarce, so a few bytes of RAM may be traded for a large flash
  saving. The one such trade is the 4-byte current sector in each file slot,
  which replaces 32-bit position arithmetic repeated across read, write and
  seek.
- Speed is otherwise a low priority.

### 1.2 Verdict

Every objective can be met, with one correction:

| Objective | Met? |
|---|---|
| One drive, the SD card | Yes |
| Create, read, write, delete, rename, seek | Yes, plus truncate and file information |
| More than one open file | Yes, `UZEFS_MAX_FILES`, default 4, 26 bytes each |
| Root directory only | Yes. Subdirectories made on a PC are listed but cannot be entered |
| No 512-byte sector buffer; buffer size configurable | **Reads: yes. Writes: no.** Every call that writes needs 512 bytes of RAM *for the length of the call*. The library keeps nothing in it between calls, so the host can lend it (the section on why a 512-byte buffer is unavoidable) |
| Performance low priority | Yes. The minimum-RAM build runs at Petit FatFs speed; more RAM buys more speed |
| FAT32 | **Yes, with `UZEFS_FAT32=1`** (2026-09-26). One build mounts FAT16 or FAT32; the emulator's card is FAT16, so FAT32 is tested on a Uzebox only |

### 1.3 Cost

| | Flash (bytes) | Permanent RAM (bytes) | While a write-type call runs |
|---|---|---|---|
| Petit FatFs R0.03a, read only (measured in Basic) | 3,552 | 42 | — |
| Petit FatFs, read + write + seek (measured in Basic by an earlier build, which put read only at 3,796) | 5,434 | 42 | — |
| FatFs R0.16, read/write, `FF_FS_TINY` (measured, fattest ROM) | 13,776 + 2,888 driver | 549 plus a `FIL` per open file | — |
| UzeFS read only (`UZEFS_WRITE=0`), measured | 4,795; 4,281 with the flags below | 23 + 26 per file | — |
| **UzeFS version 1, SDSC + FAT16, 4 files**, measured | **8,271** | 29 + 26 per file: **133 with 4** | 512, lent by the host |
| The same with `-mrelax -mcall-prologues`, measured | 7,495 | same | |
| Add FAT32, measured in Basic 2026-09-26 (estimated +300–450 before it was built) | +1,424; +1,022 with `UZEFS_ROOT_GROW=0 UZEFS_FSINFO=0` | +5 (+1 with `UZEFS_FSINFO=0`) | |
| Add tier 1 read buffer of *n* bytes, measured with *n* = 32 | +302 (+274 with the flags); read only +262 (+220) | +7 + *n* | |
| Add tier 2 kept sector, measured | +518 (+476 with the flags) | +5 | host keeps 512 lent |
| Add both tiers, measured with *n* = 32 | +728 (+720 with the flags) | +12 + *n* | host keeps 512 lent |
| Add `uzefs_getfree` | +80–120 | +0 | |

Measured figures are linked sizes (`-Wl,-gc-sections`, every function called by
the `demos/uzefstest` harness). Code on AVR costs about twice what a count of
operations suggests, because every 32-bit shift, compare and add is expanded
inline; the estimates for later additions are calibrated against the measured
build. UzeFS keeps no string constants in RAM: its one table, the characters a
name may not contain, is in flash.

For comparison inside Basic's own ROM, where only what Basic calls is linked,
full FatFs cost 11,330 bytes (the Basic Makefile's record of the attempt) and
Petit FatFs read-only cost 3,552 (3,116 with the flags below). UzeFS as Basic
builds it (one file slot, tier 2, no listing, no seek past the end) links
7,444 bytes and takes 63 bytes of RAM; the Basic integration section has the
whole ROM. Basic builds with `-mcall-prologues`, which takes 1,420 bytes off
its ROM, and not with `-mrelax`, whose link relaxation breaks the kernel's
cycle-counted timing; `demos/Basic/AGENTS.md` has the measurements. UzeFS sizes
given "with `-mrelax -mcall-prologues`" show what compiler flags can do to the
library, not a build Basic uses.

### 1.4 Why a new library

- **Petit FatFs** has no code for allocating clusters, writing directory
  entries or freeing chains, which is most of what UzeFS adds. Its streaming
  read design and its Uzebox SD driver are kept.
- **FatFs** is built around a 512-byte sector buffer kept inside `FATFS`
  between calls, with a dirty flag. Taking that out touches nearly every
  function; what is left is a rewrite.

UzeFS is therefore new code that follows Petit FatFs's streaming model, takes its
Uzebox driver as the starting point, and follows FatFs's on-disk algorithms.


## 2. Goals and non-goals

**Goals**

- One volume on the SD card: a partitioned card (partition table in sector 0) or
  an unpartitioned one (boot sector in sector 0).
- FAT16 in version 1; FAT32 as a compile-time addition.
- Files in the root directory, 8.3 names.
- Open modes: read, write, create, create-and-truncate, create-exclusive,
  append.
- Up to `UZEFS_MAX_FILES` files open at once (default 4). Several handles may
  read the same file; a handle that writes has the file to itself.
- Read, write, seek, truncate, sync, close; delete, rename, file information;
  directory listing.
- Every call leaves the card consistent for Windows `chkdsk` and Linux
  `fsck.fat` except for the documented crash cases (the crash consistency
  section).
- Works on the CUzeBox emulator's card, including its `--sdhost` mode, and on
  real cards.

**Non-goals**

- Subdirectories: no creating, entering or removing them. They appear in listings
  flagged as directories; opening, deleting or renaming one is refused.
- Creating long file names. Files that a PC gave long names are seen under their
  8.3 alias; deleting or renaming one also removes its long-name entries.
- exFAT (so SDXC cards as formatted at the factory), FAT12, SDUC cards.
- More than one volume, formatting, volume labels.
- String I/O (`gets`, `printf`). Basic does its own formatting.
- Locking between tasks. There is one caller.


## 3. Function comparison with FatFs and Petit FatFs

— means not provided. (cfg) means behind a compile-time switch.

| Function | FatFs R0.16 | Petit FatFs R0.03a | UzeFS | Notes for UzeFS |
|---|---|---|---|---|
| Mount volume | `f_mount` | `pf_mount` | `uzefs_mount` | Runs the SD card init; one volume |
| Unmount | `f_mount(NULL)` | — | `uzefs_unmount` | Closes files, writes a kept sector |
| Lend / reclaim buffer | — (inside `FATFS`) | — | `uzefs_lend`, `uzefs_unlend` | The host lends the 512-byte buffer |
| Open / create file | `f_open` (read, write, create new, create always, open always, append) | `pf_open` (existing files only, one at a time) | `uzefs_open` | Read, write, create, create+truncate, create-exclusive, append; up to `UZEFS_MAX_FILES` at once; a writer is exclusive |
| Close | `f_close` | — | `uzefs_close` | Writes size, first cluster and date |
| Read | `f_read` | `pf_read` | `uzefs_read` | |
| Write | `f_write` | `pf_write` (overwrites only; cannot grow a file; zeroes the rest of the last sector) | `uzefs_write` | Grows files, allocates clusters |
| Seek | `f_lseek` (past the end extends the file) | `pf_lseek` (within the file) | `uzefs_seek` | Past the end: extends and zero-fills with `UZEFS_SEEK_EXTEND`, otherwise refused |
| Rewind | `f_rewind` (macro) | — | `uzefs_rewind` (macro) | `uzefs_seek(fh, 0)` |
| Truncate at position | `f_truncate` | — | `uzefs_truncate` | |
| Flush | `f_sync` | — | `uzefs_sync` | |
| Position / size / end of file | `f_tell`, `f_size`, `f_eof` | fields of `FATFS` | `uzefs_tell`, `uzefs_size`, `uzefs_eof` | |
| Error flag | `f_error` | — | — | Each call returns its result; a handle that hit a disk error stays failed until closed |
| Delete file | `f_unlink` | — | `uzefs_delete` | Also removes the file's long-name entries |
| Rename | `f_rename` (also moves between directories) | — | `uzefs_rename` | In place, root only; an existing target is refused |
| File information | `f_stat` | — | `uzefs_stat` | Size, attributes, date and time |
| Open directory | `f_opendir` | `pf_opendir` | `uzefs_opendir` | Root only, no path |
| Read directory entry | `f_readdir` | `pf_readdir` | `uzefs_readdir` | Skips deleted, long-name and volume label entries; flags subdirectories |
| Rewind directory | `f_rewinddir` | `pf_readdir(NULL)` | `uzefs_readdir(d, NULL)` | |
| Close directory | `f_closedir` | — | — | A directory position holds no resources |
| Find by wildcard | `f_findfirst`, `f_findnext` | — | — | Basic filters `FILES "*.BAS"` itself; can be added later as `UZEFS_FIND` |
| Free space | `f_getfree` | — | `uzefs_getfree` (cfg) | Scans the whole FAT |
| Make / remove directory | `f_mkdir`, `f_rmdir` | — | — | Root only |
| Change directory or drive; current directory | `f_chdir`, `f_chdrive`, `f_getcwd` | — | — | One drive, root only |
| Change attributes | `f_chmod` | — | — | The read-only attribute is honoured by write, delete and rename |
| Set timestamp | `f_utime` | — | — | Dates come from the `uzefs_time` hook |
| Volume label | `f_getlabel`, `f_setlabel` | — | — | |
| Format / partition | `f_mkfs`, `f_fdisk` | — | — | Format on a PC |
| Pre-allocate contiguous space | `f_expand` | — | — | |
| Stream data to a function | `f_forward` | forwarding in `disk_readp` | — (used internally for scans) | |
| Fast seek via a cluster map | `f_lseek` with a map table | — | — | Tier 1 and tier 2 make chain walks cheaper |
| String I/O | `f_gets`, `f_putc`, `f_puts`, `f_printf` | — | — | |
| Long names, code pages | `FF_USE_LFN`, `f_setcp` | — | — | 8.3 names, ASCII |
| Locking, re-entrancy | `FF_FS_LOCK`, `FF_FS_REENTRANT` | — | — | Open-file conflicts are refused |
| FAT12 | yes | (cfg) | — | |
| FAT16 | yes | (cfg) | yes | |
| FAT32 | yes | (cfg) | (cfg) `UZEFS_FAT32`, after version 1 | |
| exFAT | (cfg) | — | — | |
| Disk layer | `disk_initialize`, `disk_status`, `disk_read`, `disk_write`, `disk_ioctl` | `disk_initialize`, `disk_readp`, `disk_writep` | `uzefs_sd_init`, `uzefs_sd_read`, `uzefs_sd_write`, `uzefs_sd_zero` | Derived from Petit FatFs's Uzebox `diskio.c` |
| Time source | `get_fattime` | — | `uzefs_time` | Weak default returns a fixed date |

Nothing the objectives ask for is missing. `uzefs_stat` and `uzefs_truncate` go
beyond them: Basic needs the first to check whether a file exists, and the second
costs little once freeing a chain exists for delete.


## 4. Why a 512-byte buffer is unavoidable for writes

### 4.1 The constraints

1. **A card write is a whole 512-byte block.** `CMD24` takes exactly 512 data
   bytes. Only SDSC cards accept a shorter block length (`CMD16`); SDHC and
   SDXC cards are fixed at 512, and the CUzeBox emulator ignores `CMD16`.
2. **Chip select stays low for the whole block.** The SD specification requires
   it for the whole transaction. The emulator discards a block whose chip select
   is released partway through, as a real card would leave it unwritten.
3. **SPI RAM shares the bus.** Uzebox Basic keeps its program in SPI RAM and
   uses it between any two library calls. So a card transaction cannot be left
   open across calls, and bytes cannot be fetched from SPI RAM in the middle of
   one.

### 4.2 The consequence

A write that changes *part* of a sector must send the unchanged bytes too, and
they must be in AVR RAM when the write starts. That covers:

- A FAT entry: 2 of 512 bytes (4 on FAT32).
- A directory entry: 32 of 512 bytes.
- An append into a partly filled sector, and any write after a seek.

Petit FatFs gets around this by never changing metadata and zeroing the rest of
the sector, which is why it cannot create or grow a file. The fattest test ROM in
this repository hit the same limit: its driver moves sectors between SPI RAM and
the card through a 512-byte AVR buffer.

What stays buffer-free: opening, reading, seeking, listing and the free-cluster
search. Those only read, and reads can take any part of a sector with one `CMD17`.

### 4.3 The least RAM that works

UzeFS keeps **no sector between calls** in its default build. A write-type call
uses a 512-byte buffer the host has lent it, only while the call runs. Basic
already has one: `sys_mem.vm_scratch.scratch_buf[512]`.

### 4.4 Rejected: rebuilding sectors from a smaller description

A FAT sector on a card that was written file by file is mostly runs of "next
cluster = this + 1", so it could be described in far fewer than 512 bytes, and
rebuilt byte by byte during the write. Rejected because:

- Directory sectors are names and sizes, which do not compress.
- A partly filled data sector cannot be rebuilt at all.
- Whether a write works would depend on how fragmented the card is. A `KILL`
  that fails on one card and works on another is worse than a fixed cost.
- The encoder and decoder would cost flash, which is also scarce.


## 5. Memory model

### 5.1 Tiers

| Tier | Selected by | RAM | What gets faster |
|---|---|---|---|
| 0, minimum (default) | nothing | 29 bytes for the volume + 26 per file slot: **133 with 4 slots**, 55 with 1. The 512-byte buffer is lent only while a write-type call runs | — Petit FatFs speed: one `CMD17` per directory sector scanned, per FAT sector scanned, per FAT entry followed and per sector touched by a read |
| 1, read buffer | `UZEFS_RDBUF` = *n* at compile time (default 0) | +7 + *n*, static | Reads shorter than *n* bytes, including each FAT entry followed during a seek, are served from the buffer, which is refilled with the *n* bytes from the first one it lacks |
| 2, kept sector | `UZEFS_KEEP=1` at compile time, and the host lends with `keep` set | +5; the host leaves its 512 bytes lent until it calls `uzefs_unlend` | Every sector read or written, except a whole sector read into or written from the caller's buffer, goes through the lent buffer and stays there; changes to it reach the card when another sector needs the buffer: about FatFs `FF_FS_TINY` speed |
| Always | — | 4 bytes of the volume figure | The free-cluster hint |

Measured in the harness on the emulator, in frames (60 a second), with the
screen reduced to two text rows so the video takes little CPU:

| | Tier 0 | Tier 1 (32 bytes) | Tier 2 |
|---|---|---|---|
| 500 reads of 8 bytes | 27 | 7 | 1 |
| 50 seeks to byte 98,000 of a 4-cluster file and back | 5 | 1 | 0 |
| 200 writes of 20 bytes, then close | 1,294 | 1,294 | 93 |
| The whole harness run | 18,299 | 18,273 | 16,690 |

Tier 1 does nothing for writes. At tier 0 each of those writes reads and
writes its data sector; tier 2 reads it once and writes it once, when the
next sector is needed. How much of tier 0's time is the emulator's model of
card write delay was not measured. Both tiers together measure the same as
tier 2.

Streaming scans are what keep tier 0 usable. The driver can pass each byte of a
sector to a handler as it arrives (Petit FatFs's forwarding mode), so finding a
name, a free directory slot or a free cluster reads a whole sector with one
`CMD17` and a few bytes of state. Reading one entry at a time instead would cost
one `CMD17`, 514 bytes clocked, for each of the 512 FAT16 root entries or 256
entries in each FAT sector.

### 5.2 Lending rules

`uzefs_lend(buf, keep)`:

- `keep = 0`: the buffer belongs to the host between calls. UzeFS uses it only
  inside a call and never assumes its contents survive. The host may lend once
  and use the buffer freely afterwards; no `uzefs_unlend` is needed.
- `keep = 1` (only with `UZEFS_KEEP=1`, and a non-NULL buffer; otherwise
  treated as 0): the buffer belongs to UzeFS until `uzefs_unlend()`. It may
  hold a sector that still has to be written. `uzefs_unlend()` writes it and
  returns the result, so it can fail with a disk error; the buffer is given
  back either way.
- `uzefs_lend` first does what `uzefs_unlend` does, so lending again, with or
  without `keep`, writes a kept sector and returns that result.
- `uzefs_unlend` also makes UzeFS forget what the read buffer and the kept
  sector hold. A host that writes to the card other than through UzeFS calls
  it (or lends again) first, and makes no UzeFS call between its own writes.
- A write-type call with nothing lent returns `UZEFS_ERR_NOBUF` and changes
  nothing.
- Data passed to `uzefs_write` must not lie inside the lent buffer, and a
  buffer passed to `uzefs_read` must not either.
- Reads never need a lent buffer. In tier 2 they use it if it is lent.

Write-type calls: `uzefs_open` with write, create or truncate; `uzefs_write`;
`uzefs_sync`; `uzefs_close` of a handle that wrote; `uzefs_truncate`;
`uzefs_seek` past the end with `UZEFS_SEEK_EXTEND`; `uzefs_delete`;
`uzefs_rename`; `uzefs_unmount` with a writer open.

### 5.3 Same results at every tier

Every call returns the same result and leaves the same card contents at every
tier, once `uzefs_sync`, `uzefs_close` or `uzefs_unlend` has returned. Between
those points a tier 2 build may not have written the last sector yet. That matters
only for a crash (the crash consistency section) and for a disk error: if
writing the kept sector fails, its changes are dropped, and the call that
needed the buffer returns `UZEFS_ERR_DISK`. Changes an earlier call reported
as done can be lost that way, where tier 0 would have written them or failed
that earlier call. Dropping them keeps one failed write from failing every
later call.

Sync writes the kept sector even for a handle with nothing to update, so
after any successful `uzefs_sync` or `uzefs_close` the card holds everything.

### 5.4 Stack

Measured by filling the stack with a known pattern and running one call, the
deepest calls go this far below their caller's frame. An interrupt that
lands during the call is counted too, so runs differ by up to about 25 bytes;
the figures are the larger of two runs.

| Call | Tier 0 | Tier 1 (32 bytes) | Tier 2 | Both |
|---|---|---|---|---|
| `uzefs_open` with `UZEFS_O_TRUNC` (the lookup's scan state stays on the stack while the chain is freed) | 192 | 190 | 218 | 220 |
| `uzefs_rename` | 163 | 163 | 186 | 188 |
| `uzefs_delete` | 150 | 149 | 177 | 179 |
| `uzefs_write` allocating a cluster | 134 | 132 | 157 | 159 |
| `uzefs_truncate` | 105 | 121 | 131 | 133 |
| Deepest point of the whole harness run, its own frames included | 256 | 254 | 282 | 284 |

Tier 2 goes about 25 bytes deeper, for writing the kept sector from inside
the call that needs the buffer.

Basic leaves 785 bytes of RAM for the stack, 652 of them below its file test
suite's frame. Its LOAD path reaches 491 bytes below that frame and SAVE 454,
interrupts included (the Basic integration section).


## 6. Card classes and FAT types

### 6.1 What exists

| Card class | Capacity | How sectors are addressed | Factory format | UzeFS |
|---|---|---|---|---|
| SDSC | up to 2 GB | by byte: `CMD17`/`CMD24` argument = sector × 512. Old version 1 cards do not answer `CMD8` | FAT16 (FAT12 below about 64 MB) | **Version 1** |
| SDHC | 4–32 GB | by sector; the card reports this with the `CCS` bit after `ACMD41` | FAT32 | Yes: FAT16 formatted since version 1, FAT32 with `UZEFS_FAT32` |
| SDXC | 64 GB–2 TB | same as SDHC | exFAT | Only if reformatted to FAT32; treated as SDHC |
| SDUC | above 2 TB | — | exFAT | Out of scope |

The card class and the FAT type are independent: the formatting decides the FAT
type. An SDSC card can be formatted FAT32 and an SDHC card up to 4 GB can be
formatted FAT16.

**The SDHC driver code already exists.** Petit FatFs's Uzebox `diskio.c`
already detects SDHC and switches to sector addressing (`CT_BLOCK`). UzeFS keeps
it, so `UZEFS_SDHC` defaults to 1. Setting it to 0 strips `CMD8` and `CCS`
handling, about 80–120 bytes, for an SDSC-only build. SDHC is tested with FAT32
after version 1. An SDHC card of 4 GB or less formatted FAT16 could test it
earlier.

The emulator's card is SDSC, byte-addressed, 2 GB, FAT16 with 32 KB clusters and
no partition table.

### 6.2 Rules that keep FAT32 an addition

Version 1 followed these rules so that adding FAT32 added code without changing the
existing code. They held: what was built is in 11.1–11.4, and the table below is the design
as written. Where the build differs: FSInfo's next-free hint is not read (the hint starts at 2
on both types); FAT entries go through `fat_loc` (the sector and offset) shared by `fat_next`
and `fat_put` rather than a `fat_get`; `dir_sect` is the root's first *sector* on both types.

| Rule in version 1 | What FAT32 adds later |
|---|---|
| Sector numbers and cluster numbers are 32-bit everywhere, in structures and arguments | Nothing: FAT32 cluster numbers are 28 bits |
| One function in the driver, `addr`, turns a sector number into a card argument | Already done by the driver |
| The root directory is reached only through a directory position (`uzefs_dir`: sector and index), with one "next entry" function | "Next entry" follows the root directory's cluster chain instead of a fixed area, and extends the chain with a zero-filled cluster when full (`uzefs_sd_zero` exists in version 1 for seek extension) |
| FAT entries are read and written only by `fat_get` / `fat_put`, which switch on entry width | 4-byte entries, top 4 bits kept on write |
| The free-cluster scan is a streaming handler over FAT sectors | Same code with a 4-byte entry width; the FAT is much larger, so the hint matters more |
| Mount decides the FAT type from the cluster count, in one place | Reads the FAT32 boot sector fields and the root cluster |
| A "first write since mount" flag exists in the volume | FSInfo's free count is set to "unknown" (0xFFFFFFFF) once; its next-free hint is read at mount |
| File handles store the directory entry's sector number, not a root index | Nothing |


## 7. API

### 7.1 Types and results

```c
typedef uint8_t uzefs_res;

typedef struct {
    char     name[13];      /* "NAME.EXT", NUL-terminated                     */
    uint8_t  attr;          /* FAT attribute byte; UZEFS_AM_DIR for directories */
    uint32_t size;
    uint16_t date, time;    /* FAT packed, last modified                       */
} uzefs_info;               /* 22 bytes, caller-owned                         */

typedef struct {
    uint32_t sector;        /* sector of the next entry to look at             */
    uint8_t  index;         /* 0..15 within that sector                        */
} uzefs_dir;                /* 5 bytes, caller-owned                          */
```

| Result | Meaning | Basic error |
|---|---|---|
| `UZEFS_OK` | success | — |
| `UZEFS_ERR_DISK` | the card refused or failed a command, or returned an error token | 57 Device I/O error |
| `UZEFS_ERR_NOT_READY` | no card, or the card did not initialise | 71 Disk not ready |
| `UZEFS_ERR_NO_FS` | no FAT16 volume (or FAT32 with `UZEFS_FAT32`) | 57 Device I/O error |
| `UZEFS_ERR_CORRUPT` | a cluster chain points outside the volume or loops | 57 Device I/O error |
| `UZEFS_ERR_NOT_MOUNTED` | call before `uzefs_mount` succeeded | 71 Disk not ready |
| `UZEFS_ERR_NO_FILE` | name not found | 53 File not found |
| `UZEFS_ERR_EXISTS` | create-exclusive or rename target already exists | 58 File already exists |
| `UZEFS_ERR_BAD_NAME` | name not valid as 8.3 | 64 Bad file name |
| `UZEFS_ERR_DENIED` | read-only file opened for writing, deleted or renamed; a directory or volume label targeted | 70 Permission denied |
| `UZEFS_ERR_LOCKED` | the file is open in a conflicting way | 55 File already open |
| `UZEFS_ERR_TOO_MANY` | no free file slot | 67 Too many files |
| `UZEFS_ERR_DIR_FULL` | no free root directory entry | 67 Too many files |
| `UZEFS_ERR_FULL` | no free cluster | 61 Disk full |
| `UZEFS_ERR_BAD_HANDLE` | handle out of range or not open | 52 Bad file number |
| `UZEFS_ERR_MODE` | write, or seek past the end, on a handle opened for reading; read on one opened for writing only; an invalid mode combination | 54 Bad file mode |
| `UZEFS_ERR_RANGE` | seek past the end without `UZEFS_SEEK_EXTEND` | 5 Illegal function call |
| `UZEFS_ERR_NOBUF` | write-type call with no buffer lent | 51 Internal error |

Basic's `errors.h` defines 51, 53, 57, 61, 64, 67, 70 and 71. It maps the
results LOAD and SAVE cannot produce with one file slot and no OPEN (exists,
locked, bad handle, mode, range, no buffer) to 51; 52, 54, 55 and 58 are added
with the commands that can produce them.

### 7.2 Calls

```c
uzefs_res uzefs_mount(void);
uzefs_res uzefs_unmount(void);             /* syncs and closes every file       */

uzefs_res uzefs_lend(uint8_t *buf, uint8_t keep);
uzefs_res uzefs_unlend(void);              /* writes a kept sector if pending   */

uzefs_res uzefs_open(uint8_t *fh, const char *name, uint8_t mode);
uzefs_res uzefs_close(uint8_t fh);
uzefs_res uzefs_sync(uint8_t fh);
uzefs_res uzefs_read (uint8_t fh, uint8_t *buf,       uint16_t n, uint16_t *done);
uzefs_res uzefs_write(uint8_t fh, const uint8_t *buf, uint16_t n, uint16_t *done);
uzefs_res uzefs_seek(uint8_t fh, uint32_t pos);
uzefs_res uzefs_truncate(uint8_t fh);      /* cut the file at the current position */
uint32_t  uzefs_tell(uint8_t fh);
uint32_t  uzefs_size(uint8_t fh);
uint8_t   uzefs_eof(uint8_t fh);
#define   uzefs_rewind(fh) uzefs_seek((fh), 0)

uzefs_res uzefs_delete(const char *name);
uzefs_res uzefs_rename(const char *from, const char *to);
uzefs_res uzefs_stat(const char *name, uzefs_info *fi);   /* fi may be NULL: existence check */

uzefs_res uzefs_opendir(uzefs_dir *d);
uzefs_res uzefs_readdir(uzefs_dir *d, uzefs_info *fi);    /* fi NULL: rewind; end: fi->name[0] == 0 */

uzefs_res uzefs_getfree(uint32_t *clusters, uint16_t *cluster_kb);  /* UZEFS_GETFREE */

/* streaming (11.16) */
uzefs_res uzefs_fragments(uint8_t fh, uint16_t *n);          /* pieces the file is in        */
uzefs_res uzefs_extent(uint8_t fh, uint32_t pos, uint32_t *sect, uint32_t *run);
uzefs_res uzefs_stream_gap(uint8_t n, uint16_t *min, uint16_t *max);
uzefs_res uzefs_stream_begin(uint32_t sect);                 /* CMD18, the card left selected */
uzefs_res uzefs_stream_end(void);                            /* CMD12, busy, deselected      */
uzefs_res uzefs_stream_restart(uint32_t sect, uint16_t busy_max);  /* the read moved to sect */

uint32_t  uzefs_time(void);   /* weak; FAT packed date << 16 | time */
```

A handle is a small number, `0 .. UZEFS_MAX_FILES-1`, so Basic's `#1` to `#4`
map straight onto it. `uzefs_read` returns `UZEFS_OK` with `*done < n` at the
end of the file.

### 7.3 Open modes

| Flag | Effect |
|---|---|
| `UZEFS_O_READ` | reading allowed |
| `UZEFS_O_WRITE` | writing allowed |
| `UZEFS_O_CREATE` | create the file if missing |
| `UZEFS_O_EXCL` | fail with `UZEFS_ERR_EXISTS` if the file exists |
| `UZEFS_O_TRUNC` | cut an existing file to 0 bytes on open |
| `UZEFS_O_APPEND` | position at the end after opening |

The flags carry an `O_` so they cannot collide with the `UZEFS_WRITE` build
option. At least one of `UZEFS_O_READ` and `UZEFS_O_WRITE` is required, and
`UZEFS_O_CREATE`, `UZEFS_O_EXCL`, `UZEFS_O_TRUNC` and `UZEFS_O_APPEND` need
`UZEFS_O_WRITE`; otherwise `UZEFS_ERR_MODE`.

Conflicts:
- A file open for writing cannot be opened again in any mode.
- A file open for reading cannot be opened for writing.
- An open file cannot be deleted or renamed.

All of these return `UZEFS_ERR_LOCKED`. Files are identified by the sector and
index of their directory entry.

### 7.4 Names

- `NAME.EXT` with 1–8 name characters and 0–3 extension characters. A name with
  no extension has no dot.
- Lower case is converted to upper case (ASCII only, no table).
- Refused with `UZEFS_ERR_BAD_NAME`:
  - control characters, space, and `" * + , / : ; < = > ? [ \ ] |`
  - bytes 0x80 and above
  - a leading dot
  - an empty name
  - more than 8 or 3 characters
- A first character of 0xE5 cannot occur, since high bytes are refused.
- Names are written with byte 12 of the entry (the lower-case flags Windows uses)
  set to 0, so a PC shows them in upper case.


## 8. Configuration

All macros are `#ifndef`-guarded in `uzefs_conf.h`, so a project sets them from
its Makefile, as Basic does.

| Macro | Default | Effect |
|---|---|---|
| `UZEFS_MAX_FILES` | 4 | File slots, 26 bytes each. At least 1 |
| `UZEFS_WRITE` | 1 | 0 gives a read-only build: mount, open for reading, read, seek, stat, directory listing; no lending |
| `UZEFS_FAT32` | 0 | FAT32 support. One build mounts FAT16 or FAT32; costs about 1.4 KB, most of it 32-bit cluster numbers |
| `UZEFS_ROOT_GROW` | 1 | With `UZEFS_FAT32`: a create in a full FAT32 root directory gives it another cluster. 0 refuses it with `UZEFS_ERR_DIR_FULL`, as a full FAT16 root does (188 bytes in Basic) |
| `UZEFS_FSINFO` | 1 | With `UZEFS_FAT32`: the first FAT change after mount marks FSInfo's free count unknown. 0 leaves a stale count for a PC to show until `chkdsk` (214 bytes and 4 of RAM in Basic) |
| `UZEFS_SDHC` | 1 | SDHC sector addressing and `CMD8`/`CCS` init; 0 for SDSC only |
| `UZEFS_RDBUF` | 0 | Tier 1 read buffer size in bytes, 0 = none, at most 255. 16–128 is sensible |
| `UZEFS_KEEP` | 0 | Tier 2: allow `uzefs_lend(buf, 1)` to keep a sector. Forced to 0 when `UZEFS_WRITE` is 0. With both tiers on, a kept sector serves reads while it is lent with `keep`, the read buffer otherwise |
| `UZEFS_DIR` | 1 | `uzefs_opendir` / `uzefs_readdir` |
| `UZEFS_APPEND` | 1 | `UZEFS_O_APPEND` honoured. 0 refuses it with `UZEFS_ERR_MODE`, so a host that never seeks does not link `uzefs_seek` |
| `UZEFS_INFO_NAME_ONLY` | 0 | 1: `uzefs_readdir` and `uzefs_stat` fill only the name and attributes of `uzefs_info` |
| `UZEFS_SEEK_EXTEND` | 1 | Seek past the end extends the file with zeros; 0 refuses it |
| `UZEFS_GETFREE` | 0 | Declares `uzefs_getfree`. **Not implemented** in `uzefs.c` (2026-09-26): a call fails to link |
| `UZEFS_SIZE_EVERY_WRITE` | 0 | Update the directory entry's size at the end of every `uzefs_write`, not only at sync and close. Costs one directory read and write per call |
| `UZEFS_FIXED_DATE` | 2026-01-01 00:00 | Date the weak `uzefs_time` returns |


## 9. Data structures

### 9.1 Volume, one static instance

| Field | Bytes | Notes |
|---|---|---|
| `flags` | 1 | mounted, FAT type, first write done |
| `cshift` | 1 | log2 of sectors per cluster, 0–7 |
| `cmask` | 1 | sectors per cluster − 1 |
| `nfats` | 1 | 1 or 2 |
| `fat_sect` | 4 | first sector of the first FAT |
| `fat_size` | 4 | sectors per FAT; the second FAT starts at `fat_sect + fat_size` |
| `dir_sect` | 4 | first root directory sector, on both types (FAT32: the first sector of `BPB_RootClus`) |
| `data_sect` | 4 | first sector of cluster 2; on FAT16 also where the root directory ends |
| `max_clust` | 4 | number of clusters + 2 |
| `free_hint` | 4 | where the next free-cluster scan starts; write builds only |
| `win` | 2 | the lent buffer, or NULL; write builds only |
| **total** | **30** (24 read-only) | `fat_size`, `max_clust` and `free_hint` are 2 bytes each without `UZEFS_FAT32` |
| `fsi_sect` | +4 | `UZEFS_FAT32` and `UZEFS_FSINFO` only: the FSInfo sector |
| `win_sect`, `wstate` | +5 | tier 2 only: sector held, and kept / holds a sector / has changes waiting |
| `rd_sect`, `rd_ofs`, `rd_len` + buffer | +7 + *n* | tier 1 only: bytes `rd_ofs` to `rd_ofs + rd_len` of sector `rd_sect`; `rd_len` 0 when empty |

### 9.2 File slot, `UZEFS_MAX_FILES` instances

| Field | Bytes | Notes |
|---|---|---|
| `flags` | 1 | open, may read, may write, entry needs updating, failed |
| `dir_index` | 1 | 0–15 |
| `dir_sect` | 4 | sector holding the directory entry |
| `sclust` | 4 | first cluster, 0 for an empty file |
| `cclust` | 4 | the cluster holding byte `fptr - 1`; 0 when `fptr` is 0 |
| `csect` | 4 | the sector holding byte `fptr - 1`; 0 when `fptr` is 0 |
| `fptr` | 4 | position |
| `fsize` | 4 | size |
| **total** | **26** | |

`csect` lets read, write and zero-fill share one routine that, at each sector
boundary, either steps to the next sector of `cclust` or moves to the next
cluster, instead of recomputing the sector from `fptr` in 32-bit arithmetic.

### 9.3 Driver

One byte, the card type (`CT_SD1`, `CT_SD2`, `CT_BLOCK`), as in Petit FatFs.


## 10. SD driver layer

`kernel/uzefs/uzefs_sd.c`, derived from `kernel/petitfatfs/diskio.c`. That file
stays untouched for the demos that use it.

| Function | From Petit FatFs | Change |
|---|---|---|
| `uzefs_sd_init` | `disk_initialize` | SDHC branch under `UZEFS_SDHC`. The stale `_FS_USE_WRITE` guard at `diskio.c:189` goes, as UzeFS never leaves a write open. Ends at the double-speed SPI setting `SpiRamInit` uses, as today |
| `uzefs_sd_read(buf, sect, ofs, cnt)` | `disk_readp` | With `buf` NULL, each byte goes to the scan handler in the variable `uzefs_sd_handler` instead of `FORWARD`. The library feeds a kept sector to the same handler |
| `uzefs_sd_write(buf, sect)` | `disk_writep`, all three phases in one call | Sends 512 bytes from `buf`, or 512 zeros when `buf` is NULL, and checks the data response and busy wait |
| `uzefs_sd_zero(sect)` | new | A macro for `uzefs_sd_write(NULL, sect)`; needs no buffer |
| `uzefs_sd_gap(sect, n, min, max)` | new | Times a CMD18 of n + 1 sectors: the bytes between sectors |
| `uzefs_sd_stream_begin(sect)`, `uzefs_sd_stream_end()` | new | CMD18 with the card left selected for the host; CMD12, its busy, and the deselect |
| `uzefs_sd_reg(which, buf)` | new | The CID (0) or the CSD (1), 16 bytes |
| `addr(sect)`, static | the `lba *= 512` lines | One place |

Every driver call returns a `uzefs_res`: `UZEFS_OK`, `UZEFS_ERR_DISK`, or
`UZEFS_ERR_NOT_READY` from init, so the library passes results on unchanged.

Rules:

- **SPI RAM chip select.** Every command first raises PA4, the SPI RAM chip
  select, as well as toggling PD6 for the card. Basic's notes call PA4 "the live
  hazard" because Petit FatFs's driver never raises it. The host must still not
  hold an SPI RAM sequential transfer open across a UzeFS call, because raising
  PA4 ends it.
- **SPIF is cleared before every command.** The SPI RAM routines time their
  bytes by counting cycles and leave SPIF set. With SPIF set, the wait after
  a byte returns while the byte is still going out, and the next write to SPDR
  is dropped, which can be the command byte. So `send_cmd` and `uzefs_sd_init`
  read SPSR and then SPDR before using the bus. The emulator does not drop such
  a write; only a Uzebox shows the fault.
- **Every call ends with the card deselected** and one byte clocked, except
  `uzefs_sd_stream_begin`, which hands the selected card to the host.
- **Init first ends any read the card is still in.** A card left in a CMD18
  (the console reset during a stream, or the ISP programmer's traffic while the
  ATmega644 is held in reset, its chip selects floating) takes no command,
  CMD0 included, until a CMD12 ends that read. So `uzefs_sd_init` sends CMD12
  after the 80 clocks, waits for its R1 (10 bytes at most) and its busy (4,000
  bytes, 0.15 s at the slow clock), then goes on to CMD0. A card that is not in
  a read answers it as an illegal command, or not at all.
- **Error tokens count as failures.** The read waits for the 0xFE start token;
  anything else, including the emulator's 0x01 error token for a failed card, is
  `UZEFS_ERR_DISK`. A write whose data response is not `xxx00101` is
  `UZEFS_ERR_DISK`; that covers CRC error (0x0B) and write error (0x0D).
- **No CRC.** The driver sends a dummy CRC and does not enable CRC checking
  (`CMD59`). That is Petit FatFs's behaviour, and the emulator only checks CRCs
  when a ROM turns them on.


## 11. On-disk algorithms

### 11.1 Mount

1. `uzefs_sd_init`; failure → `UZEFS_ERR_NOT_READY`.
2. Read sector 0. No 0x55 0xAA at 510 → `UZEFS_ERR_NO_FS`.
3. If it is a boot sector (a jump byte 0xEB, 0xE9 or 0xE8, 512 bytes per sector,
   sectors per cluster a power of two, 1 or 2 FATs), use it: this is the
   emulator's layout. Otherwise treat it as a partition table. Take the first
   entry of type 0x04, 0x06 or 0x0E (FAT16), or 0x0B / 0x0C with `UZEFS_FAT32`,
   and read its first sector as the boot sector.
4. Compute the cluster count from the boot sector. Fewer than 4,085 is FAT12 →
   `UZEFS_ERR_NO_FS`. 65,525 or more is FAT32 → accepted only with
   `UZEFS_FAT32`. The emulator's card has 65,518 and so is FAT16.
   - FAT16 must have a fixed root area (`BPB_RootEntCnt` ≠ 0) and a FAT big
     enough for every cluster; FAT32 must have none, and 128 entries a FAT
     sector.
   - FAT32 reads `BPB_FATSz32` (36), `BPB_RootClus` (44), which must be a
     cluster of the volume, and `BPB_FSInfo` (48); mount reads bytes 0..49.
5. Fill the volume structure. `free_hint = 2`, on both types: FSInfo's
   next-free hint is not read.

Only fields needed are read, with `uzefs_sd_read` into locals, so mount needs no
buffer. The "volume dirty" bit in FAT entry 1 is neither read nor set.

### 11.2 FAT entries

- `fat_next(cl)`: a read of 2 bytes at `fat_sect + cl / 256`, offset
  `(cl % 256) * 2` (4 bytes and 128 per sector on FAT32), served by the tiers
  as any read is (the read buffer and kept sector section).
- `fat_put(cl, value)`: load that FAT sector into the lent buffer, set the
  entry, write the sector to **every FAT copy**. Each call is its own
  read-modify-write at tier 0.
- End of chain is written as 0xFFFF, or 0x0FFFFFFF on FAT32. A FAT32 entry is
  28 bits: the top 4 are masked off on read and kept as they were on write.
- **FSInfo** (`UZEFS_FSINFO`): the first `fat_put` on a FAT32 volume since
  mount loads the FSInfo sector, and if its signature at 484 is `0x61417272`,
  sets the free count at 488 to 0xFFFFFFFF (unknown). The next-free hint and
  the backup copy are left alone. It is the only sector before the FAT that is
  ever written, so `sect_put` writes it once, not to every FAT copy.
- A value of 0 or 1, above `max_clust` and not end-of-chain, or equal to the
  cluster itself, when followed inside a chain → `UZEFS_ERR_CORRUPT`. A longer
  loop cannot hang a read or seek, because both stop at the file size; freeing
  a looped chain reaches an entry it has already set to 0, which is refused.

### 11.3 Allocation

- The scan streams FAT sectors from `free_hint` through the scan handler,
  stopping at the first zero entry. It wraps to cluster 2 once. Nothing found →
  `UZEFS_ERR_FULL`.
- Taking a cluster: write end-of-chain into it; `free_hint` moves past it. It
  joins the file only after its first sector of data (the write section).
- Freeing clusters below `free_hint` moves the hint back to the lowest one freed.

### 11.4 Root directory

- FAT16: the sectors from `dir_sect` up to `data_sect`, 16 entries per sector.
  FAT32: a cluster chain from `BPB_RootClus`, followed through the FAT.
  `uzefs_dir` walks either by sector and index: `dir_next` steps to the next
  sector, through the chain at a cluster's last one, and gives 0 at the end.
  An index of 16 means "the next sector", so a listing resumes after an entry
  without stepping the chain itself.
- **Growing the FAT32 root** (`UZEFS_ROOT_GROW`): a create that finds no free
  entry takes a new cluster (marked end-of-chain), zeroes all its sectors,
  then joins it to the chain's last cluster, and creates the file in its
  first entry. A crash in between leaves at worst a lost cluster. FAT16's root
  is fixed, so there it is `UZEFS_ERR_DIR_FULL`.
- **Lookup** streams each directory sector through the scan handler, which in one
  pass records:
  - the entry whose 11 name bytes match
  - the first free entry (first byte 0x00 or 0xE5)
  - where the run of long-name entries just before the current entry began
  - whether a 0x00 entry was seen, which ends the scan
- Entries with attribute 0x0F (long name) and 0x08 (volume label) never match.
  A directory (0x10) matches, but only `uzefs_stat` and listing accept it.
- **Create** writes into the first free entry: name, attribute 0x20 (archive),
  byte 12 = 0, creation and modification date and time from `uzefs_time`, first
  cluster 0, size 0, and on FAT32 the first cluster's high word at 20. No free
  entry → `UZEFS_ERR_DIR_FULL` (on FAT32 without `UZEFS_ROOT_GROW`).
- **Removing long-name entries.** When a file is deleted or renamed, the entries
  of attribute 0x0F directly before it are marked 0xE5. The lookup already
  records where that run starts. It may start in an earlier sector, which costs
  one more read-modify-write per sector. The run's checksum is not checked: an
  entry of attribute 0x0F directly before the file is removed either way.

### 11.5 Open

- Check the mode (`UZEFS_ERR_MODE`) and, if it writes, that a buffer is lent
  (`UZEFS_ERR_NOBUF`).
- Look the name up.
- Found:
  - A directory or volume label → `UZEFS_ERR_DENIED`.
  - Read-only, and the mode writes → `UZEFS_ERR_DENIED`.
  - `UZEFS_O_EXCL` → `UZEFS_ERR_EXISTS`.
  - Check conflicts.
  - First cluster 1, at or above `max_clust`, or 0 with a non-zero size →
    `UZEFS_ERR_CORRUPT`.
- Not found: without `UZEFS_O_CREATE` → `UZEFS_ERR_NO_FILE`.
- Take a slot; none free → `UZEFS_ERR_TOO_MANY`.
- Not found: create (no free entry → `UZEFS_ERR_DIR_FULL`).
- `UZEFS_O_TRUNC` on a file with a first cluster: truncate at position 0
  (below).
- `UZEFS_O_APPEND`: seek to the end.

### 11.6 Read and seek

- `uzefs_read` limits `n` to `fsize - fptr`, and reads the part of each sector
  it touches, at tier 0 straight into the caller's buffer (the read buffer and
  kept sector section for tiers 1 and 2).
- At each sector boundary the next sector is `csect + 1` inside the cluster, or
  the first sector of the next cluster, found with `fat_next`. Read, write and
  zero-fill share this step.
- `uzefs_seek(pos)`, with `pos <= fsize`, walks the chain from `sclust`. With
  clusters of 16–32 KB on FAT16 cards a chain is short, so walking from the
  current position instead is not worth its code.
- `pos > fsize`:
  - with `UZEFS_SEEK_EXTEND`, seek to the end and write zeros up to `pos`
    through the write path: `uzefs_sd_zero` for whole sectors, the write rules
    below for a partial one. A handle opened for reading only →
    `UZEFS_ERR_MODE`; nothing lent → `UZEFS_ERR_NOBUF`; both checked before the
    position moves;
  - without it, `UZEFS_ERR_RANGE`.

### 11.7 Write

For each sector the write touches:

1. **At a cluster boundary** (or the file has no cluster yet), take the next
   cluster from the chain, or allocate one if the chain ends there (allocation
   rules above). A new cluster joins the file only after step 2 has written
   its first sector: the directory entry gets the size so far and the date
   (and, for a file's first cluster, the cluster), then the cluster is linked
   from the previous one (crash consistency section). This costs one
   read-modify-write of the directory per cluster a write adds.
2. Write the data:
   - The chunk fills the whole sector: `uzefs_sd_write` straight from the
     caller's buffer, no copy.
   - The chunk starts at offset 0 of the sector and reaches the end of the file:
     the sector holds nothing worth keeping, so copy the chunk into the lent
     buffer, zero the rest, and write. No read.
   - Otherwise: read-modify-write through the lent buffer.
   - At tier 2 the last two go into the kept sector and are written later; a
     whole sector still goes straight to the card.
3. `fptr` advances; `fsize = max(fsize, fptr)`; the slot is marked as needing its
   entry updated.

At the end of the call, with `UZEFS_SIZE_EVERY_WRITE`, the directory entry is
updated as for sync.

A disk error marks the slot failed. Read, write, seek, truncate and sync on it
then return `UZEFS_ERR_DISK`. Closing it frees the slot, writes nothing, and
returns `UZEFS_ERR_DISK`.

### 11.8 Sync and close

- Sync: if the entry needs updating, read-modify-write its sector:
  - size
  - first cluster
  - modification date and time
  - archive attribute
- In tier 2, then write the kept sector if it has changes waiting, whether or
  not this handle's entry needed updating.
- Close syncs, then frees the slot.

### 11.9 Truncate

`uzefs_truncate` first checks what a write checks: the handle, a failed slot
(`UZEFS_ERR_DISK`), write mode (`UZEFS_ERR_MODE`) and a lent buffer
(`UZEFS_ERR_NOBUF`). Then, at `fptr`:

1. Unless `fptr` is 0, follow `cclust`, the cluster holding the last byte kept.
   If it links further, mark it end-of-chain.
2. Write the new size and the date, and first cluster 0 if `fptr` is 0, to the
   directory entry. The slot no longer needs its entry updated.
3. Free the clusters after `cclust`, or the whole chain if `fptr` is 0.

Cutting the chain before writing the size keeps the size from ever falling
short of the chain; at `fptr` 0 there is no chain left to cut, and the entry
must let go of the chain before it is freed.

The chain is cut wherever the size is, so clusters linked past the old size (a
crash can leave them) are freed too, and truncating at the end of the file frees
anything linked beyond it. A disk error marks the slot failed, as a write does.

### 11.10 Freeing a chain

For each cluster of the chain: read its entry to find the next cluster, then
`fat_put` 0 into it.

A file of *c* clusters costs *c* × (two reads and `nfats` writes). FAT16 SD cards
have clusters of 16–32 KB, so *c* is small: a 1 MB file is 32 to 64 clusters.
Coalescing entries that share a FAT sector would save those writes but costs
about 300 bytes of flash.

### 11.11 Delete

1. Look the name up. Not found → `UZEFS_ERR_NO_FILE`. Directory, volume label
   or read-only → `UZEFS_ERR_DENIED`. Open in any mode → `UZEFS_ERR_LOCKED`.
   Nothing lent → `UZEFS_ERR_NOBUF`.
2. Mark the long-name entries before it, then the entry, 0xE5.
3. Free the chain.

A chain that loops or leaves the volume stops the freeing with
`UZEFS_ERR_CORRUPT`. The entry is already gone by then; the clusters not yet
freed stay marked used, which `chkdsk` reports as lost clusters.

### 11.12 Rename

1. The target name must be valid (`UZEFS_ERR_BAD_NAME`) and must not exist,
   whatever it is (`UZEFS_ERR_EXISTS`). Renaming a file onto its own name is
   therefore `UZEFS_ERR_EXISTS`.
2. The source is checked as for delete: it exists, is not a directory, volume
   label or read-only, is not open, and a buffer is lent.
3. Set the long-name entries before it to 0xE5, and rewrite bytes 0–10 of the
   entry **in place**. All other bytes, the dates included, are unchanged.

In place, rather than FatFs's "new entry, then delete the old one", is simpler,
costs one read-modify-write, and leaves no moment with two entries for one chain.

### 11.13 Directory listing

- `uzefs_readdir` streams from the directory position to the next entry that is
  not free, deleted, long-name or volume label.
- It fills `uzefs_info` from bytes 0–11 and 22–31 of the entry, and leaves the
  position after it.
- Usually one `CMD17` per entry returned. Deleted and long-name entries in the
  same sector are skipped during the same read.
- A 0x00 first byte ends the listing.

### 11.14 Free space

`uzefs_getfree` streams every FAT sector and counts zero entries: 256 sectors on
the emulator's card, a few seconds.

### 11.15 Read buffer and kept sector

Every card read and write in the library goes through one layer, so the two
tiers only have to be right in one place:

- **Writing a sector** (`sect_put`): a sector in the first FAT is also written
  to every other copy. The read buffer forgets the sector if it holds bytes of
  it, and so does the kept sector unless the bytes written are the kept
  sector's own.
- **Loading a sector into the lent buffer** (`win_load`), for a change or at
  tier 2 for a read: at tier 2 with the buffer kept, nothing happens if it
  already holds that sector; otherwise the sector it holds is written first if
  it has changes waiting, then the new one is read (or, when the caller will
  fill all of it, not read).
- **Putting a changed sector back** (`win_put`): written at once, or at tier 2
  with the buffer kept, only marked as having changes waiting.
- **Reading** (`sd_read`), in this order:
  - tier 2 with the buffer kept: served from the kept sector, loaded first if
    needed. The exception is a whole sector read into the caller's buffer
    while the kept sector is a different one, which goes straight to the card.
    Scans are fed to their handler from the kept sector.
  - tier 1: a read shorter than the buffer is served from it; if it does not
    hold all those bytes, it is refilled from the first byte wanted, up to its
    size or the end of the sector. A scan or a longer read goes to the card.
  - otherwise straight from the card.
- The kept sector is written at sync, close, unmount, `uzefs_unlend` and
  `uzefs_lend`, and whenever another sector needs the buffer. Mount forgets
  both without writing (unmount has written them).

At tier 2 a direct whole-sector write can reach the card before a change
still waiting in the kept sector. The crash consistency section explains why
the order still holds.


### 11.16 Streaming

For a host that reads a file faster than a command per sector allows: the video
player in `demos/Uzeamp`, which reads about
1.2 MB/s while it draws.

- **`uzefs_fragments`** follows the chain once and counts the runs of
  consecutive clusters. A count of 1 marks the slot (`FF_CONTIG`), until a write
  to it clears the mark.
- **`uzefs_extent(fh, pos, *sect, *run)`** gives the card sector holding byte
  `pos` and how many sectors follow it without a break, up to the file's last.
  On a marked slot it is arithmetic (`clust_sect(sclust + n)`). Otherwise it
  follows the chain to `pos` and on to the end of that run, a FAT read per
  cluster.
- **`uzefs_stream_begin(sect)`** sends CMD18 and returns with the card selected
  and the read open; the host clocks the SPI itself. The card then sends, for
  each sector, 0xFF while it fetches it, the 0xFE token, 512 bytes and a CRC.
  The host:
  - sends 0xFF;
  - may stop the clock between any two bytes (SPI off, card still selected:
    measured on a Toshiba SA04G, stops of up to 250 ms mid-sector and between
    sectors, `demos/streamtest`);
  - must not deselect the card: three of the ten cards in the SD survey stop
    answering until they are reset;
  - must make no other UzeFS call and touch no SPI RAM until
    `uzefs_stream_end`.
- **`uzefs_stream_end()`** turns the SPI back on, sends CMD12 wherever the read
  is, waits out the busy and deselects the card.
- **`uzefs_stream_restart(sect, busy_max)`** moves the open read: CMD12, then
  CMD18 at `sect`, the card left selected as by `uzefs_stream_begin`. It takes a
  few dozen bytes plus the busy after CMD12, waited out for at most `busy_max`
  bytes, so a host with a deadline (the next video frame) can bound it. Past
  that it returns `UZEFS_ERR_DISK` with the card still selected and busy:
  `uzefs_stream_end` then waits longer. Uzeamp's still picture restarts the
  read at the paused picture in every vertical blank.
- **`uzefs_stream_gap(n, *min, *max)`** times the bytes between the sectors of a
  CMD18 from the data area's start.

**The FE/FF-free format.** A host that clocks bytes at a fixed rate cannot wait
for each token. When a file holds no 0xFE or 0xFF byte and each sector's CRC16 is
FEFE, FEFF, FFFE or FFFF, the host keeps every byte below 0xFE and drops the
rest. The card's CRC, its 0xFF wait (however long) and its token all fall out,
and what is kept is the file's data, sector after sector. The CRC is set by 3
bytes the encoder solves in each sector (`demos/Uzeamp/tools/uzm.py`: CRC16 is
linear, so this is a table lookup). On the SA04G every gap between sectors of a
16 MB read was 4 bytes, and no byte in them was other than 0xFF and the token.

## 12. Crash consistency and write order

### 12.1 Order

The rule the order serves: **a file's size in its directory entry is never
less than its cluster chain needs.** Windows `chkdsk` "repairs" a chain longer
than the size by extending the size over the whole chain, handing the file up
to a cluster of stale bytes, zeros or old data from deleted files. It cuts a
size that runs past the chain back to the chain, which loses nothing that was
in the file. So every step that changes either one moves the size first when
growing and the chain first when shrinking.

1. **A new cluster is marked end-of-chain, then its first sector of data is
   written, then the size goes into the directory entry, then the cluster is
   linked** from the previous cluster (or, for a file's first cluster, recorded
   in the entry in that same write). Until the link the cluster belongs to no
   file, and a crash leaves it as a lost cluster.
2. **A file's first cluster goes into its directory entry with its first
   sector of data**, not only at sync. FatFs waits for sync. Writing it
   straight away pays for itself twice (below).
3. **The size also goes into the entry at sync and close** (and at every write
   with `UZEFS_SIZE_EVERY_WRITE`). Between those, the size lags only inside the
   file's last cluster, which the chain already covers.
4. **Truncate cuts the chain, then writes the size, then frees what was cut.**
   Truncating to 0 and deleting change the entry **before** freeing clusters,
   so no entry ever names a freed cluster.

Tier 2 changes this in two ways, and neither breaks the rule:

- **Changes to one sector merge.** The kept sector is written before a
  different sector takes the buffer, so the sectors that order matters
  between (the FAT and the directory) still reach the card in order. Only
  consecutive changes to the same sector become one write, and a crash then
  shows the state before them or after them, both of which tier 0 also passes
  through.
- **Whole data sectors go straight to the card**, possibly before a change
  still waiting in the kept sector. Nothing in the order above puts data after
  metadata except a new cluster's first sector, which must be on the card
  before the size is; a direct write gets there earlier still. Data written
  before its cluster's end-of-chain mark or link lands in a cluster no file
  owns yet, which a crash leaves as free space or a lost cluster.

### 12.2 What a card looks like after a crash or card pull

| Pulled during | Result | Repair |
|---|---|---|
| a write, before sync | File has the size of its last sync or of the first sector of its last cluster, whichever is later; bytes written after that are past the size, inside the last cluster | None needed; those bytes are lost |
| a write adding a cluster, between the end-of-chain mark and the link | One cluster marked used but in no chain; after the size is written, the size also runs up to one sector past the chain | Lost cluster; the size is cut back to the chain |
| delete, after the entry is marked but before the chain is freed | Chain in no file | Lost clusters |
| truncate, between cutting the chain and freeing the rest | Clusters cut off are in no chain; before the size is written, the size runs past the chain | Lost clusters; the size is cut back to the chain |
| delete or rename of a long-named file whose long name starts in an earlier sector, between the two sector writes | The file keeps only its 8.3 name (rename: still the old one) | None needed |
| a directory or FAT sector write | That sector is whatever the card kept: SD cards write a block completely or not at all in normal conditions, but no guarantee exists for power loss | Normal repair |
| any time at tier 2, with changes waiting in the kept sector | Those changes are missing; whole data sectors written since are there. The card looks like one of the cases above | As for that case |

What UzeFS's order guarantees: no cluster is ever in two chains, no cluster is
in a chain and marked free at the same time, and no file's size falls short of
its chain, so a repair never hands a file bytes it did not write. Between calls
in tiers 0 and 1, the only thing not on the card is how far files have grown
inside their last cluster since their last sync. At tier 2 the kept sector's
waiting changes are missing too.

### 12.3 Why first-cluster-at-once matters

- **Real cards:** a crash before the first sync leaves the data reachable from a
  directory entry, rather than as clusters no entry points to.
- **CUzeBox `--sdhost`:** the emulator can only place a data sector on the PC
  once some directory entry's chain owns it. With FatFs, a new file's data waits
  in the emulator until the file is synced. More than 2 MB unsynced is lost (the
  emulator repository's `doc/spec-sd-enhanced-vfat.md`, limitation "Unflushed FAT
  windows are still lost"). With UzeFS, each cluster is placed as soon as it is linked, and its
  length comes from the bytes written until the size catches up (that spec's
  resize rule).

Rename in place is also what `--sdhost` recognises as a rename (that spec's
rename test covers a driver that renames in place).


## 13. Uzebox Basic integration

Basic's LOAD, SAVE and FILES use UzeFS. OPEN, CLOSE, PRINT#, INPUT#, LINE INPUT#,
EOF, LOF, LOC, SEEK, KILL and NAME are not written yet; the commands
table below gives the calls they will use.

### 13.1 Configuration

Set in `demos/Basic/default/Makefile`:

| Option | Value | Why |
|---|---|---|
| `UZEFS_MAX_FILES` | 1 | LOAD and SAVE use one file at a time. Raised when OPEN arrives |
| `UZEFS_DIR` | 1 | FILES |
| `UZEFS_SEEK_EXTEND` | 0 | Nothing seeks |
| `UZEFS_KEEP` | 1 | SAVE lends its buffer for its whole run anyway (the lending table), so the kept sector costs 5 bytes of RAM and 476 of flash, and saves `BENCHFUL.BAS` in 172 frames instead of 1,569 (CUzeBox's original card timing, 100 ms busy per block) |
| `UZEFS_RDBUF` | 0 | LOAD reads 64 bytes at a time; a buffer only helps reads shorter than itself |
| `UZEFS_FAT32` | 1 | Cards sold today come FAT32 formatted. +1,424 bytes of flash (+1,428 against the build before, which also took the `cmask` byte) and +15 of SRAM in every build (2026-09-26); `UZEFS_ROOT_GROW` and `UZEFS_FSINFO` are left at 1, and are the first 402 bytes to take back if flash gets tight |

### 13.2 Flash and RAM, measured 2026-09-14

The `rom` build, with `-mcall-prologues` like every Basic variant. The before
and after figures were measured with `-mrelax` as well; the last three rows of
the first block are the build as it is now.

| | Bytes |
|---|---|
| Whole ROM (`.text` + `.data`), Petit FatFs read only, before (with `-mrelax`) | 51,172 |
| Whole ROM with UzeFS, LOAD and SAVE (with `-mrelax`) | 55,146 |
| Whole ROM with UzeFS, LOAD and SAVE, `-mcall-prologues` only | **56,388** |
| Ceiling (bootloader above) | 61,440 |
| **Free** | **5,052** |
| UzeFS in it (`uzefs.o` 6,622 + `uzefs_sd.o` 836) | 7,458 |
| Petit FatFs it replaced (`pff.o` 2,470 + `diskio.o` 646) | 3,116 |
| `.data` + `.bss`, before → after | 3,298 → 3,311 of 4,096 |
| UzeFS RAM: volume 34, one slot 26, card type 1, handler 2 | 63 |

The `tests` build is 64,444 bytes, past the 61,440 ceiling and 1,092 bytes
short of the chip's end, so it cannot be
started from the Uzebox bootloader. Upstream CUzeBox starts a `.hex` whose code
reaches past 0xF000 in the boot section, where it hangs; the emulator fork
boots such a `.hex` at 0 (`doc/notes-gdbstub.md` in that repository, "Boot
choice for .hex files").

Room for the remaining commands, if flash runs short:

- Switch from video mode 80 to video mode 42. Mode 80's core, with its font, is
  21,796 bytes. Mode 42's core, assembled with Basic's kernel options, is 7,388
  bytes, plus a 2,048-byte 8×8 character set from mode 40: about 12,300 bytes
  less. But mode 42 has 40 columns, not 80, so Basic's terminal would have to
  change; this was measured by size only, not built into Basic.
- Drop `vfprintf_flt` (1,698 bytes).
- Link-time optimisation.

### 13.3 Lending the buffer

| Basic state | Buffer to lend | `keep` |
|---|---|---|
| SAVE, typed or inside a program | `sys_mem` (512 bytes; `compiler.src` + `compiler.emit`, which is also `vm_scratch.scratch_buf`), lent after the file name is copied onto the stack, given back with `uzefs_unlend()` before SAVE returns | 1 |
| LOAD | none: it only reads, and the tokenizer needs `sys_mem` for each line | — |
| Later: PRINT#, KILL, NAME in a program | `sys_mem`, while nothing in the VM keeps data in `scratch_buf` between statements (true today) | 0, or 1 with an unlend before the compiler or string code runs |

Data written must not lie inside the lent buffer. SAVE expands each program
line from SPI RAM into a 66-byte stack buffer and writes it in pieces of up to
63 characters, expanding the line again with the characters already written
skipped. A whole-line buffer (258 bytes) under `uzefs_write` left 11 bytes of
stack.

### 13.4 LOAD and SAVE

**Mount.** LOAD mounts once and keeps the mount until a disk error, a missing
card or "not mounted" drops it; a mount is the card init, about 34 frames.
SAVE mounts every time: writing with the geometry of a card swapped since the
last mount would put the FAT in the wrong place.

**LOAD** opens for reading and reads 64-byte chunks, splitting lines at CR or
LF and stopping at Ctrl-Z, as it did with Petit FatFs. The handle is closed on
every path.

**SAVE** opens with `UZEFS_O_WRITE | UZEFS_O_CREATE | UZEFS_O_TRUNC`, so an
existing file is replaced, and writes each line as LIST shows it, ending in
CRLF, with no Ctrl-Z. A name with no dot gets `.BAS`, as with LOAD. On an error
the file is closed and keeps what was written. LOAD of a saved file builds the
same program: the tests compare the stored program's CRC.

**Timing**, in emulator frames (60 a second), from the Basic file test suite.
The `--sdhost` card holds the 14 fixtures; the older emulator card is Basic's
`default/` folder, about 60 entries. Frame counts are ±1. This table was
measured with CUzeBox's original card timing: 500 ms to start up and 100 ms
busy after every written block, which is most of each SAVE and of the first
LOAD's mount.

| | Size (bytes) | Petit FatFs, `--sdhost` | UzeFS, `--sdhost` | Petit FatFs, older card | UzeFS, older card |
|---|---|---|---|---|---|
| LOAD `MANDEL.BAS`, first (includes the mount) | 437 | 39 | 38 | 44 | 39 |
| LOAD `MANDEL.BAS` | 437 | 5 | 4 | 10 | 5 |
| LOAD `TEST.BAS` | 1,557 | 28 | 27 | 34 | 28 |
| LOAD `BENCHRES.BAS` | 2,378 | 19 | 19 | 21 | 19 |
| LOAD `BENCHFUL.BAS` | 6,425 | 43 | 43 | 45 | 43 |
| LOAD of a missing file | — | 2 | 1 | 10 | 1 |
| SAVE `BENCHFUL` (218 lines) | 6,425 | — | 172 | — | 174 |
| SAVE `MANDEL` (24 lines) | 437 | — | 80 | — | 95 |
| SAVE of an empty program (mount + create) | 0 | — | 41 | — | 42 |

CUzeBox's defaults now match a Uzebox with a 256 MB card (16 ms to start up,
250 µs busy per block). With them, UzeFS under `--sdhost` takes 9 frames for
the first LOAD of `MANDEL.BAS`, 45 for `BENCHFUL.BAS`, 30 to SAVE `BENCHFUL`,
9 to SAVE `MANDEL` and 6 to SAVE an empty program. On the Uzebox itself, the
same test ROM takes 50 frames to LOAD `BENCHFUL.BAS`, 29 to SAVE it and 7 to
SAVE `MANDEL`.

A big LOAD is no faster: reading all of `BENCHFUL.BAS` in 64-byte chunks takes
22 frames with UzeFS, and both libraries clock a whole 514-byte block per
chunk; the other half is tokenizing and storing lines. Opening is faster,
because UzeFS scans the directory a sector at a time where Petit FatFs read a
sector per entry, which is why small files and the 60-entry card gain most.
A 128-byte chunk would halve the reads for about 64 more bytes of stack.

**Stack**, deepest point below the file test suite's frame, interrupts
included, with 652 bytes free there: LOAD 491 (Petit FatFs 481), SAVE 454, a
statement with no file access 237.

### 13.5 Commands

| BASIC | UzeFS | In Basic |
|---|---|---|
| `LOAD f$` | `uzefs_open(O_READ)`, `uzefs_read` in 64-byte chunks, `uzefs_close` | Yes |
| `SAVE f$` | `uzefs_open(O_WRITE \| O_CREATE \| O_TRUNC)`, `uzefs_write` per piece of a line, `uzefs_close` | Yes |
| `OPEN f$ FOR INPUT AS #n` | `uzefs_open(O_READ)` | Not yet |
| `OPEN f$ FOR OUTPUT AS #n` | `uzefs_open(O_WRITE \| O_CREATE \| O_TRUNC)` | Not yet |
| `OPEN f$ FOR APPEND AS #n` | `uzefs_open(O_WRITE \| O_CREATE \| O_APPEND)` | Not yet |
| `OPEN f$ FOR RANDOM AS #n` | `uzefs_open(O_READ \| O_WRITE \| O_CREATE)` | Not yet |
| `CLOSE #n` | `uzefs_close` | Not yet |
| `PRINT #n`, `INPUT #n`, `LINE INPUT #n` | `uzefs_write` / `uzefs_read` in chunks | Not yet |
| `EOF(n)`, `LOF(n)`, `LOC(n)`, `SEEK #n` | `uzefs_eof`, `uzefs_size`, `uzefs_tell`, `uzefs_seek` | Not yet |
| `KILL f$` | `uzefs_delete` | Not yet |
| `NAME a$ AS b$` | `uzefs_rename` | Not yet |
| `FILES [pattern]` | `uzefs_opendir` / `uzefs_readdir`, filtered in Basic | `FILES` done (2026-09-17); the pattern not yet |

`INPUT #` should read in chunks into a stack buffer rather than byte by byte:
without a kept sector every `uzefs_read` call costs a `CMD17`.

Basic calls `spi_set_ram_mode()` before UzeFS and never holds an SPI RAM
sequential transfer open across a call; the SD driver raises PA4 before every
command, which would end it.


## 14. Test plan

### 14.1 Where

- **Emulator:** a harness ROM in `demos/uzefstest/`, on the pattern of
  `demos/fattest/`, run under `cuzebox --sdhost` on a card from the emulator
  repository's `test/mkcard.sh`. The PC files are compared byte for byte.
  The emulator's card has no long names, about 2 GB free and a working card, so
  the harness makes those conditions itself with the SD driver's raw sector
  calls. It writes long-name entries in front of a file it then creates. It
  fills the free root directory entries with long-name entries, and marks all
  but two free clusters bad (0xFFF7). It writes loops and out-of-range links
  into a file's chain, and first clusters out of range into its entry. For a
  disk error mid-session it sends the card `CMD59` to turn CRC checking on, so
  the card refuses every UzeFS command, then turns it off again. Each of these
  is put back afterwards. A failed card at mount is a card folder replaced by a
  plain file, run separately. Before each raw sector access the harness lends
  its buffer again, so a kept sector reaches the card and UzeFS rereads what
  the harness changes; the `CMD59` switch skips that on purpose, so at tier 2
  a kept sector with changes waiting meets the failed card.
  `demos/uzefstest/run-tiers.sh` builds and runs every tier and compares the
  results and card folders with tier 0.
- **Basic:** the file suite in Basic's test ROM (`demos/Basic/file_tests.c`),
  run by `demos/Basic/run-filetests.sh` under `--sdhost` on a card made of
  Basic's `.BAS` fixtures (with `LEGACY=1`, on the older emulator card). It
  LOADs every fixture and checks the stored program's CRC against values
  recorded with Petit FatFs, SAVEs programs typed and inside a program and
  LOADs them back, and times every case in frames. The script then checks the
  saved files on the PC.
- **Real card:** the user's SDSC card, formatted FAT16 on a PC, run on the
  Uzebox. Afterwards the card is checked with `chkdsk /f` on Windows (and
  `fsck.fat -n` where available) and must report no errors.
- **FAT32, real card only** (the emulator's card is always FAT16): a probe ROM
  of Basic's (2026-09-26, not kept) ran at boot and mirrored the screen to the
  UART: FILES, a one-line SAVE/LOAD/RUN, a 201-line 10 KB file across three
  4 KB clusters SAVEd, LOADed and listed, 140 SAVEs, which grow a 128-entry
  root cluster, FILES of all 140, and 140 KILLs. On an SDHC card formatted
  FAT32 with 4 KB clusters, all passed, `FRE(0)` matching the emulator, and
  Windows `chkdsk` found no problems afterwards.
- **Sizes:** `avr-size` per object and the linker map, against the cost
  breakdown below.

### 14.2 Scenarios

1. Mount: emulator card with no partition table; real card with a partition
   table; FAT12 cards, and FAT32 without `UZEFS_FAT32`, refused with `UZEFS_ERR_NO_FS`; no card gives
   `UZEFS_ERR_NOT_READY`.
2. Create, write across several cluster boundaries, close: exact size and
   content.
3. Append to an existing file in a partly filled sector.
4. Seek into the middle and overwrite; seek past the end with and without
   `UZEFS_SEEK_EXTEND`.
5. Truncate to the middle of a cluster, to a cluster boundary, and to 0.
6. Rename; rename onto an existing name refused; rename a file a PC gave a long
   name, then check that no orphaned long-name entries remain (`chkdsk`).
7. Delete, including a long-named PC file; delete a read-only file refused;
   delete an open file refused.
8. Root directory full: `UZEFS_ERR_DIR_FULL` with nothing changed.
9. Card full: `UZEFS_ERR_FULL`, with the file still consistent up to what was
   written.
10. Four files open and interleaved: reads and writes alternating, two files
    growing into the same FAT sector, one file read by two handles; a fifth open
    returns `UZEFS_ERR_TOO_MANY`; a second open of a file being written returns
    `UZEFS_ERR_LOCKED`.
11. Write-type call without a lent buffer: `UZEFS_ERR_NOBUF`, card unchanged.
12. Disk errors from the emulator:
    - a card folder replaced by a plain file → `UZEFS_ERR_DISK` at mount
    - after that, the next call works without a remount (UzeFS holds nothing)
13. Corrupt chains: a self-loop and a link out of range in a hand-made FAT →
    `UZEFS_ERR_CORRUPT`, no hang.
14. Card pulled during a write (real hardware, several times): the card is
    repaired by `chkdsk` with at most the cases in the crash table, and never
    reports cross-linked files.
15. **Same results at every tier:** scenarios 2–13 at tier 0, tier 1 with a
    32-byte buffer, tier 2, and both; and the read-only build with and without
    the buffer. The PC files and every result must be identical.
16. Stack depth measured by filling the stack with a pattern, deepest scenario
    at each tier.
17. Read-only build (`UZEFS_WRITE=0`): size compared with Petit FatFs read only
    (3,552 bytes).
18. Basic LOAD and SAVE: every fixture LOADs to the same program as with Petit
    FatFs; LOAD errors 53, 64 and 66; SAVE then LOAD gives the same program;
    saving twice gives identical files; a save over a longer file leaves none
    of its bytes; an empty program saves as 0 bytes; SAVE inside a program
    carries on to the next line; a bad name gives 64.


## 15. Cost breakdown

Measured parts are the linked sizes of the functions in the `uzefstest` write
build, default flags. Anchors: Petit FatFs read only 3,552 bytes in Basic's
build; FatFs in Basic 11,330; FatFs R0.16 `ff.o` 13,776 and its fattest driver
2,888 in the fattest ROM.

| Part | Flash (bytes) |
|---|---|
| SD driver: init (SD v1, v2, SDHC), streaming read, sector write and zero | 926 |
| Mount and unmount: boot sector or partition table, FAT16 checks | 1,110 |
| Sector I/O: writing to every FAT copy, loading and putting back the lent buffer | 206 |
| FAT entries, free-cluster scan, allocation, freeing chains | 1,218 |
| Root directory: streamed lookup, free entry, long-name run, entry write, open-file check | 790 |
| Names: 8.3 conversion both ways, with its 17-byte table | 303 |
| Open, create, close, sync, slot table, lending | 802 |
| Read, write, seek and extension with zeros, one shared position routine | 1,696 |
| Truncate, delete, rename, long-name removal | 776 |
| Stat and directory listing | 410 |
| Small helpers | 34 |
| **Version 1, measured** | **8,271** |
| Version 1 with `-mrelax -mcall-prologues`, measured | 7,495 |
| Tier 1, 32-byte read buffer: the buffered read path (with the flags) | +302 (+274) |
| Tier 2, kept sector: loading, delayed writes, reads from it, writes at sync, unmount and lending (with the flags) | +518 (+476) |
| Read-only build, measured (with the flags) | 4,795 (4,281) |


## 16. Decisions and limitations

### 16.1 Decisions

| Question | Decision |
|---|---|
| Start from FatFs, Petit FatFs, or new code? | **New code** on Petit FatFs's streaming model and Uzebox driver, with FatFs's on-disk rules |
| Where the 512 bytes for writes come from | **Lent by the host**, per call by default, kept between calls only if the host chooses (tier 2) |
| FAT types | **FAT16 in version 1; FAT32 with `UZEFS_FAT32` since 2026-09-26**, one build mounting either |
| Card classes | **SDSC tested in version 1**; SDHC tested FAT16 and FAT32 formatted on a Uzebox |
| Open files | **Up to `UZEFS_MAX_FILES`, default 4**; one writer per file, any number of readers when there is no writer |
| Rename | **In place**, long-name entries removed |
| When a new file's first cluster reaches the directory | **With its first sector of data**, not at sync |
| Size against chain after a crash | **The size is never less than the chain needs**: the size is written before each new cluster is linked and after a truncate cuts the chain, because Windows `chkdsk` extends a short size over stale bytes |
| Handle representation | **A small index**, matching Basic's file numbers |
| Seek past the end | **Extends with zeros** by default (`UZEFS_SEEK_EXTEND`) |
| Driver file | **A copy in `kernel/uzefs/`**; `kernel/petitfatfs/diskio.c` is left alone for existing demos |

### 16.2 Limitations

- **Every write-type call needs 512 bytes free somewhere** for its duration. A
  host with no such page cannot write.
- **Slow at tier 0:**
  - a `CMD17`, 514 bytes clocked, for each sector a read touches, each FAT entry
    followed and each directory sector scanned;
  - a read and a write for each partial-sector write.
  Writing a text file a line at a time costs about one read and one write of the
  data sector per line. Tier 2 removes most of that, for 518 bytes of flash
  and the host keeping 512 bytes lent.
- **Tier 2 loses waiting changes on a disk error** (the same results at every
  tier section), and a card pulled with changes waiting loses them too.
- **8.3 names only.** Long names made on a PC are lost when UzeFS renames or
  deletes the file, and are otherwise untouched.
- **No subdirectories.** Files inside a PC-made subdirectory cannot be reached.
- **FAT16 root directory is fixed at its formatted size**, normally 512 entries,
  fewer if long names use several each. A FAT32 root holds a cluster's worth
  (cluster bytes / 32: 128 at 4 KB, 512 at 16 KB) per cluster in its chain and
  grows when full, unless `UZEFS_ROOT_GROW` is 0.
- **`chkdsk` may need to repair a card pulled mid-write** (the crash table). No
  journal; none is affordable.
- **Dates are fixed** unless the host provides `uzefs_time`, for example from
  the ESP8266.
- **FSInfo is not maintained on FAT32** beyond marking the free count unknown, so
  a PC has to count free space itself. Harmless, but it can take a moment on a
  large card. With `UZEFS_FSINFO=0` not even that: a PC shows the stale count
  until `chkdsk` corrects it.
- **The FAT32 free-cluster scan starts at cluster 2 after every mount** (FSInfo's
  hint is not read). A large, nearly full card makes the first allocation after a
  mount slow; Basic mounts before every SAVE.
