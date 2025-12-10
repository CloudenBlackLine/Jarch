/*
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
 */



#ifndef UTIL_H
#define UTIL_H


#include "../../include/types.h"
#include "../../include/jarch.h"


void		writefile	    (const s8*);
s8* 		readfile	    (const s8*);

bool		start_contains	(s8*, const s8*);
s8*		    read_line       (s8*,s8*);
s8*		    read_splice     (s8*,s8*);
u64		    read_hex        (s8**);

#endif
