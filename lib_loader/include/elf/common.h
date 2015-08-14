#ifndef __COMMON__
#define __COMMON__


#include "debug.h"

#define ZERO_OBJ(p_obj) memset(p_obj, 0, sizeof(typeof(*p_obj)))

#define ALIGN_SIZE(size, align) \
    (size % align) > 0 ?        \
    ((size/align)+1)*align :    \
    size                        \


/*
 * segment: PF_R      = 4  PF_W       = 2   PF_X      = 1 
 * mempage: PROT_READ = 1  PROT_WRITE = 2   PROT_EXEC = 4
 */
static int segflag2pgflag(int seg_flags) {
    int page_flags = 0;
    page_flags |= ( seg_flags & 4 ) > 0 ? 1 : 0;    // page_flags |= ( seg_flags & PF_R ) > 0 ? PROT_READ  : 0;
    page_flags |= ( seg_flags & 2 ) > 0 ? 2 : 0;    // page_flags |= ( seg_flags & PF_W ) > 0 ? PROT_WRITE : 0;
    page_flags |= ( seg_flags & 1 ) > 0 ? 4 : 0;    // page_flags |= ( seg_flags & PF_X ) > 0 ? PROT_EXEC  : 0;
    return page_flags;
}


#endif
