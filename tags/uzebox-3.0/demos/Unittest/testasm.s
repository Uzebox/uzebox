.global WritePGM
.global ReadPGM

.text
;****************************
; Write byte to PGM
; extern void WritePGM(int addr,u16 value)
; r25:r24 - addr
; r23:r22 - value to write
;****************************

WritePGM:
    movw r0,r22
    movw r30,r24
    spm
    ret
    
;****************************
; Read byte from EEPROM
; extern unsigned char ReadPGM(int addr)
; r25:r24 - addr
; r25:24 - value read
;****************************
ReadPGM:
    movw r30,r24
    lpm r24,Z+
    lpm r25,Z
    ret