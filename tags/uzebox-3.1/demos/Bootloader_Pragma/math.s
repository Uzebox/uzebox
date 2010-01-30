#include <avr/io.h>
#include "defines.h"

.global loadA32
.global loadB32
.global loadB16
.global storeA32
.global add32
.global debugA
.global debugB


.section .text
;
; load register 'A' from Y - r4:r5:r6:r7
;
loadA32:
    ldd r4,Y+0
    ldd r5,Y+1
    ldd r6,Y+2
    ldd r7,Y+3
    ret
    
;
; load register 'B' from Y - r8:r9:r10:r11
;
loadB32:   
    ; load Y
    ldd r8,Y+0
    ldd r9,Y+1
    ldd r10,Y+2
    ldd r11,Y+3
    ret

;
; load register 'B' from Y - r4:r5:r6:r7
;
loadB16:
    ldd r8,Y+0
    ldd r9,Y+1
    clr r10
    clr r11
    ret   
;
; save reigster 'A' to Y
;
storeA32:
    std Y+0,r4
    std Y+1,r5
    std Y+2,r6
    std Y+3,r7
    ret

;
; adds vars in A and B
;
add32:
    ; add with carry and return
    add r4,r8
    adc r5,r9
    adc r6,r10
    adc r7,r11
    ret
    
debugA:    
    ldi r16,'A'
    out PPRINT,r16
    ldi r16,':'
    out PPRINT,r16
    
    out HPRINT,r7
    out HPRINT,r6
    out HPRINT,r5
    out HPRINT,r4
    
    ldi r16,'\n'
    out PPRINT,r16
    ret
    
debugB:    
    ldi r16,'B'
    out PPRINT,r16
    ldi r16,':'
    out PPRINT,r16
    
    out HPRINT,r11
    out HPRINT,r10
    out HPRINT,r9
    out HPRINT,r8
    
    ldi r16,'\n'
    out PPRINT,r16
    ret