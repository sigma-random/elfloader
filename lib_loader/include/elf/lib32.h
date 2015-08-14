#ifndef __LIB32_H__
#define __LIB32_H__

#include "common.h"
#include "elf.h"
 
/* export functions */
//==================================================//

void* dl_open(const char *filename, int flags);

void* dl_sym(void *handle, const char *symbol);

void  dl_close(void* handle);

//==================================================//

typedef struct  {
    void        *fmem;    
    void        *m_base;  // mapping addr of the libfile 
    long        r_size;   // real file size
    long        m_size;   // mapping size
    int         refer;    // reference count
    int         fd;
    const char  *name;
}Lib_Info;
typedef Lib_Info Lib32_Info;


#endif
