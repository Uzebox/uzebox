/*
 *  UzeFS, a small FAT file library for Uzebox Basic.
 *
 *  Keeps no sector between calls. Reads go straight from the card into the
 *  caller's buffer, and scans of the FAT and the directory stream a sector
 *  through a byte handler. See spec-uzefs.md.
 */

#include <string.h>
#include <avr/pgmspace.h>
#include "uzefs.h"
#include "uzefs_sd.h"

/* Volume flags */
#define VF_MOUNTED  0x01
#define VF_FAT32    0x02
#define VF_FSI      0x04    /* FSInfo's free count has been marked unknown since mount */

/* File slot flags. FF_RD and FF_WR are UZEFS_O_READ and UZEFS_O_WRITE shifted
** left by one, so open can copy them across in one step. */
#define FF_OPEN     0x01
#define FF_RD       0x02
#define FF_WR       0x04
#define FF_DIRTY    0x08
#define FF_FAILED   0x10
#define FF_CONTIG   0x20    /* uzefs_fragments found one piece; cleared by any write */
#define FF_NEWCL    0x40    /* a cluster is taken, waiting for its first data (link_new) */

/* Lent buffer states, tier 2 */
#define WS_KEPT     0x01    /* lent with keep: the buffer stays UzeFS's between calls */
#define WS_VALID    0x02    /* it holds sector win_sect */
#define WS_DIRTY    0x04    /* and that sector has changes not yet on the card */

#define FAT16_EOC   0xFFF8U

/* A cluster number. FAT16 has fewer than 65,527, so 16 bits hold every one
** there; FAT32 widens the type. The FAT type is the mounted volume's. */
#if UZEFS_FAT32
typedef uint32_t clust_t;
#define IS32        (vol.flags & VF_FAT32)
#define FAT32_EOC   0x0FFFFFF8UL
#else
typedef uint16_t clust_t;
#define IS32        0
#endif
/* End of chain as fat_put writes it: 0xFFFF, or 0x0FFFFFFF on FAT32. */
#define CL_EOC      ((clust_t)~(clust_t)0)
/* log2 of FAT entries per sector */
#define FAT_EPS     (IS32 ? 7 : 8)

typedef struct {
	uint8_t  flags;
	uint8_t  cshift;        /* log2 of sectors per cluster */
	uint8_t  cmask;         /* sectors per cluster - 1 */
	uint8_t  nfats;
	uint32_t fat_sect;
	clust_t  fat_size;      /* BPB_FATSz16 on FAT16: 16 bits */
	uint32_t dir_sect;      /* the root directory's first sector */
	uint32_t data_sect;     /* also where the FAT16 root directory ends */
	clust_t  max_clust;
#if UZEFS_WRITE
	clust_t  free_hint;
	uint8_t *win;
#if UZEFS_FAT32 && UZEFS_FSINFO
	uint32_t fsi_sect;      /* FAT32 FSInfo sector */
#endif
#endif
#if UZEFS_KEEP
	uint32_t win_sect;
	uint8_t  wstate;
#endif
#if UZEFS_RDBUF
	uint32_t rd_sect;
	uint16_t rd_ofs;
	uint8_t  rd_len;        /* bytes held from rd_ofs of rd_sect; 0 when empty */
	uint8_t  rd_buf[UZEFS_RDBUF];
#endif
} volume;

typedef struct {
	uint8_t  flags;
	uint8_t  dir_index;
	uint32_t dir_sect;
	clust_t  sclust;
	clust_t  cclust;        /* cluster holding byte fptr - 1; 0 when fptr is 0 */
	uint32_t csect;         /* sector holding byte fptr - 1; 0 when fptr is 0 */
	uint32_t fptr;
	uint32_t fsize;
} slot;

static volume vol;

/* The volume through a pointer the compiler cannot fold back into a constant
** address, held in Y or Z: each field is then an ldd/std of 2 bytes, not an
** lds/sts of 4. Worth it only where a function reaches many fields. */
static inline volume *vol_base(void)
{
	volume *v = &vol;

	__asm__ ("" : "+b" (v));
	return v;
}
#define VOL (*vol_base())
static slot   files[UZEFS_MAX_FILES];

/* What the byte handler in uzefs_sd_handler works on: a directory scan or a
** free-cluster scan, which never run at once. */
static void  *hctx;


/*--------------------------------------------------------------------------*/
/* Helpers                                                                  */
/*--------------------------------------------------------------------------*/

/* FAT is little-endian, as the AVR is: a field is a plain unaligned load. */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "uzefs: rd16/rd32/wr16/wr32 assume a little-endian target"
#endif
static uint16_t rd16(const uint8_t *p)
{
	uint16_t v;

	memcpy(&v, p, 2);
	return v;
}

static uint32_t rd32(const uint8_t *p)
{
	uint32_t v;

	memcpy(&v, p, 4);
	return v;
}

#if UZEFS_WRITE
static void wr16(uint8_t *p, uint16_t v)
{
	memcpy(p, &v, 2);
}

static void wr32(uint8_t *p, uint32_t v)
{
	memcpy(p, &v, 4);
}
#endif

static uint32_t __attribute__((noinline)) clust_sect(clust_t cl)
{
	return vol.data_sect + ((uint32_t)(clust_t)(cl - 2) << vol.cshift);
}


/*--------------------------------------------------------------------------*/
/* Sector I/O. Every card read and write of the library goes through here,  */
/* which is what keeps the read buffer and the kept sector current.         */
/*--------------------------------------------------------------------------*/

#if UZEFS_WRITE
/* Writes buf (512 zeros with buf NULL) as sector sect, and to every FAT copy
** when sect is in the first FAT (nothing before the FAT is ever written). The
** read buffer's copy of that sector is dropped, and so is the kept sector's
** unless buf is the kept sector. */
static uzefs_res sect_put(const uint8_t *buf, uint32_t sect)
{
	uzefs_res res;
	uint8_t   n = (sect < VOL.fat_sect + VOL.fat_size) ? VOL.nfats : 1;

#if UZEFS_FAT32 && UZEFS_FSINFO
	/* FAT32's FSInfo is the one sector before the FAT that is written. */
	if (sect < VOL.fat_sect) { n = 1; }
#endif

#if UZEFS_RDBUF
	if (sect == VOL.rd_sect) { VOL.rd_len = 0; }
#endif
#if UZEFS_KEEP
	if (buf != VOL.win && sect == VOL.win_sect) { VOL.wstate &= (uint8_t)~(WS_VALID | WS_DIRTY); }
#endif
	for (;;) {
		res = uzefs_sd_write(buf, sect);
		if (res || !--n) { return res; }
		sect += VOL.fat_size;
	}
}

#if UZEFS_KEEP
/* Writes the kept sector if it has changes waiting. If that fails the changes
** are dropped, so a failed card cannot fail every later call too. */
static uzefs_res win_flush(void)
{
	uzefs_res res = UZEFS_OK;

	if (VOL.wstate & WS_DIRTY) {
		VOL.wstate &= (uint8_t)~(WS_VALID | WS_DIRTY);
		res = sect_put(VOL.win, VOL.win_sect);
		if (!res) { VOL.wstate |= WS_VALID; }
	}
	return res;
}
#endif

