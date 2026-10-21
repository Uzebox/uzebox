#ifndef BOOTJACKER_H
#define BOOTJACKER_H
#include <stdint.h>

enum {
    BJ_OK = 0, BJ_DEVICE, BJ_FUSES, BJ_LOCKED, BJ_PAYLOAD,
    BJ_ALREADY, BJ_NO_GADGET, BJ_TIMING, BJ_VERIFY, BJ_V5_SITE,
    BJ_SD, BJ_NO_FILE, BJ_HEX, BJ_CHANGED
};
typedef struct {
    uint8_t signature[3], high_fuse, locks, command_register, delay;
    uint16_t gadget, stopped_pc, failed_page;
    uint32_t installed_crc;
    uint16_t api_version;
} bj_report_t;

extern bj_report_t bj_report;
uint8_t bj_preflight(void);
uint8_t bj_install(void);

/* Dispatch PCs are word addresses; flash-operation parameters are byte addresses. */
extern uint16_t bj_entry_word, bj_irq_word;
extern uint8_t bj_register, bj_ticks;
extern uint16_t bj_saved_sp;
void bj_invoke(uint16_t address, uint16_t data, uint8_t command);
void bj_native(uint16_t address, uint16_t data, uint8_t command);
void bj_probe_sts(void);
void bj_probe_out(void);
#endif
