# Disk images

The content of this folder contains disk images in standard 8" SSSD (single sided single density) CP/M v2.2 images. The specs of the disk images are the following: Tracks: 77, Sectors per track: 26, Sector lenght: 128, Blocksize: 1024, Max directory entries: 64, Skew: 6, Boot tracks: 2.

The provided file named DISK.CFG must be on the SD card in order to describes the mapping between drives numbers and images files. Up to four drives are supported by CP/M named A:, B:, C: and D:. By default the four images files are:
 * A:CPMDISK0.DSK  
 * B:CPMDISK1.DSK  
 * C:CPMDISK2.DSK  
 * D:MULTIPLN.DSK 

# Images provided

* ADVENT.DSK  : Microsoft's Adventure text based game.
* CPM22.DSK   : Original CP/M 2.2 boot disk with utilities.
* CPMDISK0.DSK: Boot disk with programs and utilities.
* CPMDISK1.DSK: Games Zork, Chess and Hitchhicker's guide to the galaxy.
* CPMDISK2.DSK: Microsoft Basic 5.21 and some basic programs.
* INFOCOM1.DSK: Infocom classic games disk 1.
* INFOCOM2.DSK: Infocom classic games disk 2.
* INFOCOM3.DSK: Infocom classic games disk 3.
* MULTIPLN.DSK: Microsoft Multiplan 1.06 VT100
* WM.DSK	  : Word-Master word processor.
* WSTAR33.SDK : WordStar 3.3 word processor.

# Editing disk images

The followings tool are able to read/write to and from those CP/M disk images.

## CPMFS

See: https://www.sydneysmith.com/wordpress/cpmfs/
(A copy is under tools/cpmfs)

Note that only SSSD images types are supported (-t LGSSSD).

Examples:
* Create a new disk image
	`cpmfs -t LGSSSD MYDISK.DSK init`
* List files on an image: 
	`cpmfs CPMDISK0.DSK dir`
* Extract a file from the image to the local drive: 
	`cpmfs CPMDISK0.DSK r TELNET.DAT`
* Write a file to an image: 
	`cpmfs CPMDISK0.DSK w TELNET.DAT`

## CPMTOOLS 

http://cpmarchives.classiccmp.org/cpm/mirrors/www.cpm8680.com/cpmtools/index.htm

* List files on an image: 
	`cpmls -f ibm-3740 cpmdisk2.dsk`

