/*
 *  UzeFS SD card driver over a version 5 bootloader (UZEFS_BOOTLOADER=1), in
 *  place of uzefs_sd.c: the card's init, the mount's search for the volume, the
 *  commands and the release are the bootloader's (../bootld5.h); the data loops
 *  are uzefs_sd.c's, at the full SPI clock with the card's CRC turned off.
 *
 *  - raises PA4, the SPI RAM chip select, and clears SPIF before every call of
 *    the bootloader's (they wait on SPIF, which the SPI RAM's routines leave set);
 *  - the bootloader's command waits up to 16 bytes for R1, and its release
 *    deselects the card and clocks one byte.
 */

#include <avr/io.h>
#include <bootld5.h>
#include "uzefs_sd.h"

#if UZEFS_BOOTLOADER	/* uzefs_sd.c is the driver otherwise */

extern void WaitUs(unsigned int microseconds);

#define RAM_OFF()   PORTA |= (1U << 4)

#define CMD17   17          // READ_SINGLE_BLOCK
#define CMD24   24          // WRITE_BLOCK
#define CMD59   59          // CRC_ON_OFF

static uint8_t sdhc;        // the card is block-addressed
void (*uzefs_sd_handler)(uint8_t);


static uint8_t rcv_spi(void)
{
	SPDR = 0xFF;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

/* Inside a sector: the byte in flight, once it is in, with the next one
** started before this one is returned (uzefs_sd.c's). */
static inline __attribute__((always_inline)) uint8_t next_spi(void)
{
	uint8_t d;

	loop_until_bit_is_set(SPSR, SPIF);
	d = SPDR;
	SPDR = 0xFF;
	return d;
}

/* The bus from the SPI RAM routines: its chip select up, SPIF cleared. */
static void spi_claim(void)
{
	RAM_OFF();
	(void)SPSR;
	(void)SPDR;
}

static uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
	spi_claim();
	return bl5_sd_command(cmd, arg);
}

/* A read or write command for sector sect, byte-addressed unless the card is
** block-addressed (SDHC). */
static uint8_t __attribute__((noinline)) cmd_sect(uint8_t cmd, uint32_t sect)
{
	if (!sdhc) { sect <<= 9; }
	return send_cmd(cmd, sect);
}


uzefs_res uzefs_sd_mount(sdc_struct_t *v)
{
	uint8_t r;

	if (!bl5_present()) { return UZEFS_ERR_NOT_READY; }
	spi_claim();
	r = bl5_fat_init(v);
	if (r) { return r == 7 ? UZEFS_ERR_NO_FS : r == 6 ? UZEFS_ERR_DISK : UZEFS_ERR_NOT_READY; }
	sdhc = v->flags & SDC_FLAGS_SDHC;
	/* No CRC on what UzeFS reads and writes, and the SPI RAM's clock, which is
	** the card's from here on */
	send_cmd(CMD59, 0);
	bl5_sd_release();
	SPCR = (1U << MSTR) | (1U << SPE);
	SPSR = 0x01;
	return UZEFS_OK;
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

	bl5_sd_release();
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

	bl5_sd_release();
	return res;
}
#endif

#endif /* UZEFS_BOOTLOADER */
