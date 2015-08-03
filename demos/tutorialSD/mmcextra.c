/*
 * mmcextra.c
 *
 *  Created on: 17/12/2014
 *      Author: Cunning Fellow
 */

#ifndef MMCEXTRA_C_
#define MMCEXTRA_C_

#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "mmc.h"
#include "fat.h"


long findFileFirstSector(char *fileName){
	mmc_init_no_buffer();
	mmc_cuesector(0x000);					// Get the first sector of the SD card.  This is where the MBR is

	mmcSkipBytes(offsetof(MBR, partition1)+ offsetof(PartitionEntry, startSector));			// Skip the execCode and a few other bytes

	long bootRecordSector = mmcGetLong();   // Read the sector that the boot record starts at

	mmc_stoptransmission();					// stop reading the MBR
	mmc_cuesector(bootRecordSector);		// and start reading the boot record

	mmcSkipBytes(offsetof(BootRecord, bytesPerSector));

	int  bytesPerSector    = mmcGetInt();
	char sectorsPerCluster = mmcGetChar();
	int  reservedSectors   = mmcGetInt();
	mmcSkipBytes(1);
	int  maxRootDirectoryEntries = mmcGetInt();
	mmcSkipBytes(3);
	int sectorsPerFat = mmcGetInt();

	long dirTableSector = bootRecordSector + reservedSectors + (sectorsPerFat * 2);

	mmc_stoptransmission();
	mmc_cuesector(dirTableSector);

	uint8_t fileFound = 1;

	do {
		if(fileFound == 0) {
			mmcSkipBytes(21);
			fileFound = 1;
		}

		for(uint8_t i = 0; i<11; i++){
			if(mmc_get_byte() != fileName[i]) fileFound = 0;
		}

	} while (fileFound == 0);

	mmcSkipBytes(15);

	int firstCluster = mmcGetInt();

	mmc_stoptransmission();

	return(dirTableSector+((maxRootDirectoryEntries * 32)/bytesPerSector)+((firstCluster-2)*sectorsPerCluster));
}

#endif /* MMCEXTRA_C_ */
