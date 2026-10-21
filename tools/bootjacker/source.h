#ifndef BJ_SOURCE_H
#define BJ_SOURCE_H
#include <stdint.h>

extern uint8_t bj_page[256], bj_from_sd;
extern uint16_t bj_target_version, bj_native_word;
extern uint32_t bj_target_crc;
extern void (*bj_source_progress)(uint8_t page);
void bj_source_builtin(void);
uint8_t bj_source_sd(void);
uint8_t bj_source_page(uint8_t page);
uint8_t bj_source_verify(void);
#endif