/* Makes the lent buffer hold sector sect: read from the card (rd 1), or
** about to be filled whole by the caller (rd 0). Tier 2, with the buffer
** kept: nothing to do if it already holds sect; otherwise the sector it
** holds is written first if it has changes waiting. */
static uzefs_res win_load(uint32_t sect, uint8_t rd)
{
	uzefs_res res = UZEFS_OK;

#if UZEFS_KEEP
	if (VOL.wstate & WS_KEPT) {
		if ((VOL.wstate & WS_VALID) && VOL.win_sect == sect) { return UZEFS_OK; }
		res = win_flush();
		if (res) { return res; }
		VOL.wstate &= (uint8_t)~WS_VALID;
	}
#endif
	if (rd) { res = uzefs_sd_read(VOL.win, sect, 0, 512); }
#if UZEFS_KEEP
	if (!res && (VOL.wstate & WS_KEPT)) {
		VOL.win_sect = sect;
		VOL.wstate |= WS_VALID;
	}
#endif
	return res;
}

/* Sends the lent buffer, changed, back as sector sect: at once, or with the
** buffer kept (tier 2), when another sector needs the buffer or at sync,
** close, unmount or uzefs_unlend. */
static uzefs_res win_put(uint32_t sect)
{
#if UZEFS_KEEP
	if (vol.wstate & WS_KEPT) {
		vol.wstate |= WS_DIRTY;
		return UZEFS_OK;
	}
#endif
	return sect_put(vol.win, sect);
}
#endif /* UZEFS_WRITE */

/* Reads cnt bytes at ofs of sector sect into buf, or with buf NULL through
** uzefs_sd_handler. Tier 2, with the buffer kept: served from the kept
** sector, loaded first, except a whole sector read straight into the
** caller's buffer while the kept sector is another one. Tier 1: a read
** shorter than the read buffer is served from it, refilled from ofs when it
** does not hold those bytes. */
static uzefs_res sd_read(uint8_t *buf, uint32_t sect, uint16_t ofs, uint16_t cnt)
{
#if UZEFS_KEEP || UZEFS_RDBUF
	uzefs_res      res;
	const uint8_t *src;
#endif

#if UZEFS_KEEP
	if ((VOL.wstate & WS_KEPT) &&
	    !(buf && cnt == 512 && !((VOL.wstate & WS_VALID) && VOL.win_sect == sect))) {
		res = win_load(sect, 1);
		if (res) { return res; }
		src = VOL.win + ofs;
		goto serve;
	}
#endif
#if UZEFS_RDBUF
	if (buf && cnt < UZEFS_RDBUF) {
		if (sect != VOL.rd_sect || ofs < VOL.rd_ofs || ofs + cnt > VOL.rd_ofs + VOL.rd_len) {
			uint16_t n = 512 - ofs;

			if (n > UZEFS_RDBUF) { n = UZEFS_RDBUF; }
			VOL.rd_len = 0;
			res = uzefs_sd_read(VOL.rd_buf, sect, ofs, n);
			if (res) { return res; }
			VOL.rd_sect = sect;
			VOL.rd_ofs = ofs;
			VOL.rd_len = (uint8_t)n;
		}
		src = VOL.rd_buf + (ofs - VOL.rd_ofs);
		goto serve;
	}
#endif
	return uzefs_sd_read(buf, sect, ofs, cnt);

#if UZEFS_KEEP || UZEFS_RDBUF
serve:
	if (buf) {
		memcpy(buf, src, cnt);
	} else {
		while (cnt--) { uzefs_sd_handler(*src++); }
	}
	return UZEFS_OK;
#endif
}

/* The FAT sector holding cl's entry, and the entry's offset in it. */
static uint32_t __attribute__((noinline)) fat_loc(clust_t cl, uint16_t *ofs)
{
#if UZEFS_FAT32
	if (IS32) {
		*ofs = (uint16_t)((uint8_t)cl & 0x7F) << 2;
		return vol.fat_sect + (cl >> 7);
	}
#endif
	*ofs = (uint16_t)((uint8_t)cl) << 1;
	return vol.fat_sect + (cl >> 8);
}

