#include <avr/io.h>
#include "defines.h"

#define PARTITION1_SECTOR_OFS 454
#define BOOT_STRUCT_SIZE 62

.global fatinit
.global filterdir
.global getroot
.global getfiles
.global getclusterstartsector
.global getnextcluster

.global uzeboxDirTableCluster
.global menuStartDirEntry
.global sectorsPerCluster

.section .bss
    tmpSector:                      .space 4
    fatSector:                      .space 4
    rootDirTableSector:             .space 4
    uzeboxDirTableCluster:          .space 2
    sectorsPerCluster:              .space 1
        
.section .text
    root_name:           .asciz "UZEBOX"
    binary_extension:    .ascii "uze"
    
;
; fatinit() - init routine for FAT16 support
; uses... just about all registers
;
fatinit:
    ; read in the MBR - use A to set the desired sector
    clr r4
    clr r5
    clr r6
    clr r7
    call mmcreadsector
    tst r1
    breq fatinit_mbr
    rjmp fatfail
fatinit_mbr:
    
    ; find the mbr sector LBA in the MBR for the first partition
    ldi YL,lo8(mbr_partition1_startSector)
    ldi YH,hi8(mbr_partition1_startSector)
    call loadA32 ; load into B for later
    call mmcreadsector
    tst r1
    breq fatinit_bootsector
    rjmp fatfail
fatinit_bootsector:
    
    ;;;;  fatSector = bootRecordSector + reservedSectors;
    ; bootRecordSector is still loaded from earlier (B)
    
    ; load A with reserved sectors
    ldi YL,lo8(boot_reservedSectors)
    ldi YH,hi8(boot_reservedSectors)
    call loadB16
    
    ; add into A and store back into fatSector var
    call add32
    ldi YL,lo8(fatSector)
    ldi YH,hi8(fatSector)
    call storeA32
    
	;;;; rootDirTableSector = fatSector + (sectorsPerFat * 2);
    ; load sectors per fat into B and promote to 32 bitss
    ldi YL,lo8(boot_sectorsPerFat)
    ldi YH,hi8(boot_sectorsPerFat)  
    call loadB16
    
    ; add sectoersPerFat to fatSector twice (emulating a mul by two)
    call add32
    call add32
    
    ldi YL,lo8(rootDirTableSector)
    ldi YH,hi8(rootDirTableSector)
    call storeA32
    
    ; save sectors per cluster
    ldi YL,lo8(boot_sectorsPerCluster)
    ldi YH,hi8(boot_sectorsPerCluster) 
    ld r16,Y
    ldi YL,lo8(sectorsPerCluster)
    ldi YH,hi8(sectorsPerCluster) 
    st Y,r16
	    
    ; find the root directory ("/UZEBOX")
    clr r20
    clr r21
    ldi ZL,pm_lo8(getroot)
    ldi ZH,pm_hi8(getroot)
    call filterdir
        
    rjmp fatpass
    
;
; getclusterstartsector(cluster r20:r21)
;
; uses: r0,r1,A,B,X
; returns: start sector in A
getclusterstartsector:
    ; load rootDirTableSector into A
    ldi YL,lo8(rootDirTableSector)
    ldi YH,hi8(rootDirTableSector)
    call loadA32
    
    ; test sector number for zero - skip calculation if so
    clr r0
    cp r20,r0
    cpc r21,r0
    breq getclusterstartsector_done
    
    ;;;; return rootDirTableSector + ((cluster-1) * sectorsPerCluster);
    ; decrement cluster number
    subi r20,1
    sbci r21,0
    
    ; load sectors per cluster
    ldi YL,lo8(sectorsPerCluster)
    ldi YH,hi8(sectorsPerCluster)
    ld r2,Y
        
    ; multiply and store in B
    mul r20,r2  ; low byte
       ;;; movw r8,r0
        mov r8,r0
        mov r9,r1
        clr r10
        clr r11
    mul r21,r2 ; high byte
        add r9,r0
        adc r10,r1
        adc r11,r11 ; r7 = 0 + carry
    call debugB
    
    ; add B and A
    call add32
getclusterstartsector_done:
    ret

