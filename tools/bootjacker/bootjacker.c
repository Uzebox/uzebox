#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "bootjacker.h"
#include "source.h"

#define BOOT_BASE 0xF000U
#define LAST_PAGE 0xFF00U
bj_report_t bj_report;
uint16_t bj_entry_word, bj_irq_word, bj_saved_sp;
uint8_t bj_register, bj_ticks;

/* Save peripheral state; mask IRQs because the V5 helper executes SEI.
 * Repurposing Timer0 interrupts the AD725 video clock. */
typedef struct {
    uint8_t sreg, t0a, t0b, count, ocra, ocrb, m0, m1, m2;
    uint8_t ext, pc, uart, adc, ac, spi, twi, ee;
} machine_t;
static machine_t saved;

static void suspend_machine(void) {
    saved.sreg = SREG;
    cli();
    eeprom_busy_wait();
    boot_spm_busy_wait();
    saved.m0 = TIMSK0; saved.m1 = TIMSK1; saved.m2 = TIMSK2;
    saved.ext = EIMSK; saved.pc = PCICR; saved.uart = UCSR0B;
    saved.adc = ADCSRA; saved.ac = ACSR; saved.spi = SPCR;
    saved.twi = TWCR; saved.ee = EECR;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; EIMSK = 0; PCICR = 0;
    UCSR0B &= ~(_BV(RXCIE0) | _BV(TXCIE0) | _BV(UDRIE0));
    ADCSRA &= ~(_BV(ADIE) | _BV(ADIF));
    ACSR &= ~(_BV(ACIE) | _BV(ACI));
    SPCR &= ~_BV(SPIE);
    TWCR &= ~(_BV(TWIE) | _BV(TWINT));
    EECR &= ~_BV(EERIE);
    SPMCSR = 0;
    saved.t0a = TCCR0A; saved.t0b = TCCR0B; saved.count = TCNT0;
    saved.ocra = OCR0A; saved.ocrb = OCR0B;
    TCCR0B = 0; TCCR0A = 0;
    TIMSK0 = _BV(OCIE0A);
}

static void restore_machine(void) {
    cli();
    TIMSK0 = 0; TCCR0B = 0;
    SPMCSR = 0;
    OCR0A = saved.ocra; OCR0B = saved.ocrb; TCNT0 = saved.count;
    TIFR0 = _BV(OCF0A) | _BV(OCF0B) | _BV(TOV0);
    TCCR0A = saved.t0a; TCCR0B = saved.t0b;
    TIMSK0 = saved.m0; TIMSK1 = saved.m1; TIMSK2 = saved.m2;
    EIMSK = saved.ext; PCICR = saved.pc; UCSR0B = saved.uart;
    ADCSRA = saved.adc & ~_BV(ADIF); ACSR = saved.ac & ~_BV(ACI);
    SPCR = saved.spi; TWCR = saved.twi & ~_BV(TWINT); EECR = saved.ee;
    SREG = saved.sreg;
}

static uint16_t word_at(uint16_t a) { return pgm_read_word(a); }

static uint32_t crc_flash(const uint8_t *p) {
    uint32_t crc = 0xFFFFFFFFUL;
    uint16_t i;
    for (i = 0; i < 4096; ++i) {
        uint8_t n;
        crc ^= pgm_read_byte(p + i);
        for (n = 0; n < 8; ++n)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320UL : 0);
    }
    return ~crc;
}

static uint8_t matches(uint16_t page) {
    uint16_t i;
    for (i = 0; i < 256; ++i)
        if (pgm_read_byte(page + i) != bj_page[i]) return 0;
    return 1;
}

/* Require a single-page OUT/STS SPMCSR,rN; SPM; SEI/NOP below FF00.
 * Exclude dispatch registers: r0:r1, r18:r19, X, and Z. */
static uint8_t find_gadget(void) {
    uint16_t a;
    for (a = BOOT_BASE; a < LAST_PAGE - 6; a += 2) {
        uint16_t op = word_at(a), spm;
        uint8_t reg;
        if ((op & 0xFE0F) == 0x9200 && word_at(a + 2) == _SFR_MEM_ADDR(SPMCSR))
            spm = a + 4;
        else if ((op & 0xF800) == 0xB800 &&
                 ((op & 15) | ((op >> 5) & 0x30)) == _SFR_IO_ADDR(SPMCSR))
            spm = a + 2;
        else continue;
        reg = (op >> 4) & 31;
        if (reg < 2 || reg == 18 || reg == 19 || reg == 26 || reg == 27 || reg >= 30)
            continue;
        if ((a >> 8) != ((spm + 3) >> 8)) continue;
        if (word_at(spm) != 0x95E8) continue;
        if (word_at(spm + 2) != 0x9478 && word_at(spm + 2) != 0x0000) continue;
        bj_report.gadget = a;
        bj_report.command_register = reg;
        return (uint8_t)(spm - a);
    }
    return 0;
}

static uint8_t test_timing(uint8_t prefix_bytes) {
    uint16_t probe = prefix_bytes == 4 ? (uint16_t)bj_probe_sts : (uint16_t)bj_probe_out;
    uint16_t after = probe + prefix_bytes / 2 + 1;
    uint8_t t, first = 0, last = 0;
    bj_register = 5;
    bj_entry_word = probe;
    /* Center the timing window with the interrupt PC immediately after SPM. */
    for (t = 4; t <= 20; ++t) {
        bj_ticks = t;
        bj_invoke(LAST_PAGE, 0, 0);
        if (bj_irq_word == after) {
            if (!first) first = t;
            last = t;
        }
    }
    if (!first || last - first < 1) return BJ_TIMING;
    bj_ticks = first + (last - first) / 2;
    bj_entry_word = bj_report.gadget / 2;
    bj_register = bj_report.command_register;
    after = bj_entry_word + prefix_bytes / 2 + 1;
    /* SPMEN=0: this exercises the real bootloader site without writing. */
    for (t = 0; t < 32; ++t) {
        bj_invoke(LAST_PAGE, 0, 0);
        bj_report.stopped_pc = bj_irq_word * 2;
        if (bj_irq_word != after) return BJ_TIMING;
    }
    bj_report.delay = bj_ticks;
    return BJ_OK;
}

static uint8_t preflight_inner(void) {
    uint8_t prefix, result;
    bj_report.signature[0] = boot_signature_byte_get(0);
    bj_report.signature[1] = boot_signature_byte_get(2);
    bj_report.signature[2] = boot_signature_byte_get(4);
    bj_report.high_fuse = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
    bj_report.locks = boot_lock_fuse_bits_get(GET_LOCK_BITS);
    /* Require the ATmega644/644A interrupt layout. */
    if (bj_report.signature[0] != 0x1E || bj_report.signature[1] != 0x96 ||
        bj_report.signature[2] != 0x09) return BJ_DEVICE;
    if ((bj_report.high_fuse & 7) != 2 || !(bj_report.high_fuse & _BV(4))) return BJ_FUSES;
    if ((bj_report.locks & 0x30) != 0x30) return BJ_LOCKED;
    bj_report.installed_crc = crc_flash((const uint8_t *)BOOT_BASE);
    /* Report the big-endian V5 API version independently of payload identity. */
    bj_report.api_version = 0;
    if (word_at(BOOT_BASE + 2) == 0x07B0 && word_at(BOOT_BASE + 4) == 0xAD10)
        bj_report.api_version = ((uint16_t)pgm_read_byte(BOOT_BASE + 6) << 8) |
                               pgm_read_byte(BOOT_BASE + 7);
    if (bj_report.installed_crc == bj_target_crc) {
        uint8_t p;
        for (p = 0xF0; p != 0; ++p) {
            result = bj_source_page(p - 0xf0);
            if (result) return result;
            if (!matches((uint16_t)p << 8)) break;
        }
        if (p == 0) return BJ_ALREADY;
    }
    prefix = find_gadget();
    if (!prefix) return bj_report.api_version >= 0x5000 ? BJ_V5_SITE : BJ_NO_GADGET;
    result = test_timing(prefix);
    return result;
}

uint8_t bj_preflight(void) {
    uint8_t result = bj_source_verify();
    if (result) return result;
    suspend_machine();
    result = preflight_inner();
    restore_machine();
    return result;
}

static uint8_t native_ready;

static uint8_t command(uint16_t address, uint16_t data, uint8_t op) {
    /* Every destructive operation is confined to the 4 KiB NRWW boot area. */
    if (address < BOOT_BASE || (address & 1)) return 0;
    if (native_ready) {
        bj_native(address, data, op);
    } else {
        uint16_t spm = bj_report.gadget +
            (((word_at(bj_report.gadget) & 0xFE0F) == 0x9200) ? 4 : 2);
        bj_invoke(address, data, op);
        bj_report.stopped_pc = bj_irq_word * 2;
        /* Erase/write can have one instruction of post-SPM latency.
         * The scanner admitted only SEI/NOP in this slot. */
        if (bj_report.stopped_pc != spm + 2 && bj_report.stopped_pc != spm + 4) return 0;
    }
    boot_spm_busy_wait();
    return 1;
}

static uint8_t program_page(uint16_t page) {
    uint8_t attempt, result;
    uint16_t offset;
    bj_report.failed_page = page;
    result = bj_source_page((page - BOOT_BASE) >> 8);
    if (result) return result;
    if (matches(page)) return BJ_OK;
    for (attempt = 0; attempt < 3; ++attempt) {
        /* Clear stale temporary-buffer contents before each attempt. */
        if (!command(page, 0, _BV(RWWSRE) | _BV(SPMEN))) return BJ_TIMING;
        if (!command(page, 0, _BV(PGERS) | _BV(SPMEN))) return BJ_TIMING;
        for (offset = 0; offset < 256; offset += 2) {
            uint16_t data = (uint16_t)bj_page[offset + 1] << 8 | bj_page[offset];
            if (!command(page + offset, data, _BV(SPMEN))) return BJ_TIMING;
        }
        if (!command(page, 0, _BV(PGWRT) | _BV(SPMEN))) return BJ_TIMING;
        if (matches(page)) return BJ_OK;
    }
    bj_report.failed_page = page;
    return BJ_VERIFY;
}

uint8_t bj_install(void) {
    uint8_t result, p;
    result = bj_source_verify();
    if (result) return result;
    suspend_machine();
    native_ready = 0;
    result = preflight_inner();
    if (result != BJ_OK) goto done;
    /* Install the V5 helper page using the intact instruction site below FF00. */
    result = program_page(LAST_PAGE);
    if (result != BJ_OK) goto done;
    TIMSK0 = 0;
    native_ready = 1;
    /* Write F100..FE00, then the reset-vector page F000 last. */
    for (p = 0xF1; p < 0xFF; ++p) {
        result = program_page((uint16_t)p << 8);
        if (result != BJ_OK) goto done;
    }
    result = program_page(BOOT_BASE);
    if (result != BJ_OK) goto done;
    for (p = 0xF0; p != 0; ++p) {
        bj_report.failed_page = (uint16_t)p << 8;
        result = bj_source_page(p - 0xf0);
        if (result) goto done;
        if (!matches((uint16_t)p << 8)) {
            bj_report.failed_page = (uint16_t)p << 8;
            result = BJ_VERIFY;
            goto done;
        }
    }
done:
    restore_machine();
    return result;
}
