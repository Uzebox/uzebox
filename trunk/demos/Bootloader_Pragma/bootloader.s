
#include <avr/io.h>
#include "defines.h"

.section .bss
 
.section .text

.global main

main:
    call ClearVram

   
; r24=X pos (8 bit)
; r22=Y pos (8 bit)
; r20=Font tile No (8 bit) 
; r18=Font background color 
    call mmcinit
    ;TODO: test for error condition where MMC init fails
    
    ; prep setfont
    ldi r24,0x01  ;x
    ldi r22,0x01  ;y
    ldi r18,0x00  ;bg
    ldi r20,'0'  ;char
    
    ; set font char accordingly and show output
    ldi r16,0xff
    cp r1,r16
    brne init_passed
    ldi r20,'1'
init_passed:
    //call SetFont
    
    call fatinit
    ;TODO: test for error condition for no file system
    ;TODO: test for error condition where /uzebox dir doesn't exist
    
    ; prime menu subsystem and get the first page of entries    
    call menuinit
    call getmenudata
    call rendermenu

    
main_loop:	
    ; wait for screen sync
    ldi r24,6
    call WaitVSync
    
    ; paint the cursor
    ldi r20,0
    ldi r21,5
    call menucursor
    
    ;mimic a selection
    ;call menushowinfo
    
    rjmp main_loop
    

    
