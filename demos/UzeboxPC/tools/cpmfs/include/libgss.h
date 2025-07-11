/*
    libgss.h
    (c) 2015-2016 Greg Sydney-Smith

    Shortcut for the following header files

    Revisions
    2016-06-18 Add typedef text
    2016-05-27 Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(!TRUE)
#endif

typedef const char text;	// because it's easier to type everywhere

#include "misc.h"
#include "os.h"
