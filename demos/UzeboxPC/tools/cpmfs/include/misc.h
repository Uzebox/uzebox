/*
    misc.h
    (c) 2015-2018 Greg Sydney-Smith

    Header for misc.c

*/


#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(!TRUE)
#endif



// Some reasonable assumptions not specifically #included
char *      dirname(char *s);
char *      basename(const char *s);
int         tolower(int c);					// ctype.h



int         stricmp(const char *s1, const char *s2);
char *      trim(char *s);
void        FreeLines(char *lines[]);
char *      StrDirFn(const char *dir, const char *fn);
char *      StrMid(char *fm, int start, int len);
int         MkDir(char *dir);
int         FileExists(char *fn);
long        FileSize(char *fn);

