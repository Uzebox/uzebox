/*
 *  UzeFS SD card driver for the Uzebox. Derived from Petit FatFs's AVR disk
 *  module, (C)ChaN 2010, as adapted in kernel/petitfatfs/diskio.c.
 *
 *  Differences from that file:
 *  - raises PA4, the SPI RAM chip select, before every command, so a card
 *    command can never reach the SPI RAM as well;
 *  - a sector write is a single call that sends all 512 bytes;
 *  - streamed reads go to a handler function instead of FORWARD();
 *  - clears SPIF before every command (spi_claim);
 *  - inside a sector the bytes overlap: the next one is started before the
 *    last is stored or counted (next_spi, and the write's loop).
 *  - init ends a read the card may still be in (CMD12) before CMD0;
 *  - uzefs_sd_gap times a multiple-block read (CMD18);
 *  - uzefs_sd_stream_begin/end start and stop a multiple-block read that
 *    the host clocks itself, uzefs_sd_stream_restart moves it;
 *  - uzefs_sd_reg reads the CID or the CSD;
 *  - uzefs_sd_count checks a run of FAT entries in one multiple-block read.
 */

#include <avr/io.h>
#include "uzefs_sd.h"

#if !UZEFS_BOOTLOADER	/* uzefs_bl.c is the driver then */

extern void WaitUs(unsigned int microseconds);

#define SELECT()    PORTD &= (uint8_t)~(1U << 6)
#define DESELECT()  PORTD |= (1U << 6)
#define RAM_OFF()   PORTA |= (1U << 4)

#define CMD0    (0x40 + 0)   // GO_IDLE_STATE: software reset, and enters SPI mode
#define CMD8    (0x40 + 8)   // SEND_IF_COND: voltage check; only an SD v2+ card answers it
#define CMD9    (0x40 + 9)   // SEND_CSD: the card's 16-byte CSD register, as a data block
#define CMD10   (0x40 + 10)  // SEND_CID: the card's 16-byte CID register, as a data block
#define CMD12   (0x40 + 12)  // STOP_TRANSMISSION: ends a multiple-block read
#define CMD16   (0x40 + 16)  // SET_BLOCKLEN: fixes the block length, 512 here
#define CMD17   (0x40 + 17)  // READ_SINGLE_BLOCK: reads one sector
#define CMD18   (0x40 + 18)  // READ_MULTIPLE_BLOCK: reads sectors one after another until CMD12
#define CMD24   (0x40 + 24)  // WRITE_BLOCK: writes one sector
#define CMD55   (0x40 + 55)  // APP_CMD: announces that the next command is an ACMD
#define CMD58   (0x40 + 58)  // READ_OCR: its CCS bit says whether the card is block-addressed (SDHC)
#define ACMD41  (0xC0 + 41)  // SD_SEND_OP_COND: SD card init; bit 7 makes send_cmd() send CMD55 first
#define CMD1    (0x40 + 1)   // SEND_OP_COND: MMC card init, the fallback when ACMD41 is refused

#define CT_MMC    0x01
#define CT_SD1    0x02
#define CT_SD2    0x04
#define CT_BLOCK  0x08

static uint8_t card_type;
void (*uzefs_sd_handler)(uint8_t);


static void xmit_spi(uint8_t d)
{
	SPDR = d;
	loop_until_bit_is_set(SPSR, SPIF);
}