;
; getnextcluster(cluster r20:r21)
;
; uses: everything
; returns: next cluster in r20:r21
; TODO: optimize for current cluster
getnextcluster:
    ;;;;unsigned long sector = fatSector+((cluster*2)/bytesPerSector);
    ; NOTE: assuming 512 bytes per sector - so msb of cluster is sector offset
    ;;;;unsigned long sector = fatSector+(cluster/256);
    
    ; promote MSB of cluster to B
    mov r8,r21
    clr r9
    clr r10
    clr r11
    
    ; load A with fatSector and add
    ldi YL,lo8(fatSector)
    ldi YH,hi8(fatSector)
    call loadA32
    call add32
    
    ;;;;if(mmc_readsector(sector, bufptr)==-1) return 0;
    ; store the sector (A) in temporary  var and read the sector
    ldi YL,lo8(tmpSector)
    ldi YH,hi8(tmpSector)
    call storeA32
    call mmcreadsector
        
    ;;;;return ((int*)bufptr)[cluster%(bytesPerSector/2)];
    ; NOTE: assuming 512 bytes per sector, so lsb*2 is the buffer offset for the cluster index
    ; return ((int*)bufptr)buffer[(cluster&0x00ff)*2];
    
    ; multiply lsb by two
    clr r9
    clc
    rol r8
    rol r9
    
    ; add offset to buffer pointer
    ldi YL,lo8(buffer)
    ldi YH,hi8(buffer)    
    add YL,r8
    adc YH,r9
    
    ; load and return
    ldd r20,Y+0
    ldd r21,Y+1
    
    ret
    
;
; filterdir
; r20:21 - cluster number to iterate as a directory
;
; uses:
; TODO - set this up to span multiple clusters
filterdir:    
    ; set up current cluster number and get the start sector
    call getclusterstartsector ; r20:r21 already has the sector, so just call it
    call debugA
    rjmp filterdir_loop_start
    
filterdir_loop:
    ; increment A
    clr r0
    ldi r16,1
    add r4,r16
    adc r5,r0
    adc r6,r0
    adc r7,r0
filterdir_loop_start:
    call mmcreadsector
    ldi r16,DIR_ENTRIES_PER_SECTOR
    ldi YL,lo8(buffer)
    ldi YH,hi8(buffer) ; Y tracks the current entry
filterdir_entry_loop:
    ; test the file attrs to see if we need to look deeper
    ldd r17,Y+dir_fileAttrs
    cpi r17,(FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME|FAT_ATTR_DEVICE)
    breq filterdir_entry_next    ; invalid flags combination
    
    ; test the filename first-byte to test the validity of this entry
    ld r17,Y
    cpi r17,0
    breq filterdir_done          ; end of directory
    cpi r17,0xe5
    breq filterdir_entry_next    ; deleted entry
    cpi r17,0x2e
    breq filterdir_entry_next    ; LFN entry

    ; call the filter routine
    push YL
    push YH
    push ZL
    push ZH
    icall
    pop ZH
    pop ZL
    pop YH
    pop YL
    tst r1
    brne filterdir_done
    
filterdir_entry_next:
    ; increment offset and loop counter
    adiw YL,DIR_ENTRY_SIZE
    dec r16
    brne filterdir_entry_loop
    rjmp filterdir_loop
filterdir_done:
    ret
    
;
; getroot - filter for filterdir()
; Y - points to start of entry
;
getroot:
    ; set return condition
    clr r1
    
    ; check the file attrs
    ldd r17,Y+dir_fileAttrs
    andi r17,FAT_ATTR_DIRECTORY
    breq getroot_fail
    
    ; check file name
    ldi ZL,lo8(root_name)
    ldi ZH,hi8(root_name)
    ldi r19,0
getroot_name_loop:
    ; test max length
    cpi r19,8
    breq getroot_name_done
    inc r19
        
    ; load values
    lpm r18,Z+
    ld r17,Y+
        
    ; test for zero termination
    cpi r18,0
    brne getroot_name_compare
    cpi r17,0x20  ; space terminates the filename in FAT16
    breq getroot_name_done
    rjmp getroot_fail
    
getroot_name_compare:    
    ; test for equality
    cp r17,r18
    breq getroot_name_loop
    rjmp getroot_fail
getroot_name_done:
    ; reset Y
    sub YL,r19
    sbci YH,0
    
    ; grab the cluster for this entry and save it
    ldd r20,Y+dir_first_cluster
    ldd r21,Y+dir_first_cluster+1
    ldi YL,lo8(uzeboxDirTableCluster)
    ldi YH,hi8(uzeboxDirTableCluster)
    std Y+0,r20
    std Y+1,r21
            
    inc r1 ; set to halt filtering
    ret
getroot_fail:
    ret
    
    
fatpass:
    clr r1
    ret
    
fatfail:
    clr r1
    com r1
    ret
