#ifndef UTIL_H
#define UTIL_H

#include "./linker.h"
#include "../../include/types.h"
#include "../../include/jarch.h"


void		wrtiefile	    (const s8*);
void		readfile	    (const s8*, s8*);

bool		start_contains	(s8*, const s8*);
s8*		    read_line       (s8*,s8*);
s8*		    read_splice     (s8*,s8*);
u64		    read_hex        (s8**);

#endif