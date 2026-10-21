#include <avr/pgmspace.h>
#include <string.h>
#include "pff.h"
#include "diskio.h"
#include "bootjacker.h"
#include "source.h"
#include "payload_meta.h"

extern const uint8_t bj_payload[4096] PROGMEM;
uint8_t bj_page[256], bj_from_sd;
uint16_t bj_target_version, bj_native_word = BJ_NATIVE_BYTE / 2;
uint32_t bj_target_crc = BJ_PAYLOAD_CRC;
void (*bj_source_progress)(uint8_t page);
static FATFS fs;
static uint8_t input[128];
static uint16_t pos, used;
static uint32_t raw_crc, file_crc, page_crc[16];
static uint8_t read_error;

static uint32_t crc_byte(uint32_t crc, uint8_t b) {
    uint8_t i;
    crc ^= b;
    for (i = 0; i < 8; ++i) crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320UL : 0);
    return crc;
}
static uint32_t page_checksum(void) {
    uint32_t crc = 0xffffffffUL;
    uint16_t i;
    for (i = 0; i < 256; ++i) crc = crc_byte(crc, bj_page[i]);
    return ~crc;
}
static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

/* Resolve the exact long name to its checked short-name directory entry. */
static uint8_t open_hex(void) {
    static const char name[] PROGMEM = "BOOTLOADER.HEX";
    static const uint8_t offsets[] PROGMEM = {1,3,5,7,9,14,16,18,20,22,24,28,30};
    uint32_t cluster = fs.fs_type == FS_FAT32 ? fs.dirbase : 0;
    uint32_t sector;
    uint16_t index, in_cluster = 0;
    uint8_t expected = 0, checksum = 0, matched = 0;
    if (fs.fs_type == FS_FAT32 && (cluster < 2 || cluster >= fs.n_fatent)) return BJ_SD;
    sector = cluster ? fs.database + (cluster - 2) * fs.csize : fs.dirbase;
    for (index = 0; index < 4096; ++index) {
        uint8_t entry[32], i, valid;
        if (!cluster && index >= fs.n_rootdir) break;
        if (disk_readp(entry, sector, (index % 16) * 32, 32)) return BJ_SD;
        if (!entry[0]) break;
        if (entry[0] == 0xe5) { expected = matched = 0; }
        else if (entry[11] == 0x0f) {
            uint8_t ordinal = entry[0] & 0x1f;
            if (entry[0] == 0x42) { expected = 2; checksum = entry[13]; matched = 1; }
            valid = matched && expected && ordinal == expected && entry[13] == checksum &&
                    !entry[12] && !entry[26] && !entry[27] && entry[0] == (ordinal == 2 ? 0x42 : 1);
            for (i = 0; valid && i < 13; ++i) {
                uint8_t at = pgm_read_byte(offsets + i), index = (ordinal - 1) * 13 + i;
                uint16_t c = (uint16_t)entry[at + 1] << 8 | entry[at];
                if (c >= 'a' && c <= 'z') c -= 32;
                if (c != (index < 14 ? pgm_read_byte(name + index) : (index == 14 ? 0 : 0xffff))) valid = 0;
            }
            matched = valid;
            expected = valid ? ordinal - 1 : 0;
        } else {
            if (matched && !expected && !(entry[11] & 0x18)) {
                uint8_t sum = 0, n = 0;
                char shortname[13];
                for (i = 0; i < 11; ++i) sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + entry[i];
                if (sum == checksum) {
                    for (i = 0; i < 8 && entry[i] != ' '; ++i) shortname[n++] = entry[i];
                    shortname[n++] = '.';
                    for (i = 8; i < 11 && entry[i] != ' '; ++i) shortname[n++] = entry[i];
                    shortname[n] = 0;
                    return pf_open(shortname) == FR_OK ? BJ_OK : BJ_SD;
                }
            }
            matched = expected = 0;
        }
        if (index % 16 == 15) {
            ++sector;
            if (cluster && ++in_cluster == fs.csize) {
                uint8_t fat[4];
                if (cluster < 2 || cluster >= fs.n_fatent ||
                    disk_readp(fat, fs.fatbase + cluster / 128, (cluster % 128) * 4, 4)) return BJ_SD;
                cluster = le32(fat) & 0x0fffffffUL;
                if (cluster >= 0x0ffffff8UL) break;
                if (cluster < 2 || cluster >= fs.n_fatent) return BJ_SD;
                sector = fs.database + (cluster - 2) * fs.csize;
                in_cluster = 0;
            }
        }
    }
    return BJ_NO_FILE;
}

static int16_t get_char(void) {
    uint8_t c;
    if (pos == used) {
        UINT count;
        if (pf_read(input, sizeof(input), &count) != FR_OK) { read_error = 1; return -1; }
        used = count; pos = 0;
        if (!used) return -1;
    }
    c = input[pos++];
    raw_crc = crc_byte(raw_crc, c);
    return c;
}
static int16_t hex_byte(void) {
    uint8_t i, value = 0;
    for (i = 0; i < 2; ++i) {
        int16_t c = get_char();
        if (c >= '0' && c <= '9') c -= '0';
        else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
        else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
        else return -1;
        value = (value << 4) | c;
    }
    return value;
}

