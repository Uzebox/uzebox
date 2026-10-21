/*
 * ATmega644, two-byte PC only. Timer0 COMPA unwinds the interrupted boot code.
 * SPM's Z is a BYTE address; RET's synthesized destination is a WORD address.
 * No C ISR prologue/epilogue and no RETI: return with interrupts disabled.
 */
#include <avr/io.h>

.text
.global bj_invoke
.type bj_invoke, @function
bj_invoke:
    cli
    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r28
    push r29
    in r18, _SFR_IO_ADDR(SPL)
    sts bj_saved_sp, r18
    in r18, _SFR_IO_ADDR(SPH)
    sts bj_saved_sp+1, r18
    lds r18, bj_entry_word
    push r18
    lds r18, bj_entry_word+1
    push r18
    movw r30, r24
    movw r0, r22
    lds r26, bj_register
    ldi r27, 0
    st X, r20                     /* Register file is in data space. */
    ldi r18, (1 << CS00)
    ldi r19, 0
    out _SFR_IO_ADDR(TCCR0B), r19
    out _SFR_IO_ADDR(TCNT0), r19
    ldi r19, (1 << OCF0A)
    out _SFR_IO_ADDR(TIFR0), r19
    lds r19, bj_ticks
    out _SFR_IO_ADDR(OCR0A), r19
    out _SFR_IO_ADDR(TCCR0B), r18 /* Begin the calibrated instruction path. */
    sei
    ret                           /* Computed jump, preserving target Z. */

.global TIMER0_COMPA_vect
.type TIMER0_COMPA_vect, @function
TIMER0_COMPA_vect:
    pop r25                       /* AVR stacks low byte, then high byte. */
    pop r24
    sts bj_irq_word, r24
    sts bj_irq_word+1, r25
    rjmp bj_resume

bj_resume:
    cli
    ldi r18, 0
    out _SFR_IO_ADDR(TCCR0B), r18
    lds r18, bj_saved_sp+1
    out _SFR_IO_ADDR(SPH), r18
    lds r18, bj_saved_sp
    out _SFR_IO_ADDR(SPL), r18
    pop r29
    pop r28
    pop r17
    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2
    clr r1
    ret

/* Calibration runs in the application with command=0 (SPMEN clear).
 * Landing before/after SPM is harmless; the padding bounds late interrupts.
 */
.global bj_probe_sts
.type bj_probe_sts, @function
bj_probe_sts:
    sts _SFR_MEM_ADDR(SPMCSR), r5
    spm
    nop
    .rept 32
    nop
    .endr
    rjmp bj_probe_timeout
.global bj_probe_out
.type bj_probe_out, @function
bj_probe_out:
    out _SFR_IO_ADDR(SPMCSR), r5
    spm
    nop
    .rept 32
    nop
    .endr
bj_probe_timeout:
    ldi r24, 0
    sts bj_irq_word, r24
    sts bj_irq_word+1, r24
    rjmp bj_resume

/* Use the verified V5 helper with all peripheral IRQs masked. */
.global bj_native
.type bj_native, @function
bj_native:
    ldi r18, lo8(pm(bj_native_return))
    push r18
    ldi r18, hi8(pm(bj_native_return))
    push r18
    lds r18, bj_native_word
    push r18
    lds r18, bj_native_word+1
    push r18
    movw r30, r24
    movw r0, r22
    mov r24, r20
    ret
bj_native_return:
    cli
    clr r1
    ret
