#include <avr/io.h>
#include "defines.h"

.global menuinit
.global menuHasMoreEntries
.global menuTotalEntries
.global renderMenu
.global getmenudata
.global rendermenu
.global menucursor
.global menushowinfo
.global menuClusterBuffer

.section .bss
    menuStartDirEntry:              .space 2
    menuTotalEntries:               .space 1
    menuClusterBuffer:              .space MAX_MENU_ENTRIES*6  ; each entry = [cluster:2][size:4]
    menuCurrentItem:                .space 1

.section .text

    menuTitle:  .ascii "  >>>    UZEBOX BOOTLOADER MENU    <<<  "   ; NOTE: must be screen tiles wide

menuinit: 
    ;TODO: consolidate or shove into BSS
    clr r0
    ; clear out the start entry
    ldi YL,lo8(menuStartDirEntry)
    ldi YH,hi8(menuStartDirEntry)
    st Y+,r0
    st Y+,r0
    
    ; clear out total entries
    ldi YL,lo8(menuStartDirEntry)
    ldi YH,hi8(menuStartDirEntry)
    st Y+,r0
    st Y+,r0
    
    ; paint title
    ldi XL,lo8(vram)
    ldi XH,hi8(vram)
    ldi ZL,lo8(menuTitle)
    ldi ZH,hi8(menuTitle)
    ldi r17,MENU_TITLE_BG_COLOR
    ldi r16,VRAM_TILES_H
menuinit_paint_title:
    lpm r18,Z+
    subi r18,32 ; normalize
    st X+,r18
    st X+,r17
    dec r16
    brne menuinit_paint_title
   
    ; paint info area background
    ldi XL,lo8(vram+INFO_START)
    ldi XH,hi8(vram+INFO_START)
    ldi YL,lo8(vram+VRAM_SIZE)
    ldi YH,hi8(vram+VRAM_SIZE)
    ldi r17,INFO_AREA_BG_COLOR
menuinit_paint_info:
    adiw XL,1  ; skip text
    st X+,r17    
    cp XL,YL
    cpc XH,YH
    brne menuinit_paint_info
    
    ret

;
; getfiles - filter for filterdir()
; Y - points to start of entry
; r12:r13 - start entry to begin gathering data
; r14:r15 - filter count - number of times filter has been run
;
; USES:
; r25 - current menu offset - will be one more than max entries if there is more to be parsed later
;
getfiles:    
    ; check the file attrs
    ldd r17,Y+dir_fileAttrs
    andi r17,FAT_ATTR_DIRECTORY
    brne getfiles_continue
    
    ; check the extension
    ldd r18,Y+dir_extension+0
    cpi r18,'U'
    brne getfiles_continue
    ldd r18,Y+dir_extension+1
    cpi r18,'Z'
    brne getfiles_continue
    ldd r18,Y+dir_extension+2
    cpi r18,'E'
    brne getfiles_continue
    
getfiles_positive_match:
    
    ; test if this is where the menu starts
    cp r14,r12
    cpc r15,r13
    brlo getfiles_increment
    
    ; prep for work with total entries count
    ldi XL,lo8(menuTotalEntries)
    ldi XH,hi8(menuTotalEntries)
    ld r25,X
            
    ; see if we have too many entries, record that, and end
    cpi r25,MAX_MENU_ENTRIES
    brlo getfiles_menu_add
    inc r25
    st X,r25
    ; finish up and halt the filter
    clr r1
    inc r1
    ret
    
getfiles_menu_add:
    
    ; figure out where to write into the buffer
    ldi ZL,lo8(menuClusterBuffer)
    ldi ZH,hi8(menuClusterBuffer)
    ldi r18,6 ; entry size
    mul r18,r25
    add ZL,r0
    adc ZH,r1
    
    ; write the data
    ldd r18,Y+dir_first_cluster
    st Z+,r18
        
    ldd r18,Y+dir_first_cluster+1
    st Z+,r18
        
    ldd  r18,Y+dir_file_size
    st Z+,r18
    ldd  r18,Y+dir_file_size+1
    st Z+,r18
    ldd  r18,Y+dir_file_size+2
    st Z+,r18
    ldd  r18,Y+dir_file_size+3
    st Z+,r18
                    
    ; increment the entry count and save it
    inc r25
    st X,r25
getfiles_increment:
    ; increment the filter counter
    ldi r17,1
    add r14,r17
    clr r17
    adc r15,r17    
getfiles_continue:       
    ; return to filter some more
    clr r1
    ret
    
;
; getmenudata() - loads up the look-aside data with entries suitable for display
;
getmenudata:
    ; set directory cluster number arg
    ldi YL,lo8(uzeboxDirTableCluster)
    ldi YH,hi8(uzeboxDirTableCluster)
    ldd r20,Y+0
    ldd r21,Y+1
        
    ; tell the filter where to start - start dir goes into B
    ldi YL,lo8(menuStartDirEntry)
    ldi YH,hi8(menuStartDirEntry)
    ldd r12,Y+0
    ldd r13,Y+1
            
    ; set our entry counter
    clr r14
    clr r15
    
    ; start off with zero entries loaded
    clr r3
    
    ; call the filter
    ldi ZL,pm_lo8(getfiles)
    ldi ZH,pm_hi8(getfiles)
    call filterdir
            
    ret
    
;
; rendermenu() builds the menu display based on the cluster cache built via getfiles()
;
; NOTE: implementation is largely independent of other operations.  Call getmenudata()
; to populate the file cache first, then execute this as many times as needed.
;
rendermenu:
    ; load vars
    ldi YL,lo8(menuTotalEntries)
    ldi YH,hi8(menuTotalEntries)
    ld r15,Y
    clr r14 ; counter
        
    ; set up our pointers
    ldi ZL,lo8(menuClusterBuffer)
    ldi ZH,hi8(menuClusterBuffer)
    
rendermenu_loop:
    ; load the cluster into A, and load the corresponding sector
    ; -- this is the uze file header sector
    ld r20,Z+
    ld r21,Z+
    call getclusterstartsector
    call debugA
    call mmcreadsector
    
    out HPRINT,r14
        
    ; set the write position
    ldi XL,lo8(vram+(VRAM_TILES_H*TOP_MARGIN*2))
    ldi XH,hi8(vram+(VRAM_TILES_H*TOP_MARGIN*2))
	ldi r16,(VRAM_TILES_H*2)
    mul r14,r16
    add XL,r0
    adc XH,r1
    adiw XL,(LEFT_MARGIN*2)
    
    ; load the name
    ldi YL,lo8(buffer+uze_name)
    ldi YH,hi8(buffer+uze_name)
    
    ; feed the header text into the vram display
    ldi r16,32 ; max length of name
rendermenu_name_loop:
    ld r17,Y+
    cpi r17,0
    breq rendermenu_name_done
    
    ; normalize to uppercase
    cpi r17,'a'
    brlo rendermenu_name_normal
    cpi r17,'z'+1
    brge rendermenu_name_normal
    subi r17,('a'-'A')
rendermenu_name_normal:
    subi r17,32 ; offset by non-printable chars
    
    ;TODO: normalize case and other chars
    st X+,r17
    adiw XL,1 ; advance to skip background byte
    
    dec r16
    brne rendermenu_name_loop

rendermenu_name_done:
    ;iterate Z
    ldi r16,4
    add ZL,r16
    clr r16
    adc ZL,r16
    
    ;iterate menu item
    ldi r16,MAX_MENU_ENTRIES
    inc r14
    cp  r14,r16
    breq rendermenu_done  ; special case - count can be one more than the displayable amount
    cp  r14,r15
    brne rendermenu_loop  ; normal case, havent' displayed everything yet
rendermenu_done: 
    ret
    
;
; menucursor(entrynumber:r20,background:r21) - paints the cursor line in the menu selection area
;    
; USES: r0,r1,r16,X
;
menucursor:
    ; figure out the destination
    ldi XL,lo8(vram+(VRAM_TILES_H*TOP_MARGIN*2))
    ldi XH,hi8(vram+(VRAM_TILES_H*TOP_MARGIN*2))
	ldi r16,(VRAM_TILES_H*2)
    mul r20,r16
    add XL,r0
    adc XH,r1
    
    ; paint the cursor
    ldi r16,VRAM_TILES_H
menucursor_loop:
    adiw XL,1 ; skip text
    st X+,r21
    dec r16
    brne menucursor_loop

    ret;
    
;
; menushowinfo() - paints the info section of the screen, based on menuCurrentItem
;    
; USES: damn near everything
;
menushowinfo:
    ldi XL,lo8(menuCurrentItem)
    ldi XH,hi8(menuCurrentItem)
    ld r16,X
    
    ; figure out where to read from the buffer
    ldi ZL,lo8(menuClusterBuffer)
    ldi ZH,hi8(menuClusterBuffer)
    ldi r18,6 ; entry size
    mul r18,r16
    add ZL,r0
    adc ZH,r1
    
    ; get the cluster number and populate the I/O buffer for this file
    ld r20,Z+
    ld r21,Z+
    call getclusterstartsector
    call mmcreadsector

    ; TODO
    ldi YL,lo8(buffer+uze_name)
    ldi YH,hi8(buffer+uze_name)
    ldi XL,lo8(vram+INFO_START)
    ldi XH,hi8(vram+INFO_START)
    
menushowinfo_nameloop:
    
    

    ret