/* Follows a chain one step. *nx becomes 0 at the end of the chain. */
static uzefs_res fat_next(clust_t cl, clust_t *nx)
{
	uzefs_res res;
	uint16_t  ofs;
	uint32_t  sect;
#if UZEFS_FAT32
	uint32_t  v = 0;
#else
	uint16_t  v;
#endif

	if (cl < 2 || cl >= VOL.max_clust) { return UZEFS_ERR_CORRUPT; }
	sect = fat_loc(cl, &ofs);
	res = sd_read((uint8_t *)&v, sect, ofs, IS32 ? 4 : 2);
	if (res) { return res; }
#if UZEFS_FAT32
	/* a FAT16 end-of-chain made a FAT32 one; the top 4 bits are reserved */
	if (!IS32 && v >= FAT16_EOC) { v = FAT32_EOC; }
	v &= 0x0FFFFFFFUL;
	if (v >= FAT32_EOC) {
#else
	if (v >= FAT16_EOC) {
#endif
		v = 0;
	} else if (v < 2 || v >= VOL.max_clust || v == cl) {
		/* 0 (free) or 1 inside a chain, past the volume, or pointing at itself */
		return UZEFS_ERR_CORRUPT;
	}
	*nx = v;
	return UZEFS_OK;
}

static slot *get_slot(uint8_t fh)
{
	if (fh >= UZEFS_MAX_FILES || !(files[fh].flags & FF_OPEN)) { return 0; }
	return &files[fh];
}


/*--------------------------------------------------------------------------*/
/* Names                                                                    */
/*--------------------------------------------------------------------------*/

/* "name.ext" to the 11 byte directory form. */
static uzefs_res make_name(const char *src, uint8_t *dst)
{
	uint8_t i = 0, lim = 8, c;

	memset(dst, ' ', 11);
	c = (uint8_t)*src;
	if (c == '.' || c == 0) { return UZEFS_ERR_BAD_NAME; }
	for (;;) {
		c = (uint8_t)*src++;
		if (c == 0) { break; }
		if (c == '.' && lim == 8) {
			i = 8; lim = 11;
			continue;
		}
		/* " * + , . / : ; < = > ? [ \ ] | as ranges */
		if (c <= ' ' || c >= 0x80 || i >= lim || c == '"' || c == '|' ||
		    (c >= '*' && c <= '/' && c != '-') || (c >= ':' && c <= '?') ||
		    (c >= '[' && c <= ']')) {
			return UZEFS_ERR_BAD_NAME;
		}
		if (c >= 'a' && c <= 'z') { c -= 0x20; }
		dst[i++] = c;
	}
	return UZEFS_OK;
}

/* A directory entry's first cluster; FAT32 keeps its high word at 20. */
static clust_t ent_clust(const uint8_t *e)
{
	clust_t cl = rd16(e + 26);

#if UZEFS_FAT32
	if (IS32) { cl |= (uint32_t)rd16(e + 20) << 16; }
#endif
	return cl;
}

static void info_fill(uzefs_info *fi, const uint8_t *e)
{
	char   *p = fi->name;
	uint8_t i;

	for (i = 0; i < 11; i++) {
		if (i == 8 && e[8] != ' ') { *p++ = '.'; }
		if (e[i] != ' ') { *p++ = (char)e[i]; }
	}
	*p = 0;
	if ((uint8_t)fi->name[0] == 0x05) { fi->name[0] = (char)0xE5; }
	fi->attr = e[11];
#if !UZEFS_INFO_NAME_ONLY
	fi->time = rd16(e + 22);
	fi->date = rd16(e + 24);
	fi->size = rd32(e + 28);
#endif
}


/*--------------------------------------------------------------------------*/
/* Root directory scan                                                      */
/*--------------------------------------------------------------------------*/

#define SC_LIST    0x01   /* stop at the next listable entry instead of a name */
#define SC_FOUND   0x02
#define SC_END     0x04   /* a 0x00 entry was seen */
#define SC_FREE    0x08   /* free_sect / free_idx are valid */
#define SC_LFN     0x10   /* lfn_sect / lfn_idx start a run before the current entry */

typedef struct {
	uint8_t        flags;
	uint8_t        pos;         /* byte within the current entry */
	uint8_t        idx;         /* entry within the sector being read */
	uint32_t       sect;        /* sector being read */
	const uint8_t *name;
	uint8_t        ent[32];     /* current entry; the one found once SC_FOUND */
	uint32_t       hit_sect;
	uint8_t        hit_idx;
#if UZEFS_WRITE
	uint32_t       free_sect;
	uint8_t        free_idx;
	uint32_t       lfn_sect;
	uint8_t        lfn_idx;
#endif
} dscan;

static void dir_entry_end(dscan *c)
{
	const uint8_t *e = c->ent;

	if (e[0] == 0x00 || e[0] == 0xE5) {
#if UZEFS_WRITE
		if (!(c->flags & SC_FREE)) {
			c->flags |= SC_FREE;
			c->free_sect = c->sect;
			c->free_idx = c->idx;
		}
		c->flags &= (uint8_t)~SC_LFN;
#endif
		if (e[0] == 0x00) { c->flags |= SC_END; }
		return;
	}
	if (e[11] == 0x0F) {
#if UZEFS_WRITE
		if (!(c->flags & SC_LFN)) {
			c->flags |= SC_LFN;
			c->lfn_sect = c->sect;
			c->lfn_idx = c->idx;
		}
#endif
		return;
	}
	if (!(e[11] & UZEFS_AM_VOL) &&
	    ((c->flags & SC_LIST) || memcmp(e, c->name, 11) == 0)) {
		c->flags |= SC_FOUND;
		c->hit_sect = c->sect;
		c->hit_idx = c->idx;
		return;
	}
#if UZEFS_WRITE
	c->flags &= (uint8_t)~SC_LFN;
#endif
}

static void dir_byte(uint8_t b)
{
	dscan *c = hctx;

	if (c->flags & (SC_FOUND | SC_END)) { return; }
	c->ent[c->pos] = b;
	if (++c->pos == 32) {
		c->pos = 0;
		dir_entry_end(c);
		c->idx++;
	}
}

#if UZEFS_FAT32
/* The cluster holding data sector sect: clust_sect's inverse. */
static clust_t __attribute__((noinline)) sect_clust(uint32_t sect)
{
	return (clust_t)((sect - vol.data_sect) >> vol.cshift) + 2;
}
#endif

/* The root directory sector after *sect, or 0 past its end: the FAT16 root
** is a fixed area, the FAT32 one a cluster chain. */
static uzefs_res dir_next(uint32_t *sect)
{
	uint32_t  s = *sect + 1;
#if UZEFS_FAT32
	uzefs_res res;
	clust_t   cl;

	if (IS32) {
		if ((uint8_t)(s - vol.data_sect) & vol.cmask) { goto out; }
		res = fat_next(sect_clust(*sect), &cl);
		if (res) { return res; }
		s = cl ? clust_sect(cl) : 0;
		goto out;
	}
#endif
	if (s == vol.data_sect) { s = 0; }
#if UZEFS_FAT32
out:
#endif
	*sect = s;
	return UZEFS_OK;
}

/* Scans the root directory from sect/idx until an entry is found or the
** directory ends. An idx of 16 starts at the next sector; a sect of 0 is the
** end. */
static uzefs_res dir_scan(dscan *c, uint32_t sect, uint8_t idx)
{
	uzefs_res res;

	hctx = c;
	uzefs_sd_handler = dir_byte;
	while (sect) {
		if (idx < 16) {
			c->sect = sect;
			c->idx = idx;
			c->pos = 0;
			res = sd_read(0, sect, (uint16_t)idx << 5, (uint16_t)(512 - ((uint16_t)idx << 5)));
			if (res) { return res; }
			if (c->flags & (SC_FOUND | SC_END)) { break; }
		}
		res = dir_next(&sect);
		if (res) { return res; }
		idx = 0;
	}
	return UZEFS_OK;
}

/* Converts a name into nm and looks it up. The result is in c->flags &
** SC_FOUND. */
static uzefs_res lookup(const char *name, uint8_t *nm, dscan *c)
{
	uzefs_res res;

	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
	res = make_name(name, nm);
	if (res) { return res; }
	memset(c, 0, sizeof(*c));
	c->name = nm;
	return dir_scan(c, vol.dir_sect, 0);
}

#if UZEFS_WRITE
/* Whether a file slot whose flags match mask holds the entry found. */
static uint8_t busy(const dscan *c, uint8_t mask)
{
	uint8_t i;

	for (i = 0; i < UZEFS_MAX_FILES; i++) {
		if ((files[i].flags & mask) && files[i].dir_sect == c->hit_sect &&
		    files[i].dir_index == c->hit_idx) { return 1; }
	}
	return 0;
}
#endif


#if UZEFS_WRITE
/*--------------------------------------------------------------------------*/
/* Writes. Every caller has checked that a buffer is lent.                  */
/*--------------------------------------------------------------------------*/

/* Writes the slot's size, first cluster and modification date to its
** directory entry, and sets the archive attribute. With nm given the entry is
** created first: cleared, named, and given the same creation date. */
static uzefs_res dir_write(slot *f, const uint8_t *nm)
{
	uzefs_res res;
	uint8_t  *e;
	uint32_t  dt;

	res = win_load(f->dir_sect, 1);
	if (res) { return res; }
	e = vol.win + ((uint16_t)f->dir_index << 5);
	dt = uzefs_time();
	if (nm) {
		memset(e, 0, 32);
		memcpy(e, nm, 11);
		wr32(e + 14, dt);
	}
	e[11] |= UZEFS_AM_ARC;
	wr32(e + 22, dt);
	wr16(e + 26, (uint16_t)f->sclust);
#if UZEFS_FAT32
	if (IS32) { wr16(e + 20, (uint16_t)(f->sclust >> 16)); }
#endif
	wr32(e + 28, f->fsize);
	return win_put(f->dir_sect);
}

/* Writes one FAT entry to every FAT copy (sect_put writes the copies). The
** first on a FAT32 volume since mount marks FSInfo's free count unknown, so a
** PC counts it again rather than trusting a stale one. */
static uzefs_res fat_put(clust_t cl, clust_t val)
{
	uint32_t  sect;
	uint16_t  ofs;
	uint8_t  *p;
	uzefs_res res;

#if UZEFS_FAT32 && UZEFS_FSINFO
	if ((vol.flags & (VF_FAT32 | VF_FSI)) == VF_FAT32) {
		vol.flags |= VF_FSI;
		res = win_load(vol.fsi_sect, 1);
		if (res) { return res; }
		if (rd32(vol.win + 484) == 0x61417272UL) {
			memset(vol.win + 488, 0xFF, 4);
			res = win_put(vol.fsi_sect);
			if (res) { return res; }
		}
	}
#endif
	sect = fat_loc(cl, &ofs);
	res = win_load(sect, 1);
	if (res) { return res; }
	p = vol.win + ofs;
	wr16(p, (uint16_t)val);
#if UZEFS_FAT32
	if (IS32) {
		/* the top 4 bits are reserved: kept as they are */
		p[2] = (uint8_t)(val >> 16);
		p[3] = (uint8_t)((p[3] & 0xF0) | ((uint8_t)(val >> 24) & 0x0F));
	}
#endif
	return win_put(sect);
}

/* Frees a chain, one FAT entry at a time. FAT16 clusters on an SD card are
** 16 to 32 KB, so chains are short and batching entries would not pay. A loop
** in the chain reaches an entry already freed, which fat_next refuses. */
static uzefs_res free_chain(clust_t cl)
{
	uzefs_res res;
	clust_t   nx;

	while (cl) {
		res = fat_next(cl, &nx);
		if (!res) { res = fat_put(cl, 0); }
		if (res) { return res; }
		if (cl < vol.free_hint) { vol.free_hint = cl; }
		cl = nx;
	}
	return UZEFS_OK;
}

/* The end of a truncate: the entry gets the size (and first cluster 0 when
** cut to 0) and no longer needs updating, then the chain from rest is freed. */
static uzefs_res __attribute__((noinline)) cut_chain(slot *f, clust_t rest)
{
	uzefs_res res = dir_write(f, 0);

	f->flags &= (uint8_t)~FF_DIRTY;
	if (!res) { res = free_chain(rest); }
	return res;
}

/* Cuts the file to 0 bytes. */
static uzefs_res __attribute__((noinline)) cut_all(slot *f)
{
	clust_t  rest = f->sclust;

	f->sclust = 0;
	f->fsize = 0;
	return cut_chain(f, rest);
}

/* First free entry, streamed a FAT sector at a time from free_hint, then once
** more from cluster 2. alloc_cluster marks it end-of-chain. */
typedef struct {
	clust_t  cl;       /* cluster the next entry's bytes belong to */
	clust_t  from;     /* lowest cluster that counts */
	clust_t  found;
	uint8_t  n;        /* byte within the entry */
	uint8_t  last;     /* the entry's last byte: 1, or 3 on FAT32 */
	uint8_t  acc;
} fscan;

static void fat_scan_byte(uint8_t b)
{
	fscan *s = hctx;

#if UZEFS_FAT32
	if (s->n == 3) { b &= 0x0F; }   /* FAT32's top 4 bits are reserved */
#endif
	s->acc |= b;
	if (s->n++ != s->last) { return; }
	s->n = 0;
	if (!s->acc && !s->found && s->cl >= s->from && s->cl < vol.max_clust) { s->found = s->cl; }
	s->acc = 0;
	s->cl++;
}

static uzefs_res alloc_cluster(clust_t *out)
{
	fscan     s;
	uzefs_res res;
	uint8_t   sh = FAT_EPS;
	clust_t   sect, last = (clust_t)(vol.max_clust - 1) >> sh;
	uint8_t   pass;

	memset(&s, 0, sizeof(s));
	s.last = IS32 ? 3 : 1;
	s.from = vol.free_hint;
	hctx = &s;
	uzefs_sd_handler = fat_scan_byte;
	for (pass = 0; pass < 2; pass++) {
		sect = s.from >> sh;
		s.cl = sect << sh;
		/* Mount checked that the FAT holds max_clust entries, so the last
		** sector with any is inside it. */
		for (; sect <= last; sect++) {
			res = sd_read(0, vol.fat_sect + sect, 0, 512);
			if (res) { goto done; }
			if (s.found) {
				*out = s.found;
				vol.free_hint = s.found + 1;
				res = fat_put(s.found, CL_EOC);
				goto done;
			}
		}
		s.from = 2;
	}
	res = UZEFS_ERR_FULL;
done:
	/* s is about to go: leave no pointer to it behind */
	hctx = 0;
	return res;
}

/*
** Joins a new cluster to the file once its first sector of data is written
** (xfer): the size goes into the entry first, then the cluster is linked
** after cclust, or for a file's first cluster recorded in the entry with it.
**
** The entry's size must never be less than its chain needs. Windows chkdsk
** "repairs" a chain longer than the size by extending the size over it, stale
** bytes and all, but cuts a size that runs past the chain back to the chain.
** So a crash before the link leaves the size past the chain and the new
** cluster lost, never the other way round.
**
** The new cluster is the one alloc_cluster just took: the free hint is left
** one past it, and nothing allocates in between.
*/
static uzefs_res link_new(slot *f)
{
	clust_t   cl = vol.free_hint - 1;
	uzefs_res res;

	f->flags &= (uint8_t)~FF_NEWCL;
	if (!f->cclust) { f->sclust = cl; }
	res = dir_write(f, 0);
	if (!res && f->cclust) { res = fat_put(f->cclust, cl); }
	f->cclust = cl;
	return res;
}

#if UZEFS_FAT32 && UZEFS_ROOT_GROW
/* A full FAT32 root directory, scanned to its last sector (c->sect), gets a
** new cluster: marked end-of-chain, zeroed, then joined to the chain, so a
** crash leaves at worst a lost cluster. */
static uzefs_res dir_grow(dscan *c)
{
	clust_t   cl, last = sect_clust(c->sect);
	uint32_t  sect;
	uint8_t   n = 0;
	uzefs_res res = alloc_cluster(&cl);

	if (res) { return res; }
	sect = clust_sect(cl);
	do {
		res = sect_put(0, sect + n);
	} while (!res && n++ != vol.cmask);
	if (!res) { res = fat_put(last, cl); }
	c->free_sect = sect;
	c->free_idx = 0;
	return res;
}
#endif

/* Marks the long-name entries just before the entry found 0xE5, then marks
** the entry itself, or with nm given renames it in place instead. The
** long-name run may start in an earlier sector: one read-modify-write per
** sector touched. */
static uzefs_res dir_remove(const dscan *c, const uint8_t *nm)
{
	uzefs_res res;
	uint32_t  sect = c->hit_sect;
	uint8_t   idx = c->hit_idx;
	uint8_t  *e;

	if (c->flags & SC_LFN) {
		sect = c->lfn_sect;
		idx = c->lfn_idx;
	}
	for (;;) {
		res = win_load(sect, 1);
		if (res) { return res; }
		for (; idx < 16; idx++) {
			e = vol.win + ((uint16_t)idx << 5);
			if (sect == c->hit_sect && idx == c->hit_idx) {
				if (nm) {
					memcpy(e, nm, 11);
				} else {
					e[0] = 0xE5;
				}
				return win_put(sect);
			}
			e[0] = 0xE5;
		}
		res = win_put(sect);
		if (!res) { res = dir_next(&sect); }
		if (res) { return res; }
		idx = 0;
	}
}
#endif /* UZEFS_WRITE */


/*--------------------------------------------------------------------------*/
/* Mount                                                                    */
/*--------------------------------------------------------------------------*/

#if UZEFS_BOOTLOADER
/* The volume the bootloader found (uzefs_bl.c), and what it does not keep read
** from the boot sector it leaves in the lent buffer: the FAT's size, the
** volume's, the reserved sectors and the FSInfo sector. A FAT12 volume, or one
** whose FAT is too small for its clusters, is refused as below. */
uzefs_res uzefs_mount(void)
{
	sdc_struct_t d;
	uint8_t  *b = VOL.win;
	uzefs_res res;
	uint32_t  fatsz, tot, ncl;
	uint16_t  base;         /* the partition's first sector: the bootloader keeps 16 bits */
	uint8_t   s;

	VOL.flags = 0;
	memset(files, 0, sizeof(files));
#if UZEFS_KEEP
	VOL.wstate &= WS_KEPT;
#endif
#if UZEFS_RDBUF
	VOL.rd_len = 0;
#endif
	if (!b) { return UZEFS_ERR_NOBUF; }
	d.bufp = b;
	res = uzefs_sd_mount(&d);
	if (res) { return res; }

	for (s = 0; (1U << s) != d.csize; s++) {}
	VOL.cshift = s;
	VOL.cmask = (uint8_t)(d.csize - 1);
	VOL.nfats = 2;
	fatsz = rd16(b + 22);
#if UZEFS_FAT32
	if (fatsz == 0) { fatsz = rd32(b + 36); }
#endif
	tot = rd16(b + 19);
	if (tot == 0) { tot = rd32(b + 32); }
	base = d.fatp - rd16(b + 14);
	ncl = ((tot - (d.datap - base)) >> s) + 2;
	/* a FAT entry is 4 bytes on FAT32, 2 on FAT16: 128 or 256 to its sector */
	if (ncl < 4085 + 2 || (fatsz << ((d.flags & SDC_FLAGS_FAT32) ? 7 : 8)) < ncl) { return UZEFS_ERR_NO_FS; }
	VOL.max_clust = (clust_t)ncl;
	VOL.fat_size = fatsz;
	VOL.fat_sect = d.fatp;
	VOL.dir_sect = d.rootp;
	VOL.data_sect = d.datap;
#if UZEFS_WRITE
	VOL.free_hint = 2;
#endif
	s = VF_MOUNTED;
	if (d.flags & SDC_FLAGS_FAT32) {
#if UZEFS_FAT32
		if (d.rootp < 2 || d.rootp >= ncl) { return UZEFS_ERR_NO_FS; }
		VOL.dir_sect = clust_sect((clust_t)d.rootp);
#if UZEFS_WRITE && UZEFS_FSINFO
		VOL.fsi_sect = (uint32_t)base + rd16(b + 48);
#endif
		s = VF_MOUNTED | VF_FAT32;
#else
		return UZEFS_ERR_NO_FS;
#endif
	} else if (ncl >= 65525 + 2) { return UZEFS_ERR_NO_FS; }
	VOL.flags = s;
	return UZEFS_OK;
}

#else /* !UZEFS_BOOTLOADER */

/* The fields of the boot sector mount reads: bytes 0..35, and on FAT32 up to
** 49 for the 32-bit FAT size, the root cluster and the FSInfo sector. */
#if UZEFS_FAT32
#define BOOT_LEN 50
#else
#define BOOT_LEN 36
#endif

/* Reads the fields of a boot sector into b. 0: it is one. 1: the sector ends
** in the boot signature but is not a boot sector (a partition table). 2: no
** signature, or a read failed (*res). */
static uint8_t boot_check(uint32_t sect, uint8_t *b, uzefs_res *res)
{
	*res = uzefs_sd_read(b, sect, 510, 2);
	if (*res || b[0] != 0x55 || b[1] != 0xAA) { return 2; }
	*res = uzefs_sd_read(b, sect, 0, BOOT_LEN);
	if (*res) { return 2; }
	if (b[0] != 0xEB && b[0] != 0xE9 && b[0] != 0xE8) { return 1; }
	if (rd16(b + 11) != 512) { return 1; }
	if (b[13] == 0 || (b[13] & (b[13] - 1))) { return 1; }
	if (b[16] != 1 && b[16] != 2) { return 1; }
	return 0;
}

uzefs_res uzefs_mount(void)
{
	uint8_t   b[BOOT_LEN];
	uzefs_res res;
	uint32_t  base = 0, fatsz, tot, sysect, ncl;
	uint16_t  rootsecs;
	uint8_t   i, s;

	VOL.flags = 0;
	memset(files, 0, sizeof(files));
	/* Whatever was held belongs to the card mounted before; uzefs_unmount
	** has written it. */
#if UZEFS_KEEP
	VOL.wstate &= WS_KEPT;
#endif
#if UZEFS_RDBUF
	VOL.rd_len = 0;
#endif

	res = uzefs_sd_init();
	if (res) { return res; }

	s = boot_check(0, b, &res);
	if (s == 1) {
		/* A partition table: the first FAT16 (or FAT32) entry's first sector. */
		for (i = 0; ; i++) {
			if (i == 4) { return UZEFS_ERR_NO_FS; }
			res = uzefs_sd_read(b, 0, (uint16_t)(0x1C2 + (i << 4)), 8);
			if (res) { return res; }
			if (b[0] == 0x04 || b[0] == 0x06 || b[0] == 0x0E) { break; }
#if UZEFS_FAT32
			if (b[0] == 0x0B || b[0] == 0x0C) { break; }
#endif
		}
		base = rd32(b + 4);
		s = boot_check(base, b, &res);
	}
	if (s) { return res ? res : UZEFS_ERR_NO_FS; }

	for (s = 0; (1U << s) != b[13]; s++) {}
	VOL.cshift = s;
	VOL.cmask = (uint8_t)(b[13] - 1);
	VOL.nfats = b[16];
	fatsz = rd16(b + 22);
#if UZEFS_FAT32
	if (fatsz == 0) { fatsz = rd32(b + 36); }
#endif
	tot = rd16(b + 19);
	if (tot == 0) { tot = rd32(b + 32); }
	rootsecs = rd16(b + 17) >> 4;

	sysect = rd16(b + 14) + fatsz * VOL.nfats + rootsecs;
	if (fatsz == 0 || tot <= sysect) { return UZEFS_ERR_NO_FS; }
	ncl = ((tot - sysect) >> s) + 2;
	if (ncl < 4085 + 2) { return UZEFS_ERR_NO_FS; }
	i = VF_MOUNTED;
#if UZEFS_FAT32
	if (ncl >= 65525 + 2) {
		/* FAT32: no fixed root area, entries of 4 bytes, 28-bit numbers */
		if (rootsecs || (fatsz << 7) < ncl) { return UZEFS_ERR_NO_FS; }
		i = VF_MOUNTED | VF_FAT32;
	} else
#endif
	if (ncl >= 65525 + 2 || rootsecs == 0 || (fatsz << 8) < ncl) { return UZEFS_ERR_NO_FS; }
	VOL.max_clust = (clust_t)ncl;

	VOL.fat_size = fatsz;
	VOL.fat_sect = base + rd16(b + 14);
	VOL.dir_sect = VOL.fat_sect + fatsz * VOL.nfats;
	VOL.data_sect = VOL.dir_sect + rootsecs;
#if UZEFS_WRITE
	VOL.free_hint = 2;
#endif
#if UZEFS_FAT32
	if (i & VF_FAT32) {
		ncl = rd32(b + 44);                     /* BPB_RootClus */
		if (ncl < 2 || ncl >= VOL.max_clust) { return UZEFS_ERR_NO_FS; }
		VOL.dir_sect = clust_sect((clust_t)ncl);
#if UZEFS_WRITE && UZEFS_FSINFO
		/* BPB_FSInfo; fat_put checks the sector's signature before use */
		VOL.fsi_sect = base + rd16(b + 48);
#endif
	}
#endif
	VOL.flags = i;
	return UZEFS_OK;
}
#endif /* UZEFS_BOOTLOADER */

uzefs_res uzefs_unmount(void)
{
	uzefs_res res = UZEFS_OK, r;
	uint8_t   i;

	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
#if UZEFS_WRITE
	/* A write-type call changes nothing if it cannot go through, so check
	** every dirty writer has somewhere to be synced before closing any. */
	if (!vol.win) {
		for (i = 0; i < UZEFS_MAX_FILES; i++) {
			if ((files[i].flags & (FF_OPEN | FF_DIRTY)) == (FF_OPEN | FF_DIRTY)) {
				return UZEFS_ERR_NOBUF;
			}
		}
	}
#endif
	for (i = 0; i < UZEFS_MAX_FILES; i++) {
		if (files[i].flags & FF_OPEN) {
			r = uzefs_close(i);
			if (r && !res) { res = r; }
		}
	}
#if UZEFS_KEEP
	r = win_flush();
	if (r && !res) { res = r; }
#endif
	vol.flags = 0;
	return res;
}

#if UZEFS_WRITE
/* Lending again first does what uzefs_unlend does. */
uzefs_res uzefs_lend(uint8_t *buf, uint8_t keep)
{
	uzefs_res res = uzefs_unlend();

	VOL.win = buf;
#if UZEFS_KEEP
	if (keep && buf) { VOL.wstate = WS_KEPT; }
#else
	(void)keep;
#endif
	return res;
}

/* Writes a kept sector with changes waiting, and forgets what the kept
** sector and the read buffer hold, so a host that writes to the card by
** other means calls this first. */
uzefs_res uzefs_unlend(void)
{
	uzefs_res res = UZEFS_OK;

#if UZEFS_KEEP
	res = win_flush();
	vol.wstate = 0;
#endif
#if UZEFS_RDBUF
	vol.rd_len = 0;
#endif
	vol.win = 0;
	return res;
}
#endif


/*--------------------------------------------------------------------------*/
/* Files                                                                    */
/*--------------------------------------------------------------------------*/

/* At a sector boundary, makes csect the sector holding byte fptr: the next
** sector of cclust, or the first sector of the next cluster in the chain.
** When the chain ends there and "grow" is set, a cluster is allocated and
** marked end-of-chain, but not yet joined to the file: cclust stays the
** previous cluster until link_new. */
static uzefs_res locate(slot *f, uint8_t grow)
{
	uzefs_res res;
	clust_t   cl = f->sclust;

	if (f->cclust) {
		if ((uint8_t)(f->csect + 1 - vol.data_sect) & vol.cmask) {
			f->csect++;
			return UZEFS_OK;
		}
		res = fat_next(f->cclust, &cl);
		if (res) { return res; }
	}
	if (!cl) {
#if UZEFS_WRITE
		if (!grow) { return UZEFS_ERR_CORRUPT; }
		res = alloc_cluster(&cl);
		if (res) { return res; }
		f->flags |= FF_NEWCL;
#else
		(void)grow;
		return UZEFS_ERR_CORRUPT;
#endif
	} else {
		f->cclust = cl;
	}
	f->csect = clust_sect(cl);
	return UZEFS_OK;
}

#if UZEFS_WRITE
/* Puts n bytes (zeros with buf NULL) at offset ofs of sector csect. A whole
** sector is sent as is; a part is merged into the sector through the lent
** buffer, skipping the read when nothing in the sector is worth keeping. */
static uzefs_res put_data(slot *f, const uint8_t *buf, uint16_t ofs, uint16_t n)
{
	uzefs_res res;
	uint8_t   rd = !(ofs == 0 && f->fptr + n >= f->fsize);

	if (n == 512) { return sect_put(buf, f->csect); }
	res = win_load(f->csect, rd);
	if (res) { return res; }
	if (!rd) { memset(vol.win, 0, 512); }
	if (buf) {
		memcpy(vol.win + ofs, buf, n);
	} else {
		memset(vol.win + ofs, 0, n);
	}
	return win_put(f->csect);
}
#endif

/* Read (wr 0) or write (wr 1; buf NULL writes zeros), shared by uzefs_read,
** uzefs_write and seeking past the end. */
static uzefs_res xfer(uint8_t fh, uint8_t *buf, uint16_t n, uint16_t *done, uint8_t wr)
{
	slot     *f = get_slot(fh);
	uzefs_res res;
	uint16_t  ofs, chunk;

	*done = 0;
	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
	if (f->flags & FF_FAILED) { return UZEFS_ERR_DISK; }
	if (!(f->flags & (wr ? FF_WR : FF_RD))) { return UZEFS_ERR_MODE; }
#if UZEFS_WRITE
	if (wr && !vol.win) { return UZEFS_ERR_NOBUF; }
	if (wr) { f->flags &= ~FF_CONTIG; }
#endif
	if (!wr && n > f->fsize - f->fptr) { n = (uint16_t)(f->fsize - f->fptr); }

	while (n) {
		ofs = (uint16_t)f->fptr & 511;
		if (!ofs) {
			res = locate(f, wr);
			if (res) { goto fail; }
		}
		chunk = 512 - ofs;
		if (chunk > n) { chunk = n; }
#if UZEFS_WRITE
		if (wr) {
			res = put_data(f, buf, ofs, chunk);
		} else
#endif
		{
			res = sd_read(buf, f->csect, ofs, chunk);
		}
		if (res) { goto fail; }
		f->fptr += chunk;
		if (buf) { buf += chunk; }
		*done += chunk;
		n -= chunk;
#if UZEFS_WRITE
		if (wr) {
			f->flags |= FF_DIRTY;
			if (f->fptr > f->fsize) { f->fsize = f->fptr; }
			if (f->flags & FF_NEWCL) {
				res = link_new(f);
				if (res) { goto fail; }
			}
		}
#endif
	}
	return UZEFS_OK;

fail:
	if (res == UZEFS_ERR_DISK) { f->flags |= FF_FAILED; }
	return res;
}

uzefs_res uzefs_open(uint8_t *fh, const char *name, uint8_t mode)
{
	uint8_t   nm[11];
	dscan     c;
	uzefs_res res;
	slot     *f;
	uint8_t   i;

#if UZEFS_WRITE
	if (!(mode & (UZEFS_O_READ | UZEFS_O_WRITE)) ||
	    ((mode & (UZEFS_O_CREATE | UZEFS_O_TRUNC | UZEFS_O_APPEND | UZEFS_O_EXCL)) &&
	     !(mode & UZEFS_O_WRITE))) { return UZEFS_ERR_MODE; }
	if ((mode & UZEFS_O_WRITE) && !vol.win) { return UZEFS_ERR_NOBUF; }
#if !UZEFS_APPEND
	if (mode & UZEFS_O_APPEND) { return UZEFS_ERR_MODE; }
#endif
#else
	if (mode != UZEFS_O_READ) { return UZEFS_ERR_MODE; }
#endif

	res = lookup(name, nm, &c);
	if (res) { return res; }

	if (c.flags & SC_FOUND) {
		if (c.ent[11] & (UZEFS_AM_DIR | UZEFS_AM_VOL)) { return UZEFS_ERR_DENIED; }
#if UZEFS_WRITE
		if ((mode & UZEFS_O_WRITE) && (c.ent[11] & UZEFS_AM_RDO)) { return UZEFS_ERR_DENIED; }
		if (mode & UZEFS_O_EXCL) { return UZEFS_ERR_EXISTS; }
		/* A writer excludes every other handle; a reader excludes writers. */
		if (busy(&c, (mode & UZEFS_O_WRITE) ? FF_OPEN : FF_WR)) { return UZEFS_ERR_LOCKED; }
#endif
	} else if (!(mode & UZEFS_O_CREATE)) {
		return UZEFS_ERR_NO_FILE;
	}

	for (i = 0; ; i++) {
		if (i == UZEFS_MAX_FILES) { return UZEFS_ERR_TOO_MANY; }
		f = &files[i];
		if (!(f->flags & FF_OPEN)) { break; }
	}
	/* In Y or Z even when UZEFS_MAX_FILES is 1 and f is a constant, so its
	** fields are ldd/std (2 bytes) rather than lds/sts (4). */
	__asm__ ("" : "+b" (f));
	memset(f, 0, sizeof(*f));

	if (c.flags & SC_FOUND) {
		f->dir_sect = c.hit_sect;
		f->dir_index = c.hit_idx;
		f->sclust = ent_clust(c.ent);
		f->fsize = rd32(c.ent + 28);
		if (f->sclust == 1 || f->sclust >= vol.max_clust || (f->fsize && !f->sclust)) {
			return UZEFS_ERR_CORRUPT;
		}
	}
#if UZEFS_WRITE
	else {
		if (!(c.flags & SC_FREE)) {
#if UZEFS_FAT32 && UZEFS_ROOT_GROW
			if (!IS32) { return UZEFS_ERR_DIR_FULL; }
			res = dir_grow(&c);
			if (res) { return res; }
#else
			return UZEFS_ERR_DIR_FULL;
#endif
		}
		f->dir_sect = c.free_sect;
		f->dir_index = c.free_idx;
		res = dir_write(f, nm);
		if (res) { return res; }
	}
#endif
	f->flags = (uint8_t)(FF_OPEN | ((mode & (UZEFS_O_READ | UZEFS_O_WRITE)) << 1));
#if UZEFS_WRITE
	if ((mode & UZEFS_O_TRUNC) && f->sclust) { res = cut_all(f); }
#if UZEFS_APPEND
	if (!res && (mode & UZEFS_O_APPEND)) { res = uzefs_seek(i, f->fsize); }
#endif
	if (res) {
		f->flags = 0;
		return res;
	}
#endif
	*fh = i;
	return UZEFS_OK;
}

uzefs_res uzefs_close(uint8_t fh)
{
	slot     *f = get_slot(fh);
	uzefs_res res = UZEFS_OK;

	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
#if UZEFS_WRITE
	res = uzefs_sync(fh);
	if (res == UZEFS_ERR_NOBUF) { return res; }
#endif
	f->flags = 0;
	return res;
}

uzefs_res uzefs_read(uint8_t fh, uint8_t *buf, uint16_t n, uint16_t *done)
{
	return xfer(fh, buf, n, done, 0);
}

#if UZEFS_WRITE
uzefs_res uzefs_write(uint8_t fh, const uint8_t *buf, uint16_t n, uint16_t *done)
{
	uzefs_res res = xfer(fh, (uint8_t *)buf, n, done, 1);
#if UZEFS_SIZE_EVERY_WRITE
	if (!res && *done) { res = uzefs_sync(fh); }
#endif
	return res;
}

uzefs_res uzefs_sync(uint8_t fh)
{
	slot     *f = get_slot(fh);
	uzefs_res res = UZEFS_OK;

	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
	if (f->flags & FF_FAILED) { return UZEFS_ERR_DISK; }
	if (f->flags & FF_DIRTY) {
		if (!vol.win) { return UZEFS_ERR_NOBUF; }
		res = dir_write(f, 0);
	}
#if UZEFS_KEEP
	/* The card is complete once sync returns, whichever file the kept
	** sector belongs to. */
	if (!res) { res = win_flush(); }
#endif
	f->flags = (uint8_t)(f->flags & ~FF_DIRTY) | (res ? FF_FAILED : 0);
	return res;
}

/* Cuts the file at fptr, freeing everything after it, including clusters
** linked past the old size. To 0, the entry lets go of the chain before it is
** freed. Otherwise the chain is cut first (end-of-chain in the cluster holding
** the last byte kept), then the entry gets the size, then the rest is freed:
** a crash in between leaves the size past the chain, never a chain past the
** size (link_new says why). */
uzefs_res uzefs_truncate(uint8_t fh)
{
	slot     *f = get_slot(fh);
	uzefs_res res;
	uint16_t  done;
	clust_t   rest;

	/* The zero-length write checks handle, failure, mode and buffer. */
	res = xfer(fh, 0, 0, &done, 1);
	if (res) { return res; }
	if (f->cclust) {
		f->fsize = f->fptr;
		res = fat_next(f->cclust, &rest);
		if (!res && rest) { res = fat_put(f->cclust, CL_EOC); }
		if (!res) { res = cut_chain(f, rest); }
	} else {
		res = cut_all(f);
	}
	if (res == UZEFS_ERR_DISK) { f->flags |= FF_FAILED; }
	return res;
}
#endif

uzefs_res uzefs_seek(uint8_t fh, uint32_t pos)
{
	slot     *f = get_slot(fh);
	uzefs_res res = UZEFS_OK;
	uint32_t  sect = 0, n;
	clust_t   cl = 0;
#if UZEFS_WRITE && UZEFS_SEEK_EXTEND
	uint16_t  done;
#endif

	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
	if (f->flags & FF_FAILED) { return UZEFS_ERR_DISK; }
	if (pos > f->fsize) {
#if UZEFS_WRITE && UZEFS_SEEK_EXTEND
		/* The zero-length call checks mode and buffer before anything moves. */
		res = xfer(fh, 0, 0, &done, 1);
		if (!res) { res = uzefs_seek(fh, f->fsize); }
		while (!res && f->fptr < pos) {
			n = pos - f->fptr;
			res = xfer(fh, 0, (n > 0x8000) ? 0x8000 : (uint16_t)n, &done, 1);
		}
		return res;
#else
		return UZEFS_ERR_RANGE;
#endif
	}
	if (pos) {
		cl = f->sclust;
		for (n = (pos - 1) >> (vol.cshift + 9); n; n--) {
			res = fat_next(cl, &cl);
			if (!res && !cl) { res = UZEFS_ERR_CORRUPT; }
			if (res) {
				if (res == UZEFS_ERR_DISK) { f->flags |= FF_FAILED; }
				return res;
			}
		}
		sect = clust_sect(cl) + (((pos - 1) >> 9) & vol.cmask);
	}
	f->cclust = cl;
	f->csect = sect;
	f->fptr = pos;
	return UZEFS_OK;
}

uint32_t uzefs_tell(uint8_t fh)
{
	slot *f = get_slot(fh);
	return f ? f->fptr : 0;
}

uint32_t uzefs_size(uint8_t fh)
{
	slot *f = get_slot(fh);
	return f ? f->fsize : 0;
}

uint8_t uzefs_eof(uint8_t fh)
{
	slot *f = get_slot(fh);
	return f ? (f->fptr >= f->fsize) : 1;
}


/*--------------------------------------------------------------------------*/
/* Names in the directory                                                   */
/*--------------------------------------------------------------------------*/

uzefs_res uzefs_stat(const char *name, uzefs_info *fi)
{
	uint8_t   nm[11];
	dscan     c;
	uzefs_res res;

	res = lookup(name, nm, &c);
	if (res) { return res; }
	if (!(c.flags & SC_FOUND)) { return UZEFS_ERR_NO_FILE; }
	if (fi) { info_fill(fi, c.ent); }
	return UZEFS_OK;
}

#if UZEFS_WRITE
/* Finds a file to delete or rename: it exists, is not a directory, volume
** label or read-only, is not open, and a buffer is lent. */
static uzefs_res find_closed(const char *name, dscan *c)
{
	uint8_t   nm[11];
	uzefs_res res;

	res = lookup(name, nm, c);
	if (res) { return res; }
	if (!(c->flags & SC_FOUND)) { return UZEFS_ERR_NO_FILE; }
	if (c->ent[11] & (UZEFS_AM_DIR | UZEFS_AM_VOL | UZEFS_AM_RDO)) { return UZEFS_ERR_DENIED; }
	if (busy(c, FF_OPEN)) { return UZEFS_ERR_LOCKED; }
	return vol.win ? UZEFS_OK : UZEFS_ERR_NOBUF;
}

uzefs_res uzefs_delete(const char *name)
{
	dscan     c;
	uzefs_res res;

	res = find_closed(name, &c);
	if (!res) { res = dir_remove(&c, 0); }
	if (!res) { res = free_chain(ent_clust(c.ent)); }
	return res;
}

uzefs_res uzefs_rename(const char *from, const char *to)
{
	uint8_t   nm[11];
	dscan     c;
	uzefs_res res;

	res = lookup(to, nm, &c);
	if (res) { return res; }
	if (c.flags & SC_FOUND) { return UZEFS_ERR_EXISTS; }
	res = find_closed(from, &c);
	if (!res) { res = dir_remove(&c, nm); }
	return res;
}
#endif

#if UZEFS_DIR
uzefs_res uzefs_opendir(uzefs_dir *d)
{
	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
	d->sector = vol.dir_sect;
	d->index = 0;
	return UZEFS_OK;
}

uzefs_res uzefs_readdir(uzefs_dir *d, uzefs_info *fi)
{
	dscan     c;
	uzefs_res res;

	if (!fi) { return uzefs_opendir(d); }
	fi->name[0] = 0;
	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }

	memset(&c, 0, sizeof(c));
	c.flags = SC_LIST;
	res = dir_scan(&c, d->sector, d->index);
	if (res) { return res; }
	if (c.flags & SC_FOUND) {
		info_fill(fi, c.ent);
		d->sector = c.hit_sect;
		d->index = (uint8_t)(c.hit_idx + 1);    /* 16: dir_scan moves on */
	} else {
		d->sector = 0;
	}
	return UZEFS_OK;
}
#endif


