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
#ifndef _SDEMULATOR_H_
#define _SDEMULATOR_H_

union SDEmu_date {
	uint16_t raw;
	struct {
		unsigned year:7  __attribute__ ((packed));
		unsigned month:4  __attribute__ ((packed));
		unsigned day:5  __attribute__ ((packed));
	} get;
};

union SDEmu_time {
	unsigned short raw;
	struct {
		unsigned ct_hour:5 __attribute__ ((packed));
		unsigned ct_minutes:6 __attribute__ ((packed));
		unsigned ct_seconds:5 __attribute__ ((packed));
	};
};


#define SDEFA_READ_ONLY 0x01
#define SDEFA_HIDDEN 0x02
#define SDEFA_SYSTEM 0x04
#define SDEFA_VOLUME_ID 0x08
#define SDEFA_DIRECTORY 0x10
#define SDEFA_ARCHIVE 0x20
#define SDEFA_LONGFILENAME 0x0F

typedef struct fat_BS
{
	unsigned char			bootjmp[3];
	unsigned char			oem_name[8];
	unsigned short	        bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		sectors_per_fat;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int		hidden_sector_count;
	unsigned int		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char   drive_no;
	unsigned char   ntreserved;
	unsigned char   extended_fields; /* 0x29 */
	unsigned int   serial_number;
	unsigned char   volume_label[11];
	unsigned char		filesystem_type[8]; /* FAT16 */
	unsigned char		extended_section[448];
	unsigned char   signature[2]; /* aa55 */
 
}__attribute__((packed)) fat_BS_t;

struct SDEmu_file {
	uint8_t name[8];
	uint8_t ext[3];
	uint8_t attrib;
	uint8_t nt;
	uint8_t creation_time_tenth;
	union SDEmu_time creation_time;
	union SDEmu_date creation_date;
	union SDEmu_date accessed_date;
	uint16_t zero;
	union SDEmu_time last_modification_time;
	union SDEmu_date last_modification_date;
  uint16_t cluster_no;
	uint32_t filesize;
} __attribute__((packed));

#define MAX_FILES 1024
struct SDEmu
{
	SDEmu() {
		position = 0;
		memset(&toc, 0, sizeof(toc));
		memset(&bootsector, 0, sizeof(bootsector));
	} 
	struct fat_BS bootsector;
	struct SDEmu_file toc[MAX_FILES];
	uint16_t clusters[1024*512];
	char *paths[MAX_FILES];
	int position;

	int init_with_directory(const char *path);
	int read(unsigned char *ptr, int len);
	int seek(int pos);
};

#endif
