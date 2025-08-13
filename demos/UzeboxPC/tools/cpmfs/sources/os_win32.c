
#include <sys/types.h>
#include <sys/stat.h>
#include "os.h"


#define TRUE  (1==1)
#define FALSE (!TRUE)


int os_isDir(char *path) {
    struct _stat buf;
    if (_stat(path,&buf) != 0) return FALSE;
    return (_S_IFDIR & buf.st_mode);
}

int os_isFile(char *path) {
    struct _stat buf;
    if (_stat(path,&buf) != 0) return FALSE;
    return (_S_IFREG & buf.st_mode);
}

