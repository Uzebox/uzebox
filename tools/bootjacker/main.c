#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <uzebox.h>
#include "bootjacker.h"
#include "source.h"
#include "font.inc"

static void progress(uint8_t page) {
    SetFont(8, 1, '0' + page / 10 - 32);
    SetFont(9, 1, '0' + page % 10 - 32);
}

static void compact_begin(const char *title) {
    ClearVram();
    Print(2, 0, title);
    Print(2, 1, PSTR("PAGES 00/16"));
    Print(2, 2, PSTR("KEEP POWER ON"));
    WaitVsync(1);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        SetRenderingParameters(FIRST_RENDER_LINE + (FRAME_LINES - 24) / 2, 24);
    }
    bj_source_progress = progress;
}

static void compact_end(void) {
    bj_source_progress = 0;
    WaitVsync(1);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);
    }
    ClearVram();
}

static void status(uint8_t code) {
    const char *s;
    switch (code) {
    case BJ_OK: s = PSTR("CHECKS PASSED"); break;
    case BJ_DEVICE: s = PSTR("UNSUPPORTED CHIP: NEED 644/644A"); break;
    case BJ_FUSES: s = PSTR("NEED 4K BOOT/BOOTRST/WDT OFF"); break;
    case BJ_LOCKED: s = PSTR("BOOT LOCKED: ISP REQUIRED"); break;
    case BJ_PAYLOAD: s = PSTR("PAYLOAD CRC FAILED"); break;
    case BJ_ALREADY: s = PSTR("TARGET V5 ALREADY INSTALLED"); break;
    case BJ_NO_GADGET: s = PSTR("NO SUPPORTED LEGACY SPM SITE"); break;
    case BJ_V5_SITE: s = PSTR("V5 FOUND: LEGACY SITE REQUIRED"); break;
    case BJ_TIMING: s = PSTR("INTERRUPT TIMING CHECK FAILED"); break;
    case BJ_SD: s = PSTR("SD READ OR FILESYSTEM ERROR"); break;
    case BJ_NO_FILE: s = PSTR("BOOTLOADER.HEX NOT FOUND"); break;
    case BJ_HEX: s = PSTR("INVALID OR UNSUPPORTED HEX"); break;
    case BJ_CHANGED: s = PSTR("SD FILE CHANGED - STOPPED"); break;
    default: s = PSTR("FLASH VERIFY FAILED"); break;
    }
    Print(2, 15, s);
}

static void choose_source(void) {
    uint8_t selected = 0;
    for (;;) {
        uint16_t pad;
        uint8_t result;
        bj_source_builtin();
        ClearVram();
        Print(2, 2, PSTR("UZEBOX BOOTJACKER"));
        Print(2, 5, PSTR("SELECT BOOTLOADER SOURCE"));
        Print(4, 8, PSTR("BUILT-IN (DEFAULT)"));
        Print(4, 10, PSTR("SD: BOOTLOADER.HEX"));
        Print(2, 13, PSTR("UP/DOWN: SELECT   A: CONTINUE"));
        Print(2, 18, PSTR("SD OPTION NEEDS CARD WHILE WRITING"));
        Print(2, 20, PSTR("NO SPI RAM REQUIRED"));
        for (;;) {
            Print(2, 8, selected ? PSTR(" ") : PSTR(">"));
            Print(2, 10, selected ? PSTR(">") : PSTR(" "));
            while (ReadJoypad(0)) WaitVsync(1);
            do { WaitVsync(1); pad = ReadJoypad(0); } while (!pad);
            if (pad & (BTN_UP | BTN_DOWN)) selected ^= 1;
            else if (pad == BTN_A) break;
        }
        while (ReadJoypad(0)) WaitVsync(1);
        if (!selected) return;
        compact_begin(PSTR("VALIDATING BOOTLOADER.HEX"));
        result = bj_source_sd();
        compact_end();
        if (result == BJ_OK) return;
        status(result);
        Print(2, 20, PSTR("B: RETURN TO SOURCE SELECTION"));
        while (!(ReadJoypad(0) & BTN_B)) WaitVsync(1);
    }
}