/*--------------------------------------------------------------------------*/
/* Streaming                                                                */
/*--------------------------------------------------------------------------*/

uzefs_res uzefs_fragments(uint8_t fh, uint16_t *n)
{
	slot     *f = get_slot(fh);
	uzefs_res res;
	uint32_t  left, k, sect;
	clust_t   cl, nx;
	uint16_t  runs = 0, ofs;

	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
	if (f->flags & FF_FAILED) { return UZEFS_ERR_DISK; }
	if (f->fsize) {
		runs = 1;
		cl = f->sclust;
		/* as uzefs_seek to the end: the clusters the size needs, no further */
		left = (f->fsize - 1) >> (vol.cshift + 9);
		/* In one piece, the entries of those clusters count up from the file's
		** second: one multiple-block read checks them all (a 200 MB file on 4 KB
		** clusters: 420 sectors in a row, not 1,600 reads). From a break on, the
		** walk below counts the pieces. */
		if (left && cl >= 2 && cl + left < VOL.max_clust) {
			sect = fat_loc(cl, &ofs);
			res = uzefs_sd_count(sect, ofs, (uint32_t)cl + 1, left, IS32 ? 1 : 0, &k);
			if (res) {
				f->flags |= FF_FAILED;
				return res;
			}
			cl += (clust_t)k;
			left -= k;
		}
		for (; left; left--) {
			res = fat_next(cl, &nx);
			if (!res && !nx) { res = UZEFS_ERR_CORRUPT; }
			if (res) {
				if (res == UZEFS_ERR_DISK) { f->flags |= FF_FAILED; }
				return res;
			}
			if (nx != cl + 1 && runs != 0xFFFF) { runs++; }
			cl = nx;
		}
	}
	if (runs == 1) { f->flags |= FF_CONTIG; }
	*n = runs;
	return UZEFS_OK;
}

