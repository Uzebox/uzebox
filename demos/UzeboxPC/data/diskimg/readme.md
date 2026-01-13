# Disk images

UzeboxPC uses CP/M v2.2 8" SSSD (single sided single density) disk images with the following specifications:

* Tracks: 77
* Sectors per track: 26
* Sector lenght: 128
* Blocksize: 1024
* Max directory entries: 64
* Skew: 6
* Boot tracks: 2 

`cpmtools` refers to this disk by using `ibm-3740` for its diskdef value but you don't usually need to specify the disk model when using `cpmtools` apart from when you are creating (formatting) a new disk image.

`DISK.CFG` must be in the root directory of your SD card in to define the mapping between drive letters and disk image files. Up to four drives are currently supported by UzeboxPC's CP/M - A:, B:, C: and D:.

The default disk configuration for UzeboxPC is:

* A:CPMDISK0.DSK
* B:CPMDISK1.DSK
* C:CPMDISK2.DSK
* D:MULTIPLN.DSK 

# Images provided

* ADVENT.DSK  : Microsoft's Adventure text based game.
* CPMDISK0.DSK: Boot disk with programs and utilities.
* CPMDISK1.DSK: Games Zork, Chess and Hitchhicker's guide to the galaxy.
* CPMDISK2.DSK: Microsoft Basic 5.21 and some basic programs.
* INFOCOM1.DSK: Infocom classic games disk 1.
* INFOCOM2.DSK: Infocom classic games disk 2.
* INFOCOM3.DSK: Infocom classic games disk 3.
* MULTIPLN.DSK: Microsoft Multiplan 1.06 VT100. An example spreadsheet is provided. Start with: MP BUDGET.MP

# Modifying CP/M disk images

The following tools are able to read from and write to CP/M disk image files:

## CPMFS

Windows builds of CPMFS can be [downloaded here.](https://www.sydneysmith.com/wordpress/cpmfs/)

Note that only SSSD images types are supported (-t LGSSSD).

Examples:

* Create a new disk image:
	`cpmfs -t LGSSSD MYDISK.DSK init`
	
* List files on an image: 
	`cpmfs CPMDISK0.DSK dir`
	
* Extract a file from the image to the local drive: 
	`cpmfs CPMDISK0.DSK r TELNET.DAT`
	
* Write a file to an image: 
	`cpmfs CPMDISK0.DSK w TELNET.DAT`

## CPMTOOLS

You can install cpmtools under Debian and Ubuntu based Linux distributions by running:

	sudo apt install cpmtools

Windows builds of cpmtools can be [downloaded here.](http://cpmarchives.classiccmp.org/cpm/mirrors/www.cpm8680.com/cpmtools/index.htm)

Examples:

* Create a new disk image:

	`mkfs.cpm -f ibm-3740 MYDISK.DSK`

* List files in the disk image CPMDISK0.DSK:

	`cpmls CPMDISK0.DSK`

* Copy the file SUBMIT.COM into CP/M user area 0 of disk image CPMDISK0.DSK as SUBMIT.COM:

	`cpmcp CPMDISK0.DSK SUBMIT.COM 0:SUBMIT.COM`

* Extract file TELNET.DAT from CPMDISK0.DSK:

    `cpmcp CPMDISK0.DSK 0:TELNET.DAT TELNET.DAT`

* Delete the file called ED.COM from CPMDISK0.DSK:

	`cpmrm CPMDISK0.DSK 0:ed.com`