static uint8_t rcv_spi(void)
{
	SPDR = 0xFF;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

/* Inside a sector: the byte in flight, once it is in, with the next one
** started before this one is returned, so that it clocks while the caller
** stores and counts. Needs a byte already started, and leaves one started.
** The wait reads SPSR with SPIF set and the read of SPDR then clears it, so no
** byte is started with SPIF left set. */
static inline __attribute__((always_inline)) uint8_t next_spi(void)
{
	uint8_t d;

	loop_until_bit_is_set(SPSR, SPIF);
	d = SPDR;
	SPDR = 0xFF;
	return d;
}

/* Takes the bus from the SPI RAM routines. They time their bytes by counting
** cycles and never clear SPIF, so it is still set when they return, with no
** byte in progress. Left set, the wait in rcv_spi returns while its byte is
** still going out, and the next write to SPDR in that time is dropped: send_cmd
** then lost its command byte whenever the code between two bytes ran slowly
** enough, and the card, reading the argument as a command, never answered.
** Reading SPSR and then SPDR clears SPIF. */
static void spi_claim(void)
{
	(void)SPSR;
	(void)SPDR;
}

static void release(void)
{
	DESELECT();
	rcv_spi();
}

/* Ends a multiple-block read wherever in a sector the card is, and leaves the
** card selected. CMD12 goes out as the card keeps sending; the byte after it
** is a stuff byte, then comes R1, then the line is held low until the card is
** ready again. A card that is not in a read answers it as an illegal command,
** or not at all, and is not busy. UZEFS_ERR_DISK if the busy outlasts busy_max
** bytes. */
static uzefs_res stop_read(uint16_t busy_max)
{
	uint16_t bc;
	uint8_t  rc;

	SELECT();
	xmit_spi(CMD12);
	for (bc = 4; bc; bc--) { xmit_spi(0); }
	xmit_spi(0x61);                               /* CMD12's CRC */
	rcv_spi();
	bc = 10;
	do {
		rc = rcv_spi();
	} while ((rc & 0x80) && --bc);
	bc = busy_max;
	do {
		rc = rcv_spi();
	} while (rc != 0xFF && --bc);
	return bc ? UZEFS_OK : UZEFS_ERR_DISK;
}

static uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t n, res;

	if (cmd & 0x80) {
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) { return res; }
	}

	RAM_OFF();
	DESELECT();
	spi_claim();
	rcv_spi();
	SELECT();
	rcv_spi();

	xmit_spi(cmd);
	xmit_spi((uint8_t)(arg >> 24));
	xmit_spi((uint8_t)(arg >> 16));
	xmit_spi((uint8_t)(arg >> 8));
	xmit_spi((uint8_t)arg);
	n = 0x01;
	if (cmd == CMD0) { n = 0x95; }
	if (cmd == CMD8) { n = 0x87; }
	xmit_spi(n);

	n = 10;
	do {
		res = rcv_spi();
	} while ((res & 0x80) && --n);

	return res;
}

/* Sends a read or write command for sector sect, byte-addressed unless the
** card is block-addressed (SDHC). */
static uint8_t __attribute__((noinline)) cmd_sect(uint8_t cmd, uint32_t sect)
{
#if UZEFS_SDHC
	if (!(card_type & CT_BLOCK))
#endif
	{ sect <<= 9; }
	return send_cmd(cmd, sect);
}


uzefs_res uzefs_sd_init(void)
{
	uint8_t  n, cmd, ty;
	uint16_t tmr;
	uint32_t arg;
#if UZEFS_SDHC
	uint8_t  ocr[4];
#endif

	PORTB |= (1U << PB5);
	DDRB  |= (1U << PB7) | (1U << PB5);
	PORTD |= (1U << PD6);
	DDRD  |= (1U << PD6);
	SPCR = (1U << MSTR) | (1U << SPE) | (1U << SPR1) | (1U << SPR0);
	SPSR = 0x00;

	RAM_OFF();
	DESELECT();
	spi_claim();
	for (n = 100; n; n--) { rcv_spi(); }

	/* A card left in a read (the console reset during a CMD18, or the ISP
	** programmer's traffic) takes no command, CMD0 included, until a CMD12
	** ends that read. A card that is not in a read ignores it. */
	stop_read(4000);                              /* 0.15 s at the slow clock */
	release();

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {
		cmd = ACMD41;
		arg = 0;
#if UZEFS_SDHC
		if (send_cmd(CMD8, 0x1AA) == 1) {
			for (n = 0; n < 4; n++) { ocr[n] = rcv_spi(); }
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
				ty = CT_SD2;
				arg = 1UL << 30;
			}
		} else
#endif
		if (send_cmd(ACMD41, 0) <= 1) {
			ty = CT_SD1;
		} else {
			ty = CT_MMC; cmd = CMD1;
		}
		if (ty) {
			for (tmr = 10000; tmr && send_cmd(cmd, arg); tmr--) { WaitUs(100); }
			if (!tmr) {
				ty = 0;
#if UZEFS_SDHC
			} else if (ty == CT_SD2) {
				if (send_cmd(CMD58, 0) == 0) {
					for (n = 0; n < 4; n++) { ocr[n] = rcv_spi(); }
					if (ocr[0] & 0x40) { ty |= CT_BLOCK; }
				} else {
					ty = 0;
				}
#endif
			} else if (send_cmd(CMD16, 512) != 0) {
				ty = 0;
			}
		}
	}
	card_type = ty;
	release();

	SPCR = (1U << MSTR) | (1U << SPE);
	SPSR = 0x01;

	return ty ? UZEFS_OK : UZEFS_ERR_NOT_READY;
}