/* Ascending records permit overlap checks without a 4 KiB bitmap. */
static uint8_t scan(uint8_t page) {
    uint32_t base = 0, end = 0xf000;
    uint8_t first = 1, eof = 0;
    int16_t c;
    if (pf_lseek(0) != FR_OK || fs.fptr != 0) return BJ_SD;
    memset(bj_page, 0xff, sizeof(bj_page));
    pos = used = read_error = 0; raw_crc = 0xffffffffUL;
    while ((c = get_char()) >= 0) {
        uint8_t header[4], sum = 0, i, count, type;
        uint16_t address, j;
        uint32_t start, extended = 0;
        if (c == '\r' || c == '\n') continue;
        if (eof || c != ':') return BJ_HEX;
        for (i = 0; i < 4; ++i) {
            c = hex_byte(); if (c < 0) return read_error ? BJ_SD : BJ_HEX;
            header[i] = c; sum += c;
        }
        count = header[0]; address = (uint16_t)header[1] << 8 | header[2]; type = header[3];
        start = base + address;
        if (start < base) return BJ_HEX;
        if (type == 0) {
            if (!count || start < end || start < 0xf000 || start > 0xffff ||
                start + count > 0x10000UL || (first && start != 0xf000)) return BJ_HEX;
            first = 0; end = start + count;
        } else if (!((type == 1 && !count && !address) ||
                     ((type == 2 || type == 4) && count == 2 && !address) ||
                     ((type == 3 || type == 5) && count == 4 && !address))) return BJ_HEX;
        for (j = 0; j < count; ++j) {
            c = hex_byte(); if (c < 0) return read_error ? BJ_SD : BJ_HEX;
            sum += c;
            if (type == 0 && ((start + j) >> 8) == (0xf0u + page)) bj_page[(uint8_t)(start + j)] = c;
            if (type == 2 || type == 4) extended = (extended << 8) | c;
        }
        c = hex_byte(); if (c < 0) return read_error ? BJ_SD : BJ_HEX;
        if ((uint8_t)(sum + c)) return BJ_HEX;
        if (type == 1) eof = 1;
        if (type == 2 || type == 4) base = extended << (type == 2 ? 4 : 16);
        c = get_char();
        if (c < 0) break;
        if (c != '\r' && c != '\n') return BJ_HEX;
    }
    if (read_error) return BJ_SD;
    if (!eof || first) return BJ_HEX;
    raw_crc = ~raw_crc;
    return BJ_OK;
}

static uint16_t helper(void) {
    static const uint8_t forms[3][18] PROGMEM = {
        {0xf8,0x94,0x87,0xbf,0xe8,0x95,0x78,0x94,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95},
        {0x2f,0xb7,0xf8,0x94,0x87,0xbf,0xe8,0x95,0x2f,0xbf,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95},
        {0x9f,0xb7,0xf8,0x94,0x87,0xbf,0xe8,0x95,0x9f,0xbf,0x07,0xb6,0x00,0xfc,0xfd,0xcf,0x08,0x95}
    };
    uint16_t at, found = 0;
    uint8_t f;
    for (f = 0; f < 3; ++f) {
        uint8_t len = f ? 18 : 16;
        for (at = 0; at + len <= 256; at += 2)
            if (!memcmp_P(bj_page + at, forms[f], len)) {
                if (found) return 0;
                found = (0xff00u + at) / 2;
            }
    }
    return found;
}

void bj_source_builtin(void) {
    bj_from_sd = 0;
    bj_target_crc = BJ_PAYLOAD_CRC;
    bj_native_word = BJ_NATIVE_BYTE / 2;
    bj_target_version = (uint16_t)pgm_read_byte(bj_payload + 6) << 8 | pgm_read_byte(bj_payload + 7);
}
uint8_t bj_source_sd(void) {
    uint8_t p, result;
    uint32_t crc = 0xffffffffUL;
    bj_from_sd = 0;
    if (pf_mount(&fs) != FR_OK) return BJ_SD;
    if (fs.fs_type != FS_FAT16 && fs.fs_type != FS_FAT32) return BJ_SD;
    result = open_hex(); if (result) return result;
    if (!fs.fsize || fs.fsize > 65536UL) return BJ_HEX;
    for (p = 0; p < 16; ++p) {
        uint16_t i;
        if (bj_source_progress) bj_source_progress(p);
        result = scan(p); if (result) return result;
        if (!p) {
            file_crc = raw_crc;
            if (memcmp_P(bj_page + 2, PSTR("\xb0\x07\x10\xad"), 4)) return BJ_HEX;
            for (i = 6; i < 8; ++i) if ((bj_page[i] >> 4) > 9 || (bj_page[i] & 15) > 9) return BJ_HEX;
            bj_target_version = (uint16_t)bj_page[6] << 8 | bj_page[7];
        } else if (raw_crc != file_crc) return BJ_CHANGED;
        page_crc[p] = page_checksum();
        for (i = 0; i < 256; ++i) crc = crc_byte(crc, bj_page[i]);
    }
    bj_native_word = helper();
    if (!bj_native_word) return BJ_HEX;
    bj_target_crc = ~crc; bj_from_sd = 1;
    if (bj_source_progress) bj_source_progress(16);
    return BJ_OK;
}
uint8_t bj_source_page(uint8_t page) {
    if (page >= 16) return BJ_PAYLOAD;
    if (bj_from_sd) {
        uint8_t result = scan(page);
        if (result) return result;
        if (raw_crc != file_crc || page_checksum() != page_crc[page]) return BJ_CHANGED;
    } else memcpy_P(bj_page, bj_payload + (uint16_t)page * 256, 256);
    return BJ_OK;
}
uint8_t bj_source_verify(void) {
    uint32_t crc = 0xffffffffUL;
    uint8_t p;
    for (p = 0; p < 16; ++p) {
        uint16_t i;
        if (bj_source_progress) bj_source_progress(p);
        uint8_t result = bj_source_page(p);
        if (result) return result;
        for (i = 0; i < 256; ++i) crc = crc_byte(crc, bj_page[i]);
    }
    if (bj_source_progress) bj_source_progress(16);
    return ~crc == bj_target_crc ? BJ_OK : BJ_PAYLOAD;
}
