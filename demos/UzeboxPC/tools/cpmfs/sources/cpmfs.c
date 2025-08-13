/*
  cpmfs.c
  Copyright 2017-2018 Greg Sydney-Smith

  Redistribution and use in binary forms, without
  modification, are permitted provided that the following condition is met:

  1. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  DEVELOPERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Change Log:
  2018-11-07. gss. Uses DPB byte if present.
  2018-11-06. gss. Add rmlbl and rmdpb commands.
  2018-11-06. gss. Removed proprietary code. Open source again.
  2018-11-05. gss. Add dpb command.
  2018-11-01. gss. Revised disk types. Now CDOS standard names + some extras.
  2018-10-17. gss. Add dsk_opts.
  2018-08-19. gss. old 8DSSD now 8DSSD1 as <256 x 1K blocks is a poor default
                   new 8DSSD = 2K blocks, 128Dir1 (CDOS INIT271 defaults)
                   Add 2MB disk type (128Tk x 128SPT x 128BYTES, SS, 2K 256Dir2)
  2018-07-07. gss. Add 8DSDD11 for CDOS0258. V0.06
  2018-07-04. gss. Add lbl command. Fix w command for filesize%blksiz==0. V0.05
  2017-12-13. gss. Earlier versions were open source. This one is not.
    It contains proprietary functions that are not being released.
  2017-11-09. gss. More disk fmts. Tidy up. Ver 0.03.
  2017-10-31. gss. Add 5" and C5Q disk formats. Add dir2 & used cmds. Add dbg.
  2017-10-30. gss. Initial version.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libgss.h"



#define VER_MAJOR       2
#define VER_MINOR       0
#define VER_DEV        ""
#define VER_COPYR      "Copyright 2017-2018 Greg Sydney-Smith"

#define TRUE            (1==1)
#define FALSE           (!TRUE)

#define min(a,b)        ((a)<(b)?(a):(b))
#define toupper(c)      (('a'<=c && c<='z')? c+'A'-'a' : c)
#define ROUNDUP(a,b)    (((a)+(b)-1)/(b))

typedef unsigned char byte;

typedef struct {
    int   tracks;
    int   spt0;
    int   secsiz0;
    int   spt1;
    int   secsiz1;
    //
    int   rsvd;		// reserved tracks
    int   blksiz;	// block size (1024, 2048, etc)
    int   dirblks;	// directory blocks
    int   dirtype;	// 1=16 blocks per FCB, 2=8 blocks per FCB, 3=CDOS 8+8x0
    int   skew;
    char *name;
//
    char *fn;
    // calculated on open
    FILE *fp;
    int fcbs;           // number of FCBs in the directory
    int blocks;         // size of DatArea in blocks
    int bnoSize;        // size of a block number, 1 or 2 bytes
    int fcbMapLen;      // number of entries in the FCB map
    byte exm;
    char *dir;          // an in memory copy of the directory
    int  *stbl;		// skew table

    }  DSKFMT;

#define FCBSIZE         0x20
#define FCBMAP          0x10     // offset to map in FCB


int dbg = 0;    // debug: show more detail

// tracks,spt0,secsiz0,spt1,secsiz1,rsvd,blksiz,dirblk,dirtype,skew,name
DSKFMT fmts[] = {
    { 77, 26,128, 26,128,2,1024,2,1, 6,"LGSSSD" },	// 8" CPM/CDOS
    {154, 26,128, 26,128,2,2048,2,3, 6,"LGDSSD" },	// (CDOS 2K -dt 3)
    { 77, 26,128, 16,512,2,2048,2,2,11,"LGSSDD" },
    {154, 26,128, 16,512,2,2048,4,2,11,"LGDSDD" },
    { 40, 18,128, 18,128,3,1024,2,1, 5,"SMSSSD" },	// 5" CPM/CDOS
    { 80, 18,128, 18,128,3,1024,2,1, 5,"SMDSSD" },
    { 40, 18,128, 10,512,2,1024,2,1, 4,"SMSSDD" },
    { 80, 18,128, 10,512,2,2048,2,3, 4,"SMDSDD" },	// (CDOS 2K -dt 3)
    { 77, 26,128, 26,128,0, 512,0,4, 6,"CLSSSD" },	// 8" Cromix
    {154, 26,128, 26,128,0, 512,0,4, 6,"CLDSSD" },
    { 77, 26,128, 16,512,0, 512,0,4,11,"CLSSDD" },
    {154, 26,128, 16,512,0, 512,0,4,11,"CLDSDD" },
    { 40, 18,128, 18,128,0, 512,0,4, 5,"CSSSSD" },	// 5" Cromix
    { 80, 18,128, 18,128,0, 512,0,4, 5,"CSDSSD" },
    { 40, 18,128, 10,512,0, 512,0,4, 4,"CSSSDD" },
    { 80, 18,128, 10,512,0, 512,0,4, 4,"CSDSDD" },
    { 77, 26,128, 26,128,2,1024,2,1, 6,"8_SSSD" },	// 8" CPM (same)
    {154, 26,128, 26,128,2,2048,2,2, 6,"8_DSSD" },	// (DR 2K -dt 2)
    { 77, 26,128, 16,512,2,2048,2,2,11,"8_SSDD" },
    {154, 26,128, 16,512,2,2048,4,2,11,"8_DSDD" },
    { 40, 18,128, 18,128,3,1024,2,1, 5,"5_SSSD" },	// 5" CPM (same)
    { 80, 18,128, 18,128,3,1024,2,1, 5,"5_DSSD" },
    { 40, 18,128, 10,512,2,1024,2,1, 4,"5_SSDD" },
    { 80, 18,128, 10,512,2,2048,2,2, 4,"5_DSDD" },	// (DR 2K -dt 2)
    { 77, 16,512, 16,512,2,2048,2,2,11,"8FSSDD" },	// 8" full DD
    {154, 16,512, 16,512,2,2048,2,2,11,"8FDSDD" },
    { 40, 10,512, 10,512,2,2048,2,2, 4,"5FSSDD" },	// 5" full DD
    { 80, 10,512, 10,512,2,2048,2,2, 4,"5FDSDD" },
    {255,128,128,128,128,0,2048,4,2, 0,"UM4"    }	// Udo Monk 4MB DataDisk
};


///

int sayhex(int n, char *s) {
    int i, j;
    char *hex = "0123456789ABCDEF";

    for (i=0; i<n; i+=16) {
        for (j=0; j<16 && i+j<n; j++)
            printf("%c%c ",hex[(s[i+j] & 0xF0)>>4],hex[s[i+j] & 0x0F]);
        for (; j<16; j++) printf("   ");
        for (j=0; j<16 && i+j<n; j++)
            printf("%c",(' '<=s[i+j] && s[i+j]<='~')? s[i+j] : '.');
        printf("\n");
    }
    return 0;
}


char *basename(const char *s) {
    char *pLastSlash = strrchr(s, '/');
    if (! pLastSlash) pLastSlash = strrchr(s, '\\');	// try the other one too
    return (char *)(pLastSlash ? pLastSlash + 1 : s);
}

int hasDPB(byte *buf) {
    return (buf[126]==0xE5 && 0x10 <= buf[127] && buf[127] <= 0x4F);
}

////// Disk Settings //////

static int _calcDskSize(DSKFMT *d) {		// bytes
    int size;

    size= (d->spt0 * d->secsiz0) + (d->tracks - 1) * (d->spt1 * d->secsiz1);
    return size;
}

static int _calcSysAreaSize(DSKFMT *d) {	// bytes
    int size;

    if (d->rsvd == 0) return 0;

    size =                 (d->spt0 * d->secsiz0);
    size+= (d->rsvd - 1) * (d->spt1 * d->secsiz1);
    return size;
}

static int _calcDatAreaSize(DSKFMT *d) {	// bytes
    return _calcDskSize(d) - _calcSysAreaSize(d);
}

static char *_calcDescription(DSKFMT *d) {
    static char buf[80];
    char tmp1[30], tmp2[20], tmp3[20];

    sprintf(tmp1,"%3dTk %3dsec%3d skew%02d %dK",
        d->tracks,d->spt1,d->secsiz1,d->skew,d->blksiz/1024);
    sprintf(tmp2," %3dDir%d",d->dirblks*d->blksiz/32,(d->dirtype==2)?2:1);
    sprintf(tmp3," (Tk0 %3dsec%3d)",d->spt0,d->secsiz0);
    sprintf(buf,"%s%s%s",
        tmp1,
        (d->dirtype==4)?" Cromix ":tmp2,
        (d->spt0==d->spt1 && d->secsiz0==d->secsiz1)?"":tmp3);
    return buf;
}

static int *_calcSkewTbl(int spt, int skew) {
    int *stbl, n, i, j;

    // create skew table
    stbl= (int *)malloc(spt * sizeof(int));
    if (skew<1) skew=1;
    for (n=1, i=0; i < spt; i++) {
        stbl[i]= n;
        n += skew;
        if (n > spt) {
            while (1) {
                n = n % spt;		// wrap back to start
                // check if already have this number
                for (j=0; j<=i && (n != stbl[j]); j++) ;
                // if not, all's well
                if (j>i) break;
                // otherwise, try a different sector number
                n++;
            }
        }
    }
    return stbl;
}

DSKFMT *chooseDiskTypeFromName(char *name) {
    int n, i;

    n= sizeof(fmts) / sizeof(DSKFMT);
    for (i=0; i<n; i++)
        if (stricmp(name,fmts[i].name) == 0)
            return fmts+i;
    printf("Error: unknown disk type\n");
    return NULL;
}

DSKFMT *chooseDiskTypeFromBootSector(char *fn) {
    long sz;
    FILE *fp;
    int n, i, j;
    byte buf[128], lbl[8];
    DSKFMT *d;

    // also check disk size
    // in case of boot sector copied from a different disk type
    sz=FileSize(fn);

    if ((fp=fopen(fn,"rb"))==NULL)
        { printf("Error: can't find %s\n",fn); return NULL; }
    if (fread(buf,128,1,fp) != 1)
        { printf("Error: can't read %s\n",fn); return NULL; }
    fclose(fp);

    n= sizeof(fmts) / sizeof(DSKFMT);
    for (i=0; i<n; i++) {
        for (j=0; j<8; j++) lbl[j]=0xE5;
        strncpy(lbl,fmts[i].name,6);
        if (strncmp(buf+120,lbl,6) == 0)
            if (sz == _calcDskSize(fmts+i))
                return fmts+i;
    }
    return NULL;
}

DSKFMT *chooseDiskTypeFromSize(char *fn) {
    long sz;
    int n, i;

    if ((sz=FileSize(fn)) < 0)
        { printf("Error: can't find %s\n",fn); return NULL; }

    n= sizeof(fmts) / sizeof(DSKFMT);
    for (i=0; i<n; i++)
        if (sz == _calcDskSize(fmts+i))
            return fmts+i;
    printf("Error: unrecognised disk size\n");
    return NULL;
}

////// BIOS //////

static int _sayTSA(char *pre, int track, int sector, long addr, char *post) {
    printf("%s Track %3d, Sector %3d (address %06lX)%s\n",
        pre,track,sector,addr,post);
    return 1;
}



// translate a logical sector number to a physical one
// as done in CP/M BIOS with skew table
// in: sector is 0...
// out: sector is 1...
// Assumes spt0==spt1 (like CP/M does) OR track>0 (no mixed fmt DataDisks)
int biosSecTran(DSKFMT *d, int sector) {
    int n1, phys, frac;

    // 10 DD sectors appear to CP/M as 40 x 128 byte "sectors"
    // so we have to scale down, and up again later, by "n"
    n1  = d->secsiz1 / 128;

    phys= sector / n1;		// real sector number
    frac= sector % n1;		// offset into real sector
    phys= d->stbl[phys];	// do the skew. 0... -> 1...

    // convert back to 0-based before scaling back up
    phys--;
    // scale back up, and add back the offset
    phys *= n1;
    phys += frac;
    // and back to 1 based again (sectors are numbered 1,2,3,...)
    phys++;

    if (dbg&1) printf("dskSecTran: sector %3d -> %3d\n",sector,phys);
    return phys;
}

// CP/M2: sectors are always 128 bytes (as far as it knows)
int biosRead(DSKFMT *d, int track, int sector, char *buf) {
    long addr;
    int n0, n1, len;

    n0= d->secsiz0/128;
    n1= d->secsiz1/128;
    if (track==0) {
        if (sector < 1 || sector > n0 * d->spt0)
            return _sayTSA("Error:",track,sector,0L," - track 0 invalid sector number");
        addr  = (sector-1) * 128;
    } else {
        if (sector < 1 || sector > n1 * d->spt1)
            return _sayTSA("Error:",track,sector,0L," - invalid sector number");
        addr  = d->spt0 * d->secsiz0;
        addr += (track -1) * d->spt1 * d->secsiz1;
        addr += (sector-1) * 128;
    }
    len = 128;

    if (dbg&1)
        _sayTSA("dskRead2 ",track,sector,addr,"");

    if (fseek(d->fp,addr,SEEK_SET) != 0)
        return _sayTSA("Error:",track,sector,addr," - seek failed");
    if (fread(buf,len,1,d->fp) != 1)
        return _sayTSA("Error:",track,sector,addr," - read failed");
//sayhex(len,buf);
    return 0;
}

// CP/M2 treats all sectors as 128 bytes long
int biosWrite(DSKFMT *d, int track, int sector, char *buf) {
    long addr;
    int n0, n1, len;

    n0= d->secsiz0/128;
    n1= d->secsiz1/128;
    if (track==0) {
        if (sector < 1 || sector > n0 * d->spt0)
            return _sayTSA("Error:",track,sector,0L," - track 0 invalid sector number");
        addr  = (sector-1) * 128;
    } else {
        if (sector < 1 || sector > n1 * d->spt1)
            return _sayTSA("Error:",track,sector,0L," - invalid sector number");
        addr  = d->spt0 * d->secsiz0;
        addr += (track -1) * d->spt1 * d->secsiz1;
        addr += (sector-1) * 128;
    }
    len = 128;

    if (dbg&1)
        _sayTSA("dskWrite2",track,sector,addr,"");

    if (fseek(d->fp,addr,SEEK_SET) != 0)
        return _sayTSA("Error:",track,sector,addr," - seek failed");
    if (fwrite(buf,len,1,d->fp) != 1)
        return _sayTSA("Error:",track,sector,addr," - write failed");
//sayhex(len,buf);
    return 0;
}

////// BDOS //////

int bdosRdDatRec(DSKFMT *d, int rec, char *buf) {
    int n, rpt, track, sector, phys;

    n     = d->secsiz1 / 128;		// records per sector
    rpt   = d->spt1 * n;		// records per track
    track = rec / rpt;
    sector= rec % rpt;			// cp/m thinks all sectors are 128 bytes
    phys  = biosSecTran(d,sector);	// after skew

    track+= d->rsvd;			// to data area of disk

    if (track >= d->tracks)
        { printf("Error: Seek past end\n"); return 1; }

    if (dbg&2)
        printf("Read  record %5d (track %2d, sector %2d (%2d))\n",
            rec,track,1+sector,phys);

    if (biosRead(d,track,phys,buf) != 0) return 1;

    return 0;
}

int bdosWrDatRec(DSKFMT *d, int rec, char *buf) {
    int n, rpt, track, sector, phys;

    n     = d->secsiz1 / 128;		// records per sector
    rpt   = d->spt1 * n;		// records per track
    track = rec / rpt;
    sector= rec % rpt;			// cp/m thinks all sectors are 128 bytes
    phys  = biosSecTran(d,sector);	// after skew

    track+= d->rsvd;			// to data area of disk

    if (track >= d->tracks)
        { printf("Error: Seek past end\n"); return 1; }

    if (dbg&2)
        printf("Read  record %5d (track %2d, sector %2d (%2d))\n",
            rec,track,1+sector,phys);

    if (biosWrite(d,track,phys,buf) != 0) return 1;

    return 0;
}

int bdosRdBlock(DSKFMT *d, int bno, char *buf) {
    int rec, i;

    if (dbg&2) printf("Read  block %04X\n",bno);

    rec= bno * d->blksiz / 128;
    for (i=0; i<d->blksiz; i+=128, rec++)
        if (bdosRdDatRec(d,rec,buf+i) != 0)
            return 1;
    return 0;
}

int bdosWrBlock(DSKFMT *d, int bno, char *buf) {
    int rec, i;

    if (dbg&2) printf("Write block %04X\n",bno);

    rec= bno * d->blksiz / 128;
    for (i=0; i<d->blksiz; i+=128, rec++)
        if (bdosWrDatRec(d,rec,buf+i) != 0)
            return 1;
    return 0;
}

int bdosRdDir(DSKFMT *d) {
    int bytes, i;

    bytes= d->blksiz;
    for (i=0; i<d->dirblks; i++) // for each directory block
        if (bdosRdBlock(d,i,d->dir+(i*bytes)) != 0)
            return 1;
    return 0;
}

int bdosWrDir(DSKFMT *d) {
    int bytes, i;

    bytes= d->blksiz;
    for (i=0; i<d->dirblks; i++) // for each directory block
        if (bdosWrBlock(d,i,d->dir+(i*bytes)) != 0)
            return 1;
    return 0;
}

int DskOpen(DSKFMT *d, char *imgfn, char *mode) {

    if (d->dirtype==4)
        { printf("Error: cpmfs can't access Cromix files\n"); return 1; }
    if (d->rsvd==0 && ((d->spt0 != d->spt1) || (d->secsiz0 != d->secsiz1)))
        { printf("Error: can't use mixed format data disks\n"); return 1; }
    if (!FileExists(imgfn))
        { printf("Error: can't find %s\n",imgfn); return 1; }
    if ((d->fp=fopen(imgfn,"r+b"))==NULL)
        { printf("Error: can't open %s for r/w\n",imgfn); return 1; }

    // create skew table
    d->stbl= _calcSkewTbl(d->spt1, d->skew);

    // calc some things for later
    d->fcbs  = d->dirblks * d->blksiz / FCBSIZE; // number of dir entries
    d->blocks= _calcDatAreaSize(d) / d->blksiz;  // blocks in the DatArea
    switch (d->dirtype) {
    case  1: d->bnoSize= 1;		// 1 byte block numbers
             d->fcbMapLen=16;		// num of entries in FCB map (for CP/M)
             d->exm= (d->blksiz==1024)? 0 :
                     (d->blksiz==2048)? 1 :
                     (d->blksiz==4096)? 3 : -1;
             break;
    case  2: d->bnoSize= 2;		// 2 byte block numbers
             d->fcbMapLen=8;		// FCB map entries (CP/M, 2byte)
             d->exm= (d->blksiz==2048)? 0 :
                     (d->blksiz==4096)? 1 : -1;
             break;
    case  3: d->bnoSize= 1;		// CDOS 1 byte block numbers, 2K blocks
             d->fcbMapLen=8;		// num of entries in FCB map (16K max)
             d->exm= 0;			// CDOS2 doesn't use extent mask idea
             if (d->blksiz!=2048)
                 { printf("Error: use 2K blocks with CDOS dirtype 3\n"); return 1; }
             break;
    default: printf("Error: invalid dirtype\n");
             return 1;
    }
    if (d->exm < 0)
        { printf("Error: invalid dirtype, blocksize combination\n"); return 1; }

    // read the entire directory in
    if ((d->dir = malloc(d->dirblks * d->blksiz)) == NULL)
        { printf("Error: can't allocate directory memory\n"); return 1; }
    if (bdosRdDir(d)!=0) return 1;

    return 0;
}

int DskClose(DSKFMT *d) {

    // OS will clean up anyway, but for good measure ...
    if (d->fp  != NULL) fclose(d->fp);
    if (d->dir != NULL) free(d->dir);
    return 0;
}

//// DIR / FILE based routines ////

// adjust star
// '*' -> remain len x '?'
// eg "fred*   txt",8 -> "fred????txt". "fred*3  txt",8 -> "fred????txt"
static void adjStar(char *patn, int len) {
    int i, star;

    for (star=0, i=0; i<len; i++) {
        if (patn[i]=='*') star=1;
        if (star) patn[i]='?';
    }
}

// "This call formats only the filename portion of the FCB." - CDOS2 Manual
int cpmFormatNameToFCB(char *fn, byte *fcb) { // convert win fn to CP/M FCB format
    int i, j;

    // get rid of a path if present
    fn= basename(fn);

    // drive : ...
    fcb[0]= 0;
    if (strlen(fn)>=2 && fn[1]==':') {
        fcb[0]= toupper(fn[0])-'@';
        if (fcb[0] > 1) fcb[0]=0;		// Only A: supported here
        fn+=2;
    }

    // file name and type
    for (i=0; i<11; i++) fcb[1+i]=' ';		// fill with spaces
    for (i=0; i<8 && fn[i] && fn[i]!='.'; i++)	// copy 8 chars
        fcb[1+i]= toupper(fn[i  ]);
    while (fn[i] && fn[i]!='.') i++;		// find '.' if present
    if (fn[i]=='.') i++;			// move past it
    for (j=0; j<3 && fn[i+j]; j++)		// copy 3 chars
        fcb[9+j]= toupper(fn[i+j]);

    // * -> ???etc
    adjStar(fcb+1,8);
    adjStar(fcb+9,3);
    return 0;
}

static int get1(char *p) {
    return p[0]&0xFF;
}
static int get2(char *p) {
    return (p[0]&0xFF) + (p[1]&0xFF)*256;
}

// Assumes: 0 <= x < d->fcbMapLen
int setFCBmap(DSKFMT *d, char *fcb, int x, int val) {

    if (d->bnoSize==1) {   // 1 byte bno
        fcb[0x10+x]=val;
        return 0;
    }

    // 2 byte bno
    fcb[0x10+2*x  ]=  val    &0xFF;
    fcb[0x10+2*x+1]= (val>>8)&0xFF;
    return 0;
}

// set record count (uses low bit(s) of ex byte if blksiz*maplen > 16K)
static int setFCBrc(DSKFMT *d, byte *fcb, int nrecs) {
    int hi, lo;

    if (d->dirtype==3) {	// CDOS 2K 1byte block numbers
        fcb[15]= nrecs;		// MapLen is 8 so 0<=nrecs<=0x80, so ok
    } else {			// CP/M
        hi= nrecs/128;
        lo= nrecs%128;
        if (hi>0 && lo==0) { hi--; lo=0x80; }
        fcb[12]= (fcb[12] & (~d->exm)) + hi;
        fcb[15]= lo;
    }
    return 0;
}
int getBlocksInFCB(DSKFMT *d, char *fcb, int blocks[]) {
    int extent, i, recs, nblocks, bno;
    char *p;

    if (get1(fcb+0) == 0xE5) return 0;          // deleted. none here

    // number of fcb blocks in-use
    extent = get1(fcb+12);
    recs   = (extent & d->exm) * 128 + get1(fcb+15);
    nblocks= ROUNDUP(recs,(d->blksiz/128));        // div and then round up

    // look through allocation map
    for (p=fcb+FCBMAP, i=0; i<nblocks; i++, p+=d->bnoSize) {
        bno= (d->bnoSize==1) ? get1(p) : get2(p);
        blocks[i]= bno;
    }
//printf("fcb=%-11.11s, ex=%02x rc=%02x, recs=%3x nblocks=%2d\n",fcb+1,fcb[12],fcb[15],recs,nblocks);
    return i;
}

// builds a list of blocks in a given filename, in order,
// regardless of the order of extents
int *getBlocksInFile(DSKFMT *d, char *cpmfn) {
    int i, j, extent, fcbnum, *list, nblocks, blocks[16];
    char *fcb, FnFCB[1+8+3+1];

    // 1. set all blocks to 0, initially
    list= (int *)malloc(d->blocks * sizeof(int));
    for (i=0; i < d->blocks; i++) list[i]=0;

    // 2. filename to FCB style
    cpmFormatNameToFCB(cpmfn,FnFCB); FnFCB[12]='\0';

    // 3. search through directory to find file blocks
    for (i=0; i<d->fcbs; i++) {
        fcb = d->dir + (i*FCBSIZE);
//sayhex(16,fcb);
        if ((get1(fcb)==0x00) && strncmp(fcb+1,FnFCB+1,11)==0) {
            // not deleted, filename matches

            // with exm and blocksize > 1K, extent field has 2 parts
            // LHS is sequence that we are used to
            // RHS is sort of extra bits for record count field
            extent= get1(fcb+12);
            fcbnum= extent / (d->blksiz/1024);	// 0,1,2,...

            // if fcbMapLen is 8, there are 8 blocks in each map
            // if this is fcbnum 2, we have blocks 2*8 .. 2*8+7 of the file
            if ((fcbnum+1)*d->fcbMapLen > d->blocks) // bigger than disk size
                { printf("Error: file too big\n"); return NULL; }

            nblocks= getBlocksInFCB(d,fcb,blocks);
            for (j=0; j<nblocks; j++) {
                list[fcbnum*d->fcbMapLen + j]= blocks[j];
            }
        }
    }
    return list;
}

int * getBlocksInUse(DSKFMT *d) {
    int i, j, *list, bno, nblocks, blocks[16];
    char *fcb;

    // 1. set all blocks to 0, initially
    list= (int *)malloc(d->blocks * sizeof(int));
    for (i=0; i<d->blocks; i++) list[i]=0;

    // 2. set directory blocks in use
    for (i=0; i<d->dirblks; i++) list[i]=1;

    // 3. search through directory to find other blocks in use
    for (i=0; i<d->fcbs; i++) {
        fcb = d->dir + (i*FCBSIZE);
        nblocks= getBlocksInFCB(d,fcb,blocks);
        for (j=0; j<nblocks; j++) {
            bno= blocks[j];
            if (bno < d->blocks) // ignore if outside range. work anyway.
                list[bno] += 1;
        }
    }

    return list;
}

int getCpmFileSize(DSKFMT *d, char *cpmfn) {
    int i, n, recs, extent, rc;
    char *fcb, FnFCB[1+8+3+1];

    // 1. filename to FCB style
    cpmFormatNameToFCB(cpmfn,FnFCB); FnFCB[12]='\0';

    // 2. search through directory to find file extents and sizes
    recs= -1;
    for (i=0; i<d->fcbs; i++) {
        fcb = d->dir + (i*FCBSIZE);
        if ((get1(fcb+0)==0x00) && strncmp(fcb+1,FnFCB+1,11)==0) { // match, not deleted
//sayhex(16,fcb);
            extent= get1(fcb+12); // extent number
            rc    = get1(fcb+15); // record count in the extent. eg 80H for full 16K 
            // given we're this many extents in, and have this many records here
            // we think the filesize is ...
            n= extent * 0x80 + rc; // extent size should always be 16K (80H recs)
            if (n>recs) recs=n;
        }
    }
    return recs;
}

// is this an ambiguous file name?
int isAFN(char *s) {
    for (; *s; s++)
        if (*s=='?' || *s=='*')
            return TRUE;
    return FALSE;
}

int deleteCpmFile(DSKFMT *d, char *cpmfn) { // common bit used by ERA and W
    int err, i, j;
    char FnFCB[1+8+3+1], *fcb;

    // 1. filename to FCB style
    cpmFormatNameToFCB(cpmfn,FnFCB); FnFCB[12]='\0';

    // 2. for each entry, if it matches, mark as deleted
    err=1;
    for (i=0; i<d->fcbs; i++) {
        fcb = d->dir + (i*FCBSIZE);
        if (get1(fcb+0) != 0x00) continue; // not User 0 or already deleted
        for (j=1; j<12 && (fcb[j]==FnFCB[j] || FnFCB[j]=='?'); j++) ;
        if (j==12) { // matches
            fcb[0]= 0xE5;
            err= 0;
        }
    }
    return err; // 0 if no error, 1 if not found
}

char *createExtent(DSKFMT *d, char *cpmfn, int eno) {
    int i, n;
    char FnFCB[1+8+3+1], *fcb;

    // 1. find a free slot
    for (i=0, fcb=d->dir; i<d->fcbs; fcb+=FCBSIZE, i++)
        if (get1(fcb+0) == 0xE5)
            break;
    if (i>=d->fcbs) { printf("Error: directory full\n"); return NULL; }

    // 2. fill in the FCB
    // cpm: eno gets shifted left m bits if blocksize=2^m x base
    // dirtype1 base=1K, dirtype2 base=2K. n=2^m.
    // cdos: eno is sequential (0,1,2,...) - dont shift left. n=1.
    n= (d->dirtype==2)? (d->blksiz / 2048) :
       (d->dirtype==1)? (d->blksiz / 1024) : 1;
    for (i=0; i<FCBSIZE; i++) fcb[i]=0;	// no blocks alloc'd, no recs, no flags ...
    cpmFormatNameToFCB(cpmfn,fcb);	// add file name and extension
    fcb[ 0]= 0;				// ignore any drv: prefix
    fcb[12]= eno * n;			// add extent number (might be 0)

    return fcb;
}

int getUnusedBlock(DSKFMT *d, int *blocks) { // get a free block
    int i;

    for (i=0; i<d->blocks; i++) // dir blocks should already be marked as in use
        if (blocks[i]==0) {
            blocks[i]=1;   // mark as now allocated
            return i;
        }
    printf("Error: no free space\n");
    return -1;
}

static int _dirIdx= -1;


// files bigger than 512K use s2 byte as a "module" number
// every 32 extents requires a new module (sort of an extent of extents)
// sequential access seems limited to 128 records (0-127 in nr byte)
// but, in ram, ex is really an extension of nr. So, s2 becomes part of the
// overall nr number too.
int cpmFindNext(DSKFMT *d, byte *fcb1, byte *fcb2) {		// CP/M 12H
    int i, j;
    byte *fcb;

    for (i=_dirIdx+1; i<d->fcbs; i++) {
        fcb= d->dir + (i*FCBSIZE);

//sayhex(16,fcb);
        // check not deleted (nor CDOS label) and user area 0
        if (fcb1[0]!='?' && fcb[0]!=0x00) continue;

        // check file name and type matches
        for (j=1; j<12; j++)
            if (((fcb[j]&0x7f) != (fcb1[j]&0x7f)) && (fcb1[j]!='?'))
                break;

        // check extent matches
        if (j==12 && (fcb[12]&(~(d->exm)))==(fcb1[12]&(~(d->exm)))) j++;
        if (j==12 && fcb1[12]=='?') j++;

        // ignore s1
        // also ignoring s2. Not valid for files >512KB (ex should be 0-31). 

        if (j==13) break;		// found
    }
    _dirIdx=i;

    if (i >= d->fcbs) return -1;	// not found

    // In CP/M a copy of the directory sector is in the DMA buffer
    // and the return value (0..3) is an index into there.
    // For us: we'll pass in a buf addresss, put the FCB at the start, and
    // always return an index value of 0 (at the start).
    memcpy(fcb2,d->dir + i*FCBSIZE,FCBSIZE);
    return 0;
}

int cpmFindFirst(DSKFMT *d, byte *fcb1, byte *fcb2) {		// CP/M 11H
    _dirIdx=-1;
    return cpmFindNext(d,fcb1,fcb2);
}

char **cpmDir(DSKFMT *d, char *cpmfn) {
    int i, n;
    char *fcb, FnFCB[FCBSIZE], fn[8+1], ext[3+1], fnft[8+1+3+1], buf[128];
    char **out;

    // adjust patn
    cpmFormatNameToFCB(cpmfn,FnFCB); FnFCB[12]=0; FnFCB[14]=0;	// extent 0
//sayhex(16,FnFCB);

    // 1. count matching entries
    for (n=0, i=cpmFindFirst(d,FnFCB,buf); i>=0; i=cpmFindNext(d,FnFCB,buf)) n++;

    // 2. make the list
    out= (char **)malloc((n+1)*sizeof(char *));

    // 3. fill it in
    for (n=0, i=cpmFindFirst(d,FnFCB,buf); i>=0; i=cpmFindNext(d,FnFCB,buf)) {
        fcb= buf + (i*FCBSIZE);
//printf("i=%d\n",i); sayhex(16,fcb);
        strncpy(fn ,fcb+1,8); fn[ 8]='\0'; trim(fn);
        strncpy(ext,fcb+9,3); ext[3]='\0'; trim(ext);
        sprintf(fnft,"%s.%s",fn,ext);
        strcpy(out[n]= malloc(strlen(fnft)+1),fnft);
        n++;
    }
//printf("%d files in list\n",n);
    out[n]= NULL;
    return out;
}

static int cpmCp2win(DSKFMT *d, char *cpmfn, char *winfn) {
    char *buf;
    FILE *fpOut;
    int *blocks, recs, rpb, nblocks, n, i, j;

    // 1. find blocks in file
    if ((blocks=getBlocksInFile(d,cpmfn)) == NULL) return 1;

    // 2. find size
    if ((recs= getCpmFileSize(d,cpmfn)) < 0) {
        printf("Error: file not found\n");
        return 1;
    }

    // 3. how many blocks do we need to read
    rpb= d->blksiz / 128;		// records per block
    nblocks= (recs + rpb - 1)/rpb;	// blocks needed, rounded up

    // 4. write to out file
    buf= malloc(d->blksiz);
    if ((fpOut=fopen(winfn,"wb"))==NULL) {
        printf("Error: can't create %s\n",winfn);
        return 1;
    }
    for (n=0, i=0; i<nblocks; i++) {
        if (bdosRdBlock(d,blocks[i],buf) != 0) return 1;
        for (j=0; j<rpb && n<recs; j++, n++)
            fwrite(buf+j*128,128,1,fpOut);
    }

    free(buf); fclose(fpOut);
    return 0;
}




/////// Applications //////

/// INIT - Initialize a Disk ///

int cmdInit(DSKFMT *d, int argc, char *argv[]) {
    char buf[128];
    int i, j;

    if (argc!=1)     { printf("Error: init\n"); return 1; }
    if (d->fn==NULL) { printf("Error: init needs imagefile\n"); return 1; }

    if ((d->fp=fopen(d->fn,"wb"))==NULL)
        { printf("Error: can't write to %s\n",d->fn); return 1; }

    // boot sector
    for (i=0; i<128; i++) buf[i]= 0xE5;
    strncpy(buf+120,d->name,strlen(d->name));
    if (biosWrite(d,0,1,buf) != 0)
        return 1;

    // rest of Track 0
    for (i=120; i<128; i++) buf[i]= 0xE5;
    for (i=2; i <= d->spt0 * d->secsiz0 / 128; i++)
        if (biosWrite(d,0,i,buf) != 0)
            return 1;

    // rest of tracks
    for (j=1; j< d->tracks; j++)
        for (i=1; i <= d->spt1 * d->secsiz1 / 128; i++)
            if (biosWrite(d,j,i,buf) != 0)
                return 1;

    fclose(d->fp);
    printf("OK.\n");
    return 0;
}

/// DIR - Directory ///

int cmdDir(DSKFMT *d, int argc, char *argv[]) {
    int i, j;
    char *patn, **list, *tmp;

    if (argc>2) { printf("Error: dir [patn]\n"); return 1; }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"rb") != 0) return 1;

    // 1. Get list of files
    patn= (argc==1)? "*.*" : argv[1];
    list= cpmDir(d,patn);
    if (list[0] == NULL) { printf("No file(s) found\n"); free(list); return 1; }

    // 2. Simplistic sort
    for (i=0; list[i] != NULL; i++)
        for (j=i+1; list[j] != NULL; j++)
            if (strcmp(list[i],list[j])>0)
                { tmp=list[i]; list[i]=list[j]; list[j]=tmp; }

    // 3. Print the list
    for (i=0; list[i] != NULL; i++)
        printf("%s\n",list[i]);

    FreeLines(list); free(list);
    return 0;
}

/// R - Read File from Disk Image ///

int cmdR(DSKFMT *d, int argc, char *argv[]) {
    char *patn, *dir, **list, *p2;
    int action, i, err;

    if (argc>3) { printf("Error: r patn [dir]\n"); return 1; }
    patn= argv[1];
    dir = (argc==3)? argv[2] : ".";

    // 0. Open disk image
    if (DskOpen(d,d->fn,"rb")!=0) return 1;

    // 1. Find matching CP/M files
    list= cpmDir(d,patn);
    if (list[0] == NULL) { printf("No file(s) found\n"); free(list); return 1; }

    // 2. Decide action based on args
    action  = (strcmp(dir,".")==0) ? 0 :
              os_isFile(dir)       ? 1 :
              os_isDir( dir)       ? 2 : 3;

    // 3. Carry out the action
    err= 0;				// no errors, so far
    switch (action) {
    case  0: for (i=0; list[i] != NULL; i++)	// *.*
                 err |= cpmCp2win(d,list[i],list[i]);
             break;
    case  1: err=1; printf("Error: destination must be a dir\n"); break; // *.* file
    case  3: MkDir(dir);			// *.* missing
    case  2: for (i=0; list[i] != NULL; i++) {	// *.* dir
                 p2= StrDirFn(dir,list[i]);
                 err |= cpmCp2win(d,list[i],p2);
                 free(p2);
             }
             break;
    }

    FreeLines(list); free(list);
    if (! err) printf("OK.\n");
    return err;
}

/// W - Write file (to disk image) ///

int cmdW(DSKFMT *d, int argc, char *argv[]) {
    FILE *fp2;
    long pos, len;
    char *buf, *fcb;
    int i, j, k, n, rpb, *blocks, eno, nrecs, bno, rec;
    int lo, hi;

    if (argc < 2) { printf("Error: w file [file ...]\n"); return 1; }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    buf= malloc(128);
    rpb= d->blksiz / 128;		// records per block
    for (k=1; k<argc; k++) {		// for each file

//printf("cmdW 2 %s\n",argv[k]);
        // 1. open input file
        len= FileSize(argv[k]);		// -1 if not found
        if ((fp2=fopen(argv[k],"rb"))==NULL) {
            printf("Error: can't find %s\n",argv[k]);
            return 1;
        }

        // 2. delete all user 0 file extents matching this name
        deleteCpmFile(d,argv[k]);	// continue after deleted or if none

        // 3. find all blocks in use
        if ((blocks=getBlocksInUse(d)) == NULL) return 1;

        // 4. create extents as needed
        for (pos=0, eno=0; pos<len; eno++) {	// eno= extent number
            if ((fcb= createExtent(d,argv[k],eno)) == NULL) return 1;

            // add blocks to the extent
            for (nrecs=0, i=0; i<d->fcbMapLen && pos<len; i++) {
                if ((bno= getUnusedBlock(d,blocks)) < 0) return 1; 
                setFCBmap(d,fcb,i,bno);		// add it to this extent

                // add records to the block
                if (dbg&2) printf("Write block %04X\n",bno);
                rec= bno * rpb;		// starting rec of this block
                for (j=0; j<rpb && pos<len; j++, nrecs++, rec++, pos+=128) {
                    if ((n=fread(buf,1,128,fp2)) == 0) break;
                    while (n<128) buf[n++]=0x1A;    // pad record out with 0x1A
                    if (bdosWrDatRec(d,rec,buf) != 0) return 1; // write record
                }
            }
            setFCBrc(d,fcb,nrecs);
//sayhex(32,fcb);
        }
        fclose(fp2);
    }

    // 5. write entire dir
    if (bdosWrDir(d) != 0) return 1;

    DskClose(d); free(buf);
    printf("OK.\n");
    return 0;
}

/// RS - Read System Area ///

int cmdRS(DSKFMT *d, int argc, char *argv[]) {
    int start, len, recs0, recs1, i, j, k, n;
    char *fn, buf[128];
    FILE *fp2;

    switch (argc) {
    case  1: start=0;             len=9999;          fn="sysarea.sys"; break;
    case  2: start=0;             len=9999;          fn=argv[1];       break;
    case  3: start=atoi(argv[1]); len=9999;          fn=argv[2];       break;
    case  4: start=atoi(argv[1]); len=atoi(argv[2]); fn=argv[3];       break;
    default: printf("Error: rs [fn], or\n");
             printf("       rs start [len] fn\n");
             return 1;
    }

    if (DskOpen(d,d->fn,"rb")!=0) return 1;
    if ((fp2=fopen(fn,"wb"))==NULL)
        { printf("Error: can't create %s\n",fn); return 1; }

    recs0= d->spt0 * d->secsiz0 / 128;
    recs1= d->spt1 * d->secsiz1 / 128;
    for (n=0, i=0, j=0; j < d->rsvd; j++)
        for (k=1; k <= ((j==0)?recs0:recs1); k++, i++)
            if (start<=i && i<start+len) {
                biosRead(d,j,k,buf);
                fwrite(buf,128,1,fp2);
                n++;
            }

    fclose(fp2);
    DskClose(d);

    printf("%d records read.\n",n); 
    printf("OK.\n");
    return 0;
}

/// WS - Write to System Area ///

int cmdWS(DSKFMT *d, int argc, char *argv[]) {
    int start, len, recs0, recs1, i, j, k, n;
    char *fn, buf[128];
    FILE *fp2;

    switch (argc) {
    case  1: start=0;             len=9999;          fn="sysarea.sys"; break;
    case  2: start=0;             len=9999;          fn=argv[1];       break;
    case  3: start=atoi(argv[1]); len=9999;          fn=argv[2];       break;
    case  4: start=atoi(argv[1]); len=atoi(argv[2]); fn=argv[3];       break;
    default: printf("Error: ws [fn], or\n");
             printf("       ws start [len] fn\n");
             return 1;
    }

    if (DskOpen(d,d->fn,"r+b")!=0) return 1;
    if ((fp2=fopen(fn,"rb"))==NULL)
        { printf("Error: can't find %s\n",fn); return 1; }

    recs0= d->spt0 * d->secsiz0 / 128;
    recs1= d->spt1 * d->secsiz1 / 128;
    for (n=0, i=0, j=0; j < d->rsvd; j++)
        for (k=1; k <= ((j==0)?recs0:recs1); k++, i++)
            if (start<=i && i<start+len) {
                if (biosRead( d,j,k,buf) != 0) return 1;
                if (fread(buf,1,128,fp2) == 0) break;
                if (biosWrite(d,j,k,buf) != 0) return 1;
                n++;
            }

    fclose(fp2);
    DskClose(d);

    printf("%d records written.\n",n); 
    printf("OK.\n");
    return 0;
}

/// DIR2 - Hex Dump of Directory ///

int cmdDir2(DSKFMT *d, int argc, char *argv[]) {
    int dpb, i, y, flags;
    char *fcb, fn[8+1], buf[128];

    if (argc>1) { printf("Error: dir2\n"); return 1; }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"rb")!=0) return 1;

    // 1. Show the boot sector and any disk type (bytes 120-125/127)
    if (biosRead( d,0,1,buf) != 0) return 1;
    printf("\nBoot Sector\n");
    sayhex(128,buf);
    printf("\n");

    // 2. If it has DPB information, show that
    if (hasDPB(buf)) {
        dpb= get1(buf+127);
        i  = (dpb/4)&3;
        printf("DPB\n");
        printf("Reserved tracks: %d,",dpb&3);
        printf(" Block size: %dK,",i==0?1:2);
        printf(" Dir type: %d,",i==0?1:i);
        printf(" Dir blocks: %d\n\n",dpb/16);
    }

    // 3. for each dir entry, print
    for (i=0; i<d->fcbs; i++) {
        fcb= d->dir + (i*FCBSIZE);
        if (get1(fcb+0)==0x81 && i==0) { // CDOS label
            strncpy(fn ,fcb+1,8); fn[ 8]='\0'; trim(fn);
            y= get1(fcb+11); if (y<70) y+=100;
            flags= get1(fcb+13);
            printf("CDOS disk label: %s\n",fn);
            printf("Date on disk   : %4d-%02d-%02d\n",1900+y,fcb[9],fcb[10]);
            printf("Cluster size   : %dK\n",fcb[12]/8);
            printf("Directory type : %s\n",flags==0x80? "2 byte block numbers" :
                                           flags==0x00? "Normal" :
                                           "**UNSUPPORTED**");
            printf("Directory size : %d entries\n\n",get1(fcb+15)*4); // 4 per record
        }
        if (i==0) printf("Directory\n");
        if (get1(fcb+0) != 0xE5) sayhex(32,fcb);
    }
    printf("\n");
    return 0;
}

/// ERA - Erase file(s) ///

int cmdEra(DSKFMT *d, int argc, char *argv[]) {

    if (argc!=2) { printf("Error: era patn\n"); return 1; }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    // 1. delete all user 0, non-sys/ro file extents matching this name
    if (deleteCpmFile(d,argv[1]) != 0) {
        printf("Error: file not found\n");
        return 1;
    }

    // 2. write entire directory back out
    if (bdosWrDir(d) != 0) return 1;

    DskClose(d);
    printf("OK.\n");
    return 0;
}

/// USED - Show Block Use Map ///

int cmdUsed(DSKFMT *d, int argc, char *argv[]) {
    int i, j, x, n, *blocks;

    if (argc != 1) { printf("Error: used\n"); return 1; }

    if (DskOpen(d,d->fn,"rb")      !=    0) return 1;
    if ((blocks=getBlocksInUse(d)) == NULL) return 1;

    printf("\n");
    printf("Blocks in use:\n");
    for (i=0, x=0; x < d->blocks; i++) {
        for (j=0; j<16 && (x < d->blocks); j++, x++)
            printf("%s",blocks[x]>1?"X":blocks[x]>0?"1":"0");
        printf("\n");
    }

    for (n=0, i=0; i < d->blocks; i++) if (blocks[i]>0) n++;
    printf("\n");
    printf("%d of %d blocks used\n",n,d->blocks);
    printf("OK.\n");
    return 0;
}

/// INFO - Show Disk Geometry and File System Configuration ///

int cmdInfo(DSKFMT *d, int argc, char *argv[]) {
    int i, j, size, dsiz, *stbl, n, w;

    if (argc>1) { printf("Error: info\n"); return 1; }

    size= _calcDskSize(d);
    dsiz= _calcDatAreaSize(d);
    stbl= _calcSkewTbl(d->spt1, d->skew);

    printf("\n");
    printf("Name              : %s\n",d->name);
    printf("Description       : %s\n",_calcDescription(d));
    printf("Image size        : %5d records (%d bytes)\n",size/128,size);
    printf("System area size  : %5d records\n",_calcSysAreaSize(d)/128);
    printf("Data area size    : %5d records (%dK)\n",dsiz/128,dsiz/1024);
    printf("Block size        : %5d records (%dK)\n",d->blksiz/128,d->blksiz/1024);
    printf("Dir size          : %5d records (%d entries, %d blocks)\n",
        d->dirblks*d->blksiz/128,d->dirblks*d->blksiz/32,d->dirblks);
    printf("Dir type          : %5d (%s)\n",d->dirtype,
        d->dirtype==1? "1 byte block numbers" :
        d->dirtype==2? "2 byte block numbers" :
        d->dirtype==3? "CDOS 8x 1 byte block numbers" :
        d->dirtype==4? "Cromix" : "unknown");
    printf("Tracks            : %5d\n",d->tracks);
    printf("Reserved tracks   : %5d\n",d->rsvd);
    printf("Track 0           : %5d sectors x %3d bytes\n",d->spt0,d->secsiz0);
    printf("Track 1...        : %5d sectors x %3d bytes\n",d->spt1,d->secsiz1);
    printf("Skew factor       : %d\n",d->skew);

    n=20; w=2; if (d->spt1>99) { n=15; w=3; }
    for (i=0; i<d->spt1; ) {
        printf("Skew table        : %*d",w,stbl[i++]);
        for (j=1; j<n && i<d->spt1; j++, i++) printf(",%*d",w,stbl[i]);
        printf("\n");
    }

    free(stbl);
    return 0;
}

/// REN - Rename a file ///

int cmdRen(DSKFMT *d, int argc, char *argv[]) {
    int err=1, i, j;
    char*fm, *to, **list, FnFCB1[1+8+3+1], FnFCB2[1+8+3+1], *fcb;

    if (argc==4 && strcmp(argv[2],"=")==0) { err=0; fm=argv[3]; to=argv[1]; }
    if (argc==3) { err=0; fm=argv[1]; to=argv[2]; }
    if (err) {
        printf("Error: ren new = old, or\n");
        printf("       ren old new\n");
        return 1;
    }

    // 0. Open
    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    // 1. Checks
    if (isAFN(fm) || isAFN(to))
        { printf("Error: ren doesn't yet support wildcards\n"); return 1; }
    if (strchr(fm,'/') || strchr(fm,'\\') ||strchr(to,'/') || strchr(to,'\\'))
        { printf("Error: ren doesn't support paths\n"); return 1; }

    list= cpmDir(d,to);
    if (list[0] != NULL) { printf("Error: file already exists\n"); return 1; }
    FreeLines(list); free(list);

    list= cpmDir(d,fm);
    if (list[0] == NULL) { printf("Error: file not found\n"); return 1; }
    FreeLines(list); free(list);

    // 2. Rename
    cpmFormatNameToFCB(fm,FnFCB1); FnFCB1[12]='\0';
    cpmFormatNameToFCB(to,FnFCB2); FnFCB2[12]='\0';
    for (i=0; i<d->fcbs; i++) {
        fcb= d->dir + (i*FCBSIZE);
        if (get1(fcb+0) != 0x00) continue;
        for (j=1; j<12 && fcb[j]==FnFCB1[j]; j++) ;
        if (j==12) strncpy(fcb+1,FnFCB2+1,11); // no trailing \0
    }

    // 3. Rewrite entire dir
    if (bdosWrDir(d) != 0) return 1;

    DskClose(d);
    printf("OK.\n");
    return 0;
}

/// LBL - Add or Update the CDOS Disk Label

int cmdLbl(DSKFMT *d, int argc, char *argv[]) {
    time_t now;
    struct tm *t;
    int i, y, flags;
    char *name, *fcb, fn[8+1];

    switch (argc) {
    case  1: name="Userdisk"; break;
    case  2: name= argv[1]; break;
    default: printf("Error: lbl [name]\n"); return 1;
    }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    // 1. Some checks
    if (d->secsiz0 != 128)
        { printf("Error: CDOS requires SD track 0\n"); return 1; }
    if (d->rsvd != (((d->spt0*d->secsiz0 + d->spt1*d->secsiz1)>=52*128)?2:3))
        { printf("Error: rsvd tracks doesn't match CDOS value\n"); return 1; }
    if (d->dirtype <1 || d->dirtype>3)
        { printf("Error: invalid dirtype\n"); return 1; }
    if (d->dirtype==1 && d->blksiz>1024)
        { printf("Error: incompatible FCB type (CP/M 2K+ 1byte)\n"); return 1; }

    // 2. Get first dir entry
    fcb= d->dir;
//sayhex(32,fcb);
    switch (get1(fcb+0)) {
    case 0xE5: // empty
            now= time(NULL);
            t= localtime(&now);
            for (i=0; i<FCBSIZE; i++) fcb[i]=0;    // no blocks alloc'd, no recs, no flags ...
            fcb[ 0]=0x81;
            strncpy(fcb+1,name,8); for (i=strlen(name); i<8; i++) fcb[1+i]=' ';
            fcb[ 9]= t->tm_mon+1; // month (1-12)
            fcb[10]= t->tm_mday; // day
            fcb[11]= t->tm_year; // year-1900
            fcb[12]= d->blksiz/128; // 8,16,32 = 1K,2K,4K
            fcb[13]= (d->bnoSize==2)? 0x80 : 0x00; // 1 or 2 byte block numbers
            fcb[15]= d->fcbs/4;
            for (i=0; i<d->dirblks; i++)
                setFCBmap(d,fcb,i,i);             // add dir blocks to this extent
//sayhex(32,fcb);
            if (bdosWrDir(d) != 0) return 1;    // Rewrite entire dir
            printf("OK.\n");
            break;
    case 0x81: // CDOS label
            strncpy(fcb+1,name,8); for (i=strlen(name); i<8; i++) fcb[1+i]=' ';
            if (bdosWrDir(d) != 0) return 1;    // Rewrite entire dir
            printf("OK. (name updated).\n");
            break;
    default: // a file
            printf("Error: can't add label - era first file or use another disk\n");
    }

    DskClose(d);
    return 0;
}

/// DPB - Write Disk Parameter Block info to boot sector ///
/// 00nnssrr - n=dirblks(1,2,4) s=0-2(1K,2K,4K) r=rsvd tracks
/// 1K blocks -> dirtype 1; 2K or 4K blocks -> dirtype 2

int cmdDPB(DSKFMT *d, int argc, char *argv[]) {
    int err, n;
    char buf[128];

    if (argc != 1) { printf("Error: dpb\n"); return 1; }

    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    if (d->dirblks < 1 || d->dirblks > 4)
        { printf("Error: dpb- directory size must be 1-4 blocks\n"); return 1; }
    if (d->rsvd    < 0 || d->rsvd    > 3)
        { printf("Error: dpb- reserved tracks must be 0-3\n"); return 1; }
    if (d->blksiz > 2048)
        { printf("Error: dpb- block size must be 1K or 2K\n"); return 1; }

    n= -1;
    if (d->dirtype==1 && d->blksiz==1024) n=0;	// original map format cpm/cdos
    if (d->dirtype==1 && d->blksiz==2048) n=1;	// 2K/1byte cpm only
    if (d->dirtype==2 && d->blksiz==2048) n=2;	// 2K/2byte cpm/cdos
    if (d->dirtype==3 && d->blksiz==2048) n=3;	// 2K/1byte cdos only
    if (n<0) {
        printf("Error: dpb- invalid directory type / block size combination.\n");
        return 1;
    }

    if (biosRead( d,0,1,buf) != 0) return 1;
    buf[126]= 0xE5;
    buf[127]= d->dirblks*16 + n*4 + d->rsvd;
    if (biosWrite(d,0,1,buf) != 0) return 1;

    DskClose(d);

    printf("OK.\n");
    return 0;
}

/// RMLBL - Remove the CDOS Disk Label

int cmdRmLbl(DSKFMT *d, int argc, char *argv[]) {
    int i;
    char *fcb;

    if (argc != 1) { printf("Error: rmlbl\n"); return 1; }

    // 0. Open disk image
    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    // 1. Get first dir entry
    fcb= d->dir;
    switch (get1(fcb+0)) {
    case 0xE5: // empty
            printf("OK.\n");
            break;
    case 0x81: // CDOS label
            for (i=0; i<FCBSIZE; i++) fcb[i]=0xE5;
            if (bdosWrDir(d) != 0) return 1;
            printf("OK.\n");
            break;
    default: // a file
            printf("Error: file in label slot - era first file\n");
    }

    DskClose(d);
    return 0;
}

/// RMDPB - Remove the DPB byte

int cmdRmDPB(DSKFMT *d, int argc, char *argv[]) {
    char buf[128];

    if (argc != 1) { printf("Error: rmdpb\n"); return 1; }

    if (DskOpen(d,d->fn,"r+b")!=0) return 1;

    if (biosRead( d,0,1,buf) != 0) return 1;
    buf[126]= 0xE5;
    buf[127]= 0xE5;
    if (biosWrite(d,0,1,buf) != 0) return 1;

    DskClose(d);

    printf("OK.\n");
    return 0;
}

////

void Usage(void) {
    printf("\n");
    printf("CP/M File System Utility Ver %d.%02d%s\n",VER_MAJOR,VER_MINOR,VER_DEV);
    printf("%s\n\n",VER_COPYR);
    printf("Usage: cpmfs [-d dbg] [-t type] imagefile [opts] cmd [...]\n");
    printf("\n");
    printf("cmd: dir [patn]       : list   cpm files in imagefile [like patn]\n");
    printf("     era fn1          : erase  cpm file fn1\n");
    printf("     ren fn1 fn2      : rename cpm file fn1 as fn2\n");
    printf("     r fn1 [dir]      : read   file(s) to win [dir] (wildcards ok)\n");
    printf("     w fn1 ... fnN    : write  file(s) to cpm (dir & wildcards ok)\n");
    printf("\n");
    printf("dbg: use 1 for track, sector, disk addr; 2 for block info; or 3 for both\n");
    printf("Default type is worked out from the imagefile size.\n");
    printf("Use \"cpmfs types\" for a list of supported types.\n");
    printf("Use \"cpmfs opts\" for options (adjust disk params).\n");
    printf("Use \"cpmfs adv\" for advanced commands.\n");
}

void Usage2(void) {
    int i, n;

    printf("\n");
    printf("CP/M File System Utility Ver %d.%02d%s\n",VER_MAJOR,VER_MINOR,VER_DEV);
    printf("%s\n\n",VER_COPYR);
    printf("Supported imagefile types are:\n\n");
    printf("Type       : Description\n");
    printf("---------------------------------------------------------------\n");
    n= sizeof(fmts) / sizeof(DSKFMT);
    for (i=0; i<n; i++)
        printf("%-10s : %s\n",fmts[i].name,_calcDescription(fmts+i));
    printf("\n");
    printf("Use \"cpmfs -t (type) info\" for more detail.\n");
}

void Usage3(void) {
    int i, n;

    printf("\n");
    printf("CP/M File System Utility Ver %d.%02d%s\n",VER_MAJOR,VER_MINOR,VER_DEV);
    printf("%s\n\n",VER_COPYR);
    printf("Usage: cpmfs [-d dbg] [-t type] imagefile [opts] cmd [...]\n");
    printf("\n");
    printf("opts are one or more of:\n");
    printf("-tks num              : disk size in tracks\n");
    printf("-spt num              : sectors per track\n");
    printf("-siz bytes            : sector size in bytes (eg 128 or 512)\n");
    printf("-rsvd tracks          : system area in tracks\n");
    printf("-sk num               : skew factor\n");
    printf("-bs num               : block size in KB (1/2/4)\n");
    printf("-ds blocks            : dir size in blocks (1-8)\n");
    printf("-dt num               : directory type 1, 2 or 3(CDOS 2K 1byte)\n");
    printf("-spt0 num             : as above, for track 0\n");
    printf("-siz0 bytes           : as above, for track 0\n");
    printf("\n");
}

void Usage4(void) {
    printf("\n");
    printf("CP/M File System Utility Ver %d.%02d%s\n",VER_MAJOR,VER_MINOR,VER_DEV);
    printf("%s\n\n",VER_COPYR);
    printf("Usage: cpmfs [-d dbg] [-t type] imagefile [opts] cmd [...]\n");
    printf("       cpmfs [-d dbg]  -t type  info\n");
    printf("\n");
    printf("cmd: init             : create or format imagefile\n");
    printf("     lbl [name]       : set disk label for CDOS\n");
    printf("     dpb              : write DPB byte for CP/M\n");
    printf("     info             : show   auto/selected imagefile format\n");
    printf("     dir2             : list   cpm files in raw FCB format\n");
    printf("     used             : show   block allocations\n");
    printf("     rs fn1           : read   system tracks -> win fn1\n");
    printf("     ws fn1           : write  system tracks <- win fn1\n");
    printf("     rs start len fn1 : read   system records -> win fn1\n");
    printf("     ws start len fn1 : write  system records <- win fn1\n");
    printf("                        (boot= 0 1, bios varies)\n");
    printf("     rmlbl            : remove CDOS disk label\n");
    printf("     rmdpb            : remove CP/M DPB byte\n");
    printf("\n");
}

int dsk_useDPB(DSKFMT *d) {
    FILE *fp;
    int n, dpb;
    byte buf[128];

    if (d->fn == NULL) return 1;		// no filename
    if ((fp=fopen(d->fn,"rb"))==NULL) return 2;	// no file
    n= fread(buf,128,1,fp);
    fclose(fp);

    if (n != 1) return 3;			// read failed
    if (!hasDPB(buf)) return 4;			// no DPB

    dpb= buf[127];
    n  = (dpb/4)&3;
    d->rsvd   = dpb&3;
    d->blksiz = (n==0)? 1024 : 2048;
    d->dirtype= (n==0)? 1 : n;
    d->dirblks= dpb/16;
    return 0;
}

int dsk_opts(DSKFMT *d, int argc, char *argv[]) {
    int i, j, n;
    char *opt[] =
        { "-sk", "-tks", "-spt", "-spt0", "-siz", "-siz0", "-bs",
          "-dt", "-rsvd", "-ds", NULL };

    for (i=0; i<argc; i++) {
        for (j=0; opt[j]!=NULL && strcmp(argv[i],opt[j])!=0; j++) ;
        switch (j) {
        case  0: d->skew   = atoi(argv[i+1]); i++; break;
        case  1: d->tracks = atoi(argv[i+1]); i++; break;
        case  2: d->spt1   = atoi(argv[i+1]); i++; break;
        case  3: d->spt0   = atoi(argv[i+1]); i++; break;
        case  4: d->secsiz1= atoi(argv[i+1]); i++; break;
        case  5: d->secsiz0= atoi(argv[i+1]); i++; break;
        case  6: n         = atoi(argv[i+1]); i++;
                 d->blksiz = n*1024;
                 if (n!=1 && n!=2 && n!=4) {
                     printf("Error: [-bs blocksize] should be 1, 2 or 4 (KB)\n");
                     return -1;
                 }
                 break;
        case  7: d->dirtype= atoi(argv[i+1]); i++; break;
        case  8: d->rsvd   = atoi(argv[i+1]); i++; break;
        case  9: d->dirblks= atoi(argv[i+1]); i++;
                 if (d->dirblks < 1 || d->dirblks > 8) {
                     printf("Error: [-ds blocks] dir size should be 1-8\n");
                     return -1;
                 }
                 break;
        default: return i;
        }
    }
    return i;
}

/*
   NOTE: When built in MinGW, the C startup (C0.c) automatically translates
   any ambiguous filenames to a list of actual files. This is consistent with
   what would happen in a unix environment (MinGW's purpose) and a unix shell.
   In Windows, the cmd.exe "shell" does not translate AFNs. If you build this
   outside of MinGW, you will need to add functionality to do that.

   Passing an AFN for the CP/M filename should be straight forward with cmd.exe
   (just use *.* etc on the command line) as it passes these unchanged to the
   program. However, given the MinGW automatic translation within any program
   built using it, you need to quote the AFN (as if you are using unix) - use 
   "*.*" to get it into this program as *.*. It's a great feature. Just be aware
   of when you want it (expanding windows filenames), when you don't (it can't
   expand CP/M file names stored within an imagefile), and the need to match it
   if you migrate this to a different build environment.

   Cmd.exe does none of this.
*/
int main(int argc, char *argv[]) {
    int i;
    char *fn;
    DSKFMT *d = NULL;
    char *cmds[] = {
        "init", "dir", "r", "w", "rs", "ws", "dir2", "era",
        "used", "info", "ren", "lbl", "dpb", "rmlbl", "rmdpb", NULL };

    // special cases ...
    if (argc==2 && strcmp(argv[1],"types")==0) { Usage2(); return 0; }
    if (argc==2 && strcmp(argv[1],"opts" )==0) { Usage3(); return 0; }
    if (argc==2 && strcmp(argv[1],"adv"  )==0) { Usage4(); return 0; }

    // move past progname
    argc-=1; argv+=1;

    // set debug option and move past it, if present
    if (argc>=2 && strcmp(argv[0],"-d")==0)
        { dbg=atoi(argv[1]); argc-=2; argv+=2; }

    // set disktype from option and move past it, if present
    if (argc>=2 && strcmp(argv[0],"-t")==0) {
        if ((d=chooseDiskTypeFromName(argv[1]))==NULL) return 1;
        argc-=2; argv+=2;
    }

    // set image fn and move past it, if present
    fn= NULL;
    if (argc>0 && argv[0][0]!='-' && stricmp(argv[0],"info")!=0)
        { fn=argv[0]; argc--; argv++; }

    // if disktype not specified, must include an image file name
    if (d==NULL) {
        if (fn==NULL) { Usage(); return 1; }
        if ((d=chooseDiskTypeFromBootSector(fn)) == NULL &&
            (d=chooseDiskTypeFromSize(fn)      ) == NULL) return 1;
    }
    d->fn= fn;

    // if we have an image file with a DPB, use settings from there
    dsk_useDPB(d);

    // allow user to tweak disk params
    if ((i=dsk_opts(d,argc,argv)) < 0) return 1;
    argc-=i; argv+=i;

    if (argc<1) { printf("Error: cmd missing\n"); return 1; }
    for (i=0; cmds[i]!=NULL && stricmp(argv[0],cmds[i])!=0; i++) ;
    if (i!=9 && d->fn==NULL) { printf("Error: must specify an imgfn\n"); return 1; }
    switch (i) {
    case  0: return cmdInit( d,argc,argv);
    case  1: return cmdDir(  d,argc,argv);
    case  2: return cmdR(    d,argc,argv);
    case  3: return cmdW(    d,argc,argv);
    case  4: return cmdRS(   d,argc,argv);
    case  5: return cmdWS(   d,argc,argv);
    case  6: return cmdDir2( d,argc,argv);
    case  7: return cmdEra(  d,argc,argv);
    case  8: return cmdUsed( d,argc,argv);
    case  9: return cmdInfo( d,argc,argv);
    case 10: return cmdRen(  d,argc,argv);
    case 11: return cmdLbl(  d,argc,argv);
    case 12: return cmdDPB(  d,argc,argv);
    case 13: return cmdRmLbl(d,argc,argv);
    case 14: return cmdRmDPB(d,argc,argv);
    }

    Usage();
    return 1;
}