uzefs_res uzefs_sd_read(uint8_t *buf, uint32_t sect, uint16_t ofs, uint16_t cnt)
{
	uzefs_res res = UZEFS_ERR_DISK;
	uint8_t  rc;
	uint16_t bc;

	if (cmd_sect(CMD17, sect) == 0) {
		bc = 40000;
		do {
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {
			/* The sector and its CRC, 514 bytes: the first is started here
			** and each next_spi() starts the one after, so the last is
			** waited for alone. */
			bc = 513 - ofs - cnt;
			SPDR = 0xFF;
			while (ofs) { next_spi(); ofs--; }
			if (buf) {
				while (cnt) { *buf++ = next_spi(); cnt--; }
			} else {
				while (cnt) { uzefs_sd_handler(next_spi()); cnt--; }
			}
			do { next_spi(); } while (--bc);
			loop_until_bit_is_set(SPSR, SPIF);
			(void)SPDR;
			res = UZEFS_OK;
		}
	}

	release();
	return res;
}


/* A multiple-block read (CMD18) of n + 1 sectors from sect, kept only for its
** timing. Between the last data byte of one sector and the first of the next
** the card sends 2 CRC bytes, then 0xFF for as long as it needs to fetch the
** sector, then the 0xFE token. That whole run is one gap, and what a host
** streaming with CMD18 must clock through between sectors. */
uzefs_res uzefs_sd_gap(uint32_t sect, uint8_t n, uint16_t *min, uint16_t *max)
{
	uzefs_res res = UZEFS_ERR_DISK;
	uint16_t  bc, gap, lo = 0xFFFF, hi = 0;
	uint8_t   rc, first = 1;

	if (cmd_sect(CMD18, sect) == 0) {
		res = UZEFS_OK;
		for (;;) {
			/* 0xFF until the token; the wait before the first sector is the
			** card's access time, not a gap */
			gap = 2;
			bc = 40000;
			do {
				rc = rcv_spi();
				gap++;
			} while (rc == 0xFF && --bc);
			if (rc != 0xFE) { res = UZEFS_ERR_DISK; break; }
			if (!first) {
				if (gap < lo) { lo = gap; }
				if (gap > hi) { hi = gap; }
				if (!n--) { break; }
			} else if (!n--) {
				n = 0;                      /* n 0 measures one gap, as n 1 does */
			}
			first = 0;
			for (bc = 514; bc; bc--) { rcv_spi(); }     /* the data and its CRC */
		}

		if (stop_read(40000)) { res = UZEFS_ERR_DISK; }
	}

	release();
	if (!res) {
		*min = lo;
		*max = hi;
	}
	return res;
}


uzefs_res uzefs_sd_count(uint32_t sect, uint16_t ofs, uint32_t v, uint32_t n, uint8_t wide,
                         uint32_t *k)
{
	uzefs_res res = UZEFS_ERR_DISK;
	uint32_t  done = 0;
	uint16_t  bc;
	uint8_t   rc, i, x;
	uint8_t  *p = (uint8_t *)&v;      /* v's bytes, low first, as the card sends a word's */

	wide = wide ? 4 : 2;
	if (cmd_sect(CMD18, sect) == 0) {
		res = UZEFS_OK;
		while (done < n) {
			bc = 40000;
			do {
				rc = rcv_spi();
			} while (rc == 0xFF && --bc);
			if (rc != 0xFE) { res = UZEFS_ERR_DISK; break; }
			for (bc = ofs; bc; bc--) { rcv_spi(); }
			for (bc = 512 - ofs; bc && done < n; bc -= wide) {
				x = 0;
				for (i = 0; i < wide; i++) {
					rc = rcv_spi();
					if (i == 3) { rc &= 0x0F; }
					x |= rc ^ p[i];
				}
				if (x) { n = done; break; }
				v++;
				done++;
			}
			ofs = 0;
			if (done < n) {
				for (bc += 2; bc; bc--) { rcv_spi(); }  /* the sector's rest, its CRC */
			}
		}
		if (stop_read(40000)) { res = UZEFS_ERR_DISK; }
	}
	release();
	*k = done;
	return res;
}


uzefs_res uzefs_sd_stream_begin(uint32_t sect)
{
	if (cmd_sect(CMD18, sect) == 0) { return UZEFS_OK; }
	release();
	return UZEFS_ERR_DISK;
}

uzefs_res uzefs_sd_stream_end(void)
{
	uzefs_res res;

	/* the host may have stopped the clock by turning the SPI off */
	SPCR = (1U << MSTR) | (1U << SPE);
	spi_claim();
	res = stop_read(40000);
	release();
	return res;
}


uzefs_res uzefs_sd_stream_restart(uint32_t sect, uint16_t busy_max)
{
	SPCR = (1U << MSTR) | (1U << SPE);
	spi_claim();
	if (stop_read(busy_max)) { return UZEFS_ERR_DISK; }   /* still busy: selected */
	return uzefs_sd_stream_begin(sect);
}


uzefs_res uzefs_sd_reg(uint8_t which, uint8_t *buf)
{
	uzefs_res res = UZEFS_ERR_DISK;
	uint16_t  bc;
	uint8_t   rc;

	if (send_cmd(which ? CMD9 : CMD10, 0) == 0) {
		bc = 40000;
		do {
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);
		if (rc == 0xFE) {
			for (bc = 0; bc < 16; bc++) { buf[bc] = rcv_spi(); }
			rcv_spi();
			rcv_spi();
			res = UZEFS_OK;
		}
	}
	release();
	return res;
}


#if UZEFS_WRITE
uzefs_res uzefs_sd_write(const uint8_t *buf, uint32_t sect)
{
	uzefs_res res = UZEFS_ERR_DISK;
	uint16_t bc;
	uint8_t  rc;

	if (cmd_sect(CMD24, sect) == 0) {
		rcv_spi();
		/* The data token, then the sector: each byte is fetched and
		** counted while the one before it clocks, and sent when that one
		** is out. The write of SPDR after the wait clears SPIF. */
		SPDR = 0xFE;
		bc = 512;
		do {
			rc = buf ? *buf++ : 0;
			loop_until_bit_is_set(SPSR, SPIF);
			SPDR = rc;
		} while (--bc);
		loop_until_bit_is_set(SPSR, SPIF);
		rcv_spi();
		rcv_spi();
		bc = 10;
		do {
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);
		if ((rc & 0x1F) == 0x05) {
			for (bc = 5000; rcv_spi() != 0xFF && bc; bc--) { WaitUs(100); }
			if (bc) { res = UZEFS_OK; }
		}
	}

	release();
	return res;
}
#endif

#endif /* !UZEFS_BOOTLOADER */
