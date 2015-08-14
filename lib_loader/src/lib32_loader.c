#include <elf/lib32.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>


/* - mapping the elf accroding to program headers
 * @ elf_hdr 
 * 
 *
 */
void* 
lib32_mapping(void *p_mem) {

    Elf32_Ehdr   *elf_hdr  = NULL;    // new mapping elf header 
    Elf32_Phdr   *elf_phdr = NULL;
    Lib32_Info   *libinfo  = NULL; 
    Elf32_Half   ph_num    = 0;
    Elf32_Half   sh_num    = 0;
    Elf32_Word   m_size    = 0;               // total aligned size of all segements  with PT_LOAD flag 
    Elf32_Half   align     = 0;
    Elf32_Half   pre_align = 0;
    Elf32_Half   index     = 0;
    void         *p_newmem = NULL; 
    void         *ptr      = NULL ; 

    if( NULL == p_mem ) {
        exit(EXIT_FAILURE);
    }

    libinfo  = (Lib32_Info*)p_mem; 
    elf_hdr  = (Elf32_Ehdr*) (libinfo->fmem);    
    
    /* verify elf foramt*/
    if( elf32_checkFile(elf_hdr) < 0 ) {
        printf("[err] elf file error!\n");
        exit(EXIT_FAILURE);
    }
    
    elf_phdr = elf32_getProgramSegmentTable(elf_hdr); 
    ph_num   = elf32_getNumProgramHeaders(elf_hdr);
    m_size   = elf32_getMappingSize(elf_hdr);             // bugs
    dbg_printf("[info] m_size: 0x%08x\n\n",m_size);
    assert(m_size > 0 );
    
    p_newmem = mmap( 0, 
                     m_size, 
                     PROT_READ | PROT_WRITE, 
                     MAP_PRIVATE | MAP_ANONYMOUS, 
                     -1, 
                     0); 
    if((int)p_newmem == -1 ){
        printf("[err] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    dbg_printf("[info] p_newmem:0x%08x  size:0x%08x\n\n", p_newmem, m_size);
    
    pre_align = 0;
    for( index=0; index<ph_num; index++) {
        if( elf_phdr[index].p_type == PT_LOAD ) {
            align = ALIGN_SIZE( (elf_phdr[index].p_offset % elf_phdr[index].p_align + elf_phdr[index].p_filesz),
                                 elf_phdr[index].p_align );
            ptr = mmap( p_newmem + pre_align, 
                        align, 
                        segflag2pgflag(elf_phdr[index].p_flags) |  PROT_WRITE ,
                        //PROT_READ | PROT_WRITE,
                        //MAP_PRIVATE | MAP_DENYWRITE | MAP_FIXED, 
                        MAP_PRIVATE | MAP_FIXED, 
                        libinfo->fd, 
                        (elf_phdr[index].p_offset & ~(sysconf(_SC_PAGE_SIZE)-1))); 
            if( (int)ptr == -1 ) {
                printf("[err] %s\n", strerror(errno));
                exit(EXIT_FAILURE); 
            }
            dbg_printf("[mmap] range:0x%08x-0x%08x  flag:0x%04x\n", ptr, ptr+align, elf_phdr[index].p_flags);
            pre_align = align; 
        }
    }
    dbg_printf("\n");
    libinfo->m_base = p_newmem;
    libinfo->m_size = m_size;

    return libinfo;
}


void 
lib32_reloc(void* m_base, Elf32_Ehdr *elf_hdr ) {

    Lib32_Info *libinfo  = NULL;
    Elf32_Shdr *sec_hdr  = NULL;
    Elf32_Half sh_num    = 0;
    Elf32_Half rel_num   = 0;
    Elf32_Sym  *sym_hdr  = NULL;
    Elf32_Addr sym_addr  = 0;
    Elf32_Half sh_index  = 0;
    Elf32_Half rel_index = 0;
    Elf32_Shdr *rel_sec  = NULL;
    Elf32_Rel  *rel_hdr  = NULL;
    Elf32_Sym  *sym_ent  = NULL;
    Elf32_Off  offset    = 0;
    Elf32_Half type      = 0;
    char *str_table      = NULL;
    char *     sym_name  = NULL;

    if(!elf_hdr) {
        return ;
    }
    dbg_printf("elf_hdr: 0x%08x\n",elf_hdr);
    sec_hdr = elf32_getSectionTable(elf_hdr);
    dbg_printf("sec_hdr: 0x%08x\n",sec_hdr);
    sh_num  = elf32_getNumSections(elf_hdr);
    for( sh_index = 0; sh_index < sh_num; sh_index++) {
        /* find the .dynsym section , the section with SHT_DYNSYM type and Alloc flag */
        /* strip will remove section with SYMTAB type */
        if( (sec_hdr[sh_index].sh_type == SHT_DYNSYM) && 
            (sec_hdr[sh_index].sh_flags & SHF_ALLOC) ){ 
            sym_hdr   = (Elf32_Sym *)elf32_getSectioniVirtulAddr(elf_hdr, sh_index);
        }
        /* find the .dynstr section , the first section with SHT_STRTAB type  and Alloc flag*/
        if( (sec_hdr[sh_index].sh_type == SHT_STRTAB) && 
            (sec_hdr[sh_index].sh_flags & SHF_ALLOC ) ) {
            str_table = (char *)elf32_getSectioniVirtulAddr(elf_hdr, sh_index);
        }
    }
    assert( sym_hdr > 0 );
    assert( str_table > 0 );
    if( (!sym_hdr) || (!str_table)  ) {
        printf("[err] fail to find  symbol_table in elf file!\n");
        exit(EXIT_FAILURE);
    }
    dbg_printf("sym_hdr: 0x%08x\n",sym_hdr);

    /* symbols relocation */
    for( sh_index = 1; sh_index < sh_num; sh_index++) {
        if(sec_hdr[sh_index].sh_type == SHT_REL) {
            rel_sec = &sec_hdr[sh_index];
            dbg_printf("rel_sec: 0x%08x\n",rel_sec);
            rel_hdr = (Elf32_Rel*)elf32_getSectioniVirtulAddr(elf_hdr, sh_index);
            dbg_printf("rel_hdr: 0x%08x\n",rel_hdr);
            rel_num  = rel_sec->sh_size /sizeof(Elf32_Rel);
            dbg_printf("rel_num: 0x%08x\n",rel_num);
            
            for( rel_index = 0; rel_index < rel_num; rel_index++ ) {
                offset   = ELF32_R_SYM(rel_hdr[rel_index].r_info);
                type     = ELF32_R_TYPE(rel_hdr[rel_index].r_info);
                sym_ent  = &sym_hdr[offset];
                sym_name = str_table + sym_ent->st_name;

                uint32_t   B = (uint32_t)m_base;                             // elf base virtual mem address
                uint32_t  *T = (uint32_t*)(B + rel_hdr[rel_index].r_offset); // relalloc virtual mem address            
                uint32_t   S = (uint32_t)(B + sym_ent->st_value);            // the target symbol's virtual address
                uint32_t   A = (uint32_t)(*T);                               // the value of  the relalloc virtual memory unit
                uint32_t   P = (uint32_t)T;                                  // the value is same as T
                uint32_t   G = (uint32_t)NULL;
                uint32_t   L = (uint32_t)NULL;
                uint32_t GOT = (uint32_t)NULL;

                switch(type) {
                    case R_386_NONE: {
                        //nop
                        break;
                    }
                    case R_386_32: {       // checked
                        *T = S + A;
                        break;
                    }
                    case R_386_PC32: {     // checked
                        *T = S - P + A;
                        break;
                    }
                    case R_386_GOT32: {    
                        *T = G - P + A;
                        break;
                    }
                    case R_386_PLT32: {    
                        *T = L - P + A;
                        break;
                    }
                    case R_386_COPY: {
                        //nop 
                        break;
                    }
                    case R_386_GLOB_DAT: {  // checked
                        *T = S; 
                        break;
                    }
                    case R_386_JMP_SLOT: {  // checked
                        *T = S;             // for the external symbol ,it should fill the right value in "S" mem unit before doing  R_386_JMP_SLOT!
                        break;
                    }
                    case R_386_RELATIVE: {  // checked
                        *T = B + A;
                        break;
                    } 
                    case R_386_GOTOFF: {
                        *T = S + A - GOT;           
                        break;
                    }
                    case R_386_GOTPC: {
                        *T = GOT - P + A;
                        break;
                    }
                    case R_386_32PLT: {
                        //TODO
                        break;
                    }
                    default : {
                        case R_386_TLS_TPOFF: {
                            //TODO
                            break;
                        }
                        case R_386_TLS_IE: {
                            //TODO
                            break;
                        }
                        case R_386_TLS_GOTIE: {
                            //TODO
                            break;
                        }
                        // more case to consider ...
                    }
                }// switch end
                dbg_printf( "type: 0x%08x offset: 0x%08x value: 0x%08x  sym_name: %s  sym_value: 0x%08x\n", 
                        type,
                        rel_hdr[rel_index].r_offset, 
                        *T, 
                        sym_name , 
                        sym_ent->st_value);
            }
        }
    }
}

void 
lib32_mem_protect(Elf32_Ehdr *elf_hdr) {
    // todo ...
}

void* 
dl_open(const char *filename, int flags) {
    
    Lib32_Info *libinfo = NULL;    
    int   fd    = 0;
    void  *fmem = NULL;
    void  *ptr  = NULL;
    struct stat st;
    off_t  flen = 0L;
    off_t i     = 0;
    
    if(0 > (fd = open(filename, O_RDONLY)) ) {
        printf("[err] %s\n",strerror(errno));
        return 0;
    }
    fstat(fd, &st);
    flen = st.st_size;
    
    if( !(libinfo = (Lib32_Info*)malloc(sizeof(Lib32_Info))) ) {
        return NULL;
    }
    ptr = malloc(flen);
    if( NULL == ptr ){
        printf("[err] malloc error!\n");
        exit(EXIT_FAILURE);
    }
    dbg_printf("[info] malloc addr: 0x%08x\n\n",ptr);
    
    for( i=0; i<flen; i++) {
        read(fd, ptr+i, 1);
    }
    
    libinfo->fmem = ptr;
    libinfo->name = filename;
    libinfo->r_size = flen;
    libinfo->fd = fd;
    /* lib mapping*/
    libinfo = lib32_mapping(libinfo);
    if( NULL == libinfo ) {
        return NULL;
    }
    /* lib relocation */
    lib32_reloc((void*)libinfo->m_base, (Elf32_Ehdr*)libinfo->fmem);

    return libinfo;
}


/* - lookup a specific symbol in the lib
 * @handle : lib handle
 * @symbol : the symbol name exported by lib
 * return  : the symbol's virtual address in the lib mapped in memory
 */
void* 
dl_sym(void *handle, const char *symbol) {
    
    Lib32_Info *libinfo  = NULL;
    Elf32_Ehdr *elf_hdr  = NULL;
    Elf32_Shdr *sec_hdr  = NULL;
    Elf32_Half sh_num    = 0;
    Elf32_Shdr *sym_sec  = NULL;
    Elf32_Sym  *sym_hdr  = NULL;
    Elf32_Addr sym_addr  = 0;
    Elf32_Half sym_index = 0;
    Elf32_Half str_index = 0;
    Elf32_Half index     = 0;
    char *str_table      = NULL;
    char *sym_name       = NULL;

    if(!handle) {
        return NULL;
    }
    
    libinfo = (Lib32_Info*)handle;
    elf_hdr = (Elf32_Ehdr*)(libinfo->fmem);
    assert(elf_hdr > 0);
    dbg_printf("[info] elf hdr: 0x%08x\n\n", elf_hdr);

    sec_hdr = elf32_getSectionTable(elf_hdr);
    assert(sec_hdr > 0);
    dbg_printf("[info] section hdr: 0x%08x\n\n", sec_hdr);
    
    sh_num  = elf32_getNumSections(elf_hdr);
    dbg_printf("[info] sh_num: 0x%08x\n", sh_num);
    
    for( index = 0; index < sh_num; index++) {
        //printf("sec_hdr[0x%02x].sh_type=0x%08x\n",index, sec_hdr[index].sh_type );
        /* find the .dynsym section , the section with SHT_DYNSYM type and Alloc flag */
        /* strip will remove section with SYMTAB type */
        if( (sec_hdr[index].sh_type == SHT_DYNSYM) && ( sec_hdr[index].sh_flags & SHF_ALLOC ) ){ 
            sym_index = index;
            sym_sec   = sec_hdr + sym_index;
            sym_hdr   = (Elf32_Sym *)elf32_getSectioniVirtulAddr(elf_hdr, sym_index);
        }
        /* find the .dynstr section , the first section with SHT_STRTAB type  and Alloc flag*/
        if( (sec_hdr[index].sh_type == SHT_STRTAB)  && ( sec_hdr[index].sh_flags & SHF_ALLOC ) ) {
            str_index = index;
            str_table = (char *)elf32_getSectioniVirtulAddr(elf_hdr, str_index);
        }
    }
    assert( sym_sec > 0 );
    assert( sym_hdr > 0 );
    assert( str_table > 0 );
    if( (!sym_hdr) || (!sym_sec) || (!str_table)  ) {
        printf("[err] fail to find  symbol_table in elf file!\n");
        exit(EXIT_FAILURE);
    }

    /* find the symbol name in .dynstr section */
    dbg_printf("  ::Dynmaic Symbol String Table::\n");
    for( index  = 0; index < (sym_sec->sh_size/(sizeof(Elf32_Sym))); index++){
        sym_name = str_table + sym_hdr[index].st_name;
        dbg_printf("[%04d] 0x%08x  ->  %s\n",index , sym_hdr[index].st_value, sym_name);
        // find the first symbol matched
        if( !sym_addr && (!strcmp(sym_name, symbol)) ){
            sym_addr = sym_hdr[index].st_value;
        }
    }
    dbg_printf("\n");
    return sym_addr > 0 ? (void*)( (char*)(libinfo->m_base) + sym_addr ) : NULL;
}


void 
dl_close(void* handle) {
    
    Lib32_Info *libinfo  = NULL;
 
    if(handle) {
        libinfo = (Lib32_Info*)handle;
        close(libinfo->fd);
        if(libinfo->fmem) {
            free(libinfo->fmem);      
        }
        if(libinfo->m_base) {
            munmap( libinfo->m_base, libinfo->m_size );
        }
        free(libinfo);

        libinfo = NULL;
        handle  = NULL;
    }

}