int main(void) {
    uint8_t result;
    SetTileTable(font);
    SetFontTilesIndex(0);
    choose_source();
    compact_begin(PSTR("CHECKING SOURCE AND HARDWARE"));
    result = bj_preflight();
    compact_end();
    ClearVram();
    Print(2, 2, PSTR("UZEBOX BOOTJACKER"));
    Print(2, 4, PSTR("TARGET VERSION:"));
    PrintHexInt(18, 4, bj_target_version);
    Print(2, 5, bj_from_sd ? PSTR("SOURCE: SD BOOTLOADER.HEX") : PSTR("SOURCE: BUILT-IN"));
    Print(2, 7, PSTR("CHECKING HARDWARE AND SPM TIMING"));
    Print(2, 9, PSTR("SIGNATURE:"));
    PrintHexByte(14, 9, bj_report.signature[0]);
    PrintHexByte(17, 9, bj_report.signature[1]);
    PrintHexByte(20, 9, bj_report.signature[2]);
    Print(2, 10, PSTR("HIGH FUSE:     LOCKS:"));
    PrintHexByte(13, 10, bj_report.high_fuse);
    PrintHexByte(23, 10, bj_report.locks);
    Print(2, 11, PSTR("SPM SITE:       IRQ PC:"));
    PrintHexInt(12, 11, bj_report.gadget);
    PrintHexInt(25, 11, bj_report.stopped_pc);
    Print(2, 12, PSTR("TIMER:")); PrintHexByte(10, 12, bj_report.delay);
    if (bj_report.api_version) {
        Print(20, 12, PSTR("API:"));
        PrintHexInt(25, 12, bj_report.api_version);
    }
    Print(2, 13, PSTR("BOOT CRC:"));
    PrintHexInt(12, 13, bj_report.installed_crc >> 16);
    PrintHexInt(16, 13, bj_report.installed_crc);
    status(result);
    if (result == BJ_V5_SITE) {
        Print(2, 18, PSTR("INSTALLED LOADER IS ALREADY V5"));
        Print(2, 20, PSTR("THIS IS A LEGACY-TO-V5 UPDATER"));
        Print(2, 22, PSTR("USE ISP FOR THIS CUSTOM V5 BUILD"));
    }
    if (result != BJ_OK) for (;;) WaitVsync(1);
    Print(2, 17, PSTR("POWER LOSS MAY REQUIRE ISP REPAIR"));
    Print(2, 18, PSTR("VIDEO WILL STOP DURING FLASHING"));
    Print(2, 20, PSTR("RELEASE ALL BUTTONS, THEN HOLD"));
    Print(2, 21, PSTR("START + A FOR 3 SECONDS TO WRITE"));
    while (ReadJoypad(0)) WaitVsync(1);
    {
        uint16_t held = 0;
        while (held < 180) {
            WaitVsync(1);
            if (ReadJoypad(0) == (BTN_START | BTN_A)) ++held;
            else held = 0;
        }
    }
    compact_begin(PSTR("VALIDATING BEFORE WRITE"));
    result = bj_install();
    compact_end();
    if (result == BJ_OK || result == BJ_ALREADY) {
        Print(2, 8, PSTR("ALL 4096 BYTES VERIFIED"));
        Print(2, 10, PSTR("SWITCH POWER OFF"));
        Print(2, 12, PSTR("HOLD A WHILE SWITCHING POWER ON"));
        Print(2, 14, PSTR("TO ENTER THE V5 MENU"));
    } else {
        status(result);
        Print(2, 17, PSTR("PAGE:"));
        PrintHexInt(9, 17, bj_report.failed_page);
        Print(2, 19, PSTR("UPDATE INCOMPLETE - KEEP POWER ON"));
        Print(2, 21, PSTR("ISP RECOVERY MAY BE REQUIRED"));
    }
    for (;;) WaitVsync(1);
}