uzefs_res uzefs_extent(uint8_t fh, uint32_t pos, uint32_t *sect, uint32_t *run)
{
	slot     *f = get_slot(fh);
	uzefs_res res = UZEFS_OK;
	uint32_t  i, end, last;     /* clusters, counted from the file's first */
	clust_t   cl, at, nx;

	if (!f) { return UZEFS_ERR_BAD_HANDLE; }
	if (f->flags & FF_FAILED) { return UZEFS_ERR_DISK; }
	if (pos >= f->fsize) { return UZEFS_ERR_RANGE; }
	end  = pos >> (vol.cshift + 9);
	last = (f->fsize - 1) >> (vol.cshift + 9);
	cl = f->sclust;
	if (f->flags & FF_CONTIG) {
		at = cl + (clust_t)end;
		end = last;
	} else {
		for (i = end; i; i--) {
			res = fat_next(cl, &cl);
			if (!res && !cl) { res = UZEFS_ERR_CORRUPT; }
			if (res) { goto fail; }
		}
		at = cl;
		/* on through the run: end becomes its last cluster */
		while (end < last) {
			res = fat_next(cl, &nx);
			if (!res && !nx) { res = UZEFS_ERR_CORRUPT; }
			if (res) { goto fail; }
			if (nx != cl + 1) { break; }
			cl = nx;
			end++;
		}
	}
	*sect = clust_sect(at) + ((pos >> 9) & vol.cmask);
	i = (end + 1) << vol.cshift;                 /* sectors to the run's end */
	last = ((f->fsize - 1) >> 9) + 1;            /* sectors to the file's end */
	*run = ((i < last) ? i : last) - (pos >> 9);
	return UZEFS_OK;

fail:
	if (res == UZEFS_ERR_DISK) { f->flags |= FF_FAILED; }
	return res;
}

uzefs_res uzefs_stream_gap(uint8_t n, uint16_t *min, uint16_t *max)
{
	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
	return uzefs_sd_gap(vol.data_sect, n, min, max);
}

uzefs_res uzefs_stream_begin(uint32_t sect)
{
	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
	return uzefs_sd_stream_begin(sect);
}

uzefs_res uzefs_stream_end(void)
{
	return uzefs_sd_stream_end();
}

uzefs_res uzefs_stream_restart(uint32_t sect, uint16_t busy_max)
{
	if (!(vol.flags & VF_MOUNTED)) { return UZEFS_ERR_NOT_MOUNTED; }
	return uzefs_sd_stream_restart(sect, busy_max);
}


uint32_t uzefs_time(void) __attribute__((weak));
uint32_t uzefs_time(void)
{
	return UZEFS_FIXED_DATE;
}
