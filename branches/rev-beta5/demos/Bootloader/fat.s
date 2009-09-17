#include <avr/io.h>
#include "defines.h"

.global fat_init
.global fat_next_dir_entry

.global current_dir_sector
.global sectors_per_cluster
.global cur_dir_lba
.global current_dir_entry
.global data_start_lba
.global fat_table_lba
.global max_root_dir_entries

;gcc uses little endian
.section .bss
	sector_buffer_ptr:		.word 1
	fat_table_lba:			.space 4
	cur_dir_lba:			.space 4
	data_start_lba:			.space 4
	sectors_per_cluster:	.byte 1
	current_dir_sector:		.word 1
	current_dir_entry:		.byte 1 ;entry within the current dir sector
	max_root_dir_entries:	.word 1

.section .text

;
; Initialize the FAT API
;------------------------
; C callable
; r25:r24 = Pointer to 512 bytes buffer
; returns = void
.section .text.fat_init
fat_init:
	push r2
	push r3
	push r4
	push r5

	sts sector_buffer_ptr+0,r24
	sts sector_buffer_ptr+1,r25
	movw r2,r24	;save buffer pointer for later

	call mmc_init
	cpse r24,r1 ;0?
	rjmp fat_init_exit

	;read MBR (sector 0)
	clr r25
	clr r24
	clr r23
	clr r22
	call mmc_readsector		
	;cpi r24,0
	;brne fat_init_exit


	movw ZL,r2
	subi ZL,lo8(-446)
	sbci ZH,hi8(-446)

	;validate we have a FAT16 partition
	ldd r24,Z+mbr_partition1_partitionType
	cpi r24,FAT_PARTITION_TYPE_FAT16
	breq fat_init_fat16
	ldi r24,FAT_ERR_INVALID_PARTITION_TYPE
	ret
fat_init_fat16:

	;load boot sector lba
	ldd r22,Z+mbr_partition1_startSector+0
	ldd r23,Z+mbr_partition1_startSector+1
	ldd r24,Z+mbr_partition1_startSector+2
	ldd r25,Z+mbr_partition1_startSector+3
	movw r4,r22
	call mmc_readsector	


	movw ZL,r2
	
	;store useful values
	ldd r24,Z+boot_maxRootDirectoryEntries
	sts max_root_dir_entries,r24
	ldd r24,Z+boot_maxRootDirectoryEntries+1
	sts max_root_dir_entries+1,r24
	ldd r24,Z+boot_sectorsPerCluster
	sts sectors_per_cluster,r24

	; r25:r24:r23:r22 = LBA sector (32 bit)

	;compute fat table 1st sector (assume it's <64k)
	;bootRecordSector + reservedSectors
	ldd r24,Z+boot_reservedSectors
	clr r25
	add r4,r24
	adc r5,r25
	sts fat_table_lba+0,r4 
	sts fat_table_lba+1,r5 
	sts fat_table_lba+2,r25
	sts fat_table_lba+3,r25
	

	;compute root dir sector
	;bootRecordSector + reservedSectors + (sectorsPerFat * 2); 
	ldd r22,Z+boot_sectorsPerFat+0
	ldd r23,Z+boot_sectorsPerFat+1

	clr r24
	clc
	rol r22
	rol r23
	rol r24
	add r22,r4
	adc r23,r5
	adc r24,r25
	sts cur_dir_lba+0,r22 
	sts cur_dir_lba+1,r23 
	sts cur_dir_lba+2,r24 
	sts cur_dir_lba+3,r25 
	movw r2,r22
	movw r4,r24
	
	;load root directory first sector
	sts current_dir_entry,r25
	sts current_dir_sector+0,r25
	sts current_dir_sector+1,r25
	call mmc_readsector	
	


	;compute data start sector
	;=dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)
	;=dirTableSector+(maxRootDirectoryEntries * 1/16)
	;=dirTableSector+(maxRootDirectoryEntries >> 4)

	lds r24,max_root_dir_entries
	lds r25,max_root_dir_entries+1
	
	;shift word by 4
	swap r24
	andi r24,0x0f	
	swap r25
	mov r23,r25	
	andi r23,0xf0
	or r24,r23
	andi r25,0x0f
	
	add r2,r24
	adc r3,r25
	adc r4,r1

	sts data_start_lba+0,r2
	sts data_start_lba+1,r3
	sts data_start_lba+2,r4
	sts data_start_lba+3,r5
	
	ldi r24,FS_OK

fat_init_exit:
	pop r5
	pop r4
	pop r3
	pop r2

	ret


;r25:r24 = pointer to a fat_filter function
;typedef bool (*fat_filter)(DirectoryTableEntry* dirEntry);

.section .text.fat_next_dir_entry
fat_next_dir_entry:
	push r16
	push r17

	movw ZL,r24
	lds r22,current_dir_entry
	ldi r23,32
	mul r22,r23
	lds r24,sector_buffer_ptr+1
	lds r25,sector_buffer_ptr+0
	add r24,r0
	adc r25,r1

	clr r1
	movw r16,r24
	icall
	cpi r24,0
	brne fat_next_dir_entry_accepted
	clr r16	;NULL
	clr r17	
fat_next_dir_entry_accepted:
	movw r24,r16

	pop r17
	pop r16
	ret

