/*
(The MIT License)

Copyright (c) 2013 Håkon Nessjøen <haakon.nessjoen@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>
#include "SDEmulator.h"

/* bootsector jump instruction */
unsigned char bootjmp[3] = { 0xeb, 0x3c, 0x90 };
unsigned char oem_name[8] = "uzemSDe";

static void long2shortfilename(char *dst, char *src) {
	int i;
	for (i = 0; i < 8; ++i) {
		if (*src == '.') {
			dst+=8-i;
			break;
		}
		*dst++ = toupper(*src++);
	}
	char *dot = strchr(src, '.') + 1;
	if (dot > src) {
		for (i = 0; i < 3; ++i) {
			if (*dot != 0) {
				*dst++ = toupper(*dot++);
			}
		}
	}
}

int SDEmu::init_with_directory(const char *path) {
	int i;
	struct stat st;

	memcpy(&bootsector.bootjmp, bootjmp, 3);
	memcpy(&bootsector.oem_name, oem_name, 8);
	bootsector.bytes_per_sector = 512;
	bootsector.sectors_per_cluster = 64;
	bootsector.reserved_sector_count = 1;
	bootsector.table_count = 2;
	bootsector.root_entry_count = 512;
	bootsector.total_sectors_16 = 0;
	bootsector.media_type = 0xF8;
	bootsector.sectors_per_fat = 1024; /* Update ? */
	bootsector.sectors_per_track = 32;
	bootsector.head_side_count = 32;
	bootsector.hidden_sector_count = 0;
	bootsector.total_sectors_32 = 4294967295; /* remember: file system id in mbr must be 06 */
	bootsector.drive_no = 4;
	bootsector.extended_fields = 0x29;
	bootsector.serial_number = 1234567;
	memcpy(&bootsector.volume_label, "UZEBOX     ", 11);
	memcpy(&bootsector.filesystem_type, "FAT16", 5);
	bootsector.signature[0] = 0x55;
	bootsector.signature[1] = 0xAA;

	DIR *dir = opendir(path);
	if (dir < 0) {
		return -1;
	}

	memcpy(toc[0].name,"UZEBOX  ",8);
	memcpy(toc[0].ext,"   ",3);
	toc[0].attrib = SDEFA_ARCHIVE | SDEFA_VOLUME_ID;

	i = 1;
	struct dirent *entry;
	int freecluster = 2;
	printf("SD Emulation of following files:\n");
	while ((entry = readdir(dir))) {
		if (entry->d_name && entry->d_name[0] != '.') {
			char *statpath = (char *)malloc(strlen(path) + strlen(entry->d_name) + 2);
			strcpy(statpath, path);
			strcat(statpath, "/");
			strcat(statpath, entry->d_name);
			stat(statpath, &st);
			paths[i] = statpath;

			memset(toc[i].name, 32, 11);
			long2shortfilename((char *)toc[i].name, (char *)entry->d_name);

			toc[i].attrib = SDEFA_ARCHIVE;
			toc[i].cluster_no = freecluster; /* TODO: Fill FAT */
			toc[i].filesize = st.st_size;
			printf("\t%d: %s\n", i, entry->d_name, st.st_size, toc[i].cluster_no);
			freecluster += ceil(st.st_size / (bootsector.sectors_per_cluster * 512.0f));
			if (++i == MAX_FILES) {
				break;
			}
		}
	}
	return 0;
}

int SDEmu::read(unsigned char *ptr, int len) {
	static int lastfile=-1;
	static int lastfileStart=0;
	static int lastfileEnd=0;
	static FILE *fp=0;
	int loop = 0;
	int posBootsector = 0;
	int posFatSector  = bootsector.bytes_per_sector + (bootsector.reserved_sector_count * bootsector.bytes_per_sector);
	int posRootDir    = posFatSector + (bootsector.table_count * (bootsector.sectors_per_fat * bootsector.bytes_per_sector));
	int posDataSector = posRootDir + (((bootsector.root_entry_count * 32) / bootsector.bytes_per_sector) * bootsector.bytes_per_sector);
	int pos;

	//printf("posFatSector: %u\nposRootDir: %u\nposDataSector: %u\n", posFatSector, posRootDir, posDataSector);

	for (loop = 0; loop < len; ++loop) {
			/* < 512 Bootsector */
			if (position < posFatSector)	{
				pos = position - bootsector.bytes_per_sector;
				unsigned char *boot = (unsigned char *)&bootsector;
				if (pos < sizeof(bootsector)) {
					*ptr++ = *(boot + (pos));
				} else {
					*ptr++ = 0;
				}
			} else
			/* Fat table */
			if (position < posRootDir) {
				pos = position - posFatSector;
				//printf("sdemu: reading fat: %d\n", pos);
				unsigned char *table = (unsigned char *)&clusters;
				*ptr++ = *(table + pos);
			}
			else if (position < posDataSector) {
				pos = position - posRootDir;
				unsigned char *table = (unsigned char *)&toc;
				*ptr++ = *(table + pos);
			} else {
				pos = position - posDataSector;
				if (lastfile == -1 || pos < lastfileStart || pos > lastfileEnd) {
					int i;
					lastfile = -1;
					for (i = 0; i < MAX_FILES; ++i) {
						int cluster = (pos/512/bootsector.sectors_per_cluster) + 2;
						if (toc[i].name[0] != 0 && cluster >= toc[i].cluster_no && cluster <= toc[i].cluster_no + (toc[i].filesize/512/bootsector.sectors_per_cluster)) {
							lastfile = i;
							lastfileStart = (toc[i].cluster_no-2)*512*bootsector.sectors_per_cluster;
							lastfileEnd = lastfileStart + toc[i].filesize;
							if (fp != NULL) {
								fclose(fp);
							}
							//printf("Opening file: %s\n", paths[i]);
							fp = fopen(paths[i], "r");
							break;
						}
					}
				}
				if (lastfile == -1 || fp <= 0) { *ptr++ = 0; }
				else {
					fseek(fp, pos - ((toc[lastfile].cluster_no-2)*512*bootsector.sectors_per_cluster), SEEK_SET);
					*ptr++ = fgetc(fp);
				}
			}
			position++;
	}
}

int SDEmu::seek(int pos) {
	position = pos;
}
