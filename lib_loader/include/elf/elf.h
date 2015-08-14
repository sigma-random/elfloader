
#ifndef __ELF_ELF_H__
#define __ELF_ELF_H__

#include <stdint.h>
#include <stdio.h>


#include "common.h"
#include "elf32.h"

/* Legal values for sh_type (section type).  */

#define SHT_NULL      0     /* Section header table entry unused */
#define SHT_PROGBITS      1     /* Program data */
#define SHT_SYMTAB    2     /* Symbol table */
#define SHT_STRTAB    3     /* String table */
#define SHT_RELA      4     /* Relocation entries with addends */
#define SHT_HASH      5     /* Symbol hash table */
#define SHT_DYNAMIC   6     /* Dynamic linking information */
#define SHT_NOTE      7     /* Notes */
#define SHT_NOBITS    8     /* Program space with no data (bss) */
#define SHT_REL       9     /* Relocation entries, no addends */
#define SHT_SHLIB     10        /* Reserved */
#define SHT_DYNSYM    11        /* Dynamic linker symbol table */
#define SHT_INIT_ARRAY    14        /* Array of constructors */
#define SHT_FINI_ARRAY    15        /* Array of destructors */
#define SHT_PREINIT_ARRAY 16        /* Array of pre-constructors */
#define SHT_GROUP     17        /* Section group */
#define SHT_SYMTAB_SHNDX  18        /* Extended section indeces */
#define SHT_NUM       19        /* Number of defined types.  */
#define SHT_LOOS      0x60000000    /* Start OS-specific.  */
#define SHT_GNU_ATTRIBUTES 0x6ffffff5   /* Object attributes.  */
#define SHT_GNU_HASH      0x6ffffff6    /* GNU-style hash table.  */
#define SHT_GNU_LIBLIST   0x6ffffff7    /* Prelink library list */
#define SHT_CHECKSUM      0x6ffffff8    /* Checksum for DSO content.  */
#define SHT_LOSUNW    0x6ffffffa    /* Sun-specific low bound.  */
#define SHT_SUNW_move     0x6ffffffa
#define SHT_SUNW_COMDAT   0x6ffffffb
#define SHT_SUNW_syminfo  0x6ffffffc
#define SHT_GNU_verdef    0x6ffffffd    /* Version definition section.  */
#define SHT_GNU_verneed   0x6ffffffe    /* Version needs section.  */
#define SHT_GNU_versym    0x6fffffff    /* Version symbol table.  */
#define SHT_HISUNW    0x6fffffff    /* Sun-specific high bound.  */
#define SHT_HIOS      0x6fffffff    /* End OS-specific type */
#define SHT_LOPROC    0x70000000    /* Start of processor-specific */
#define SHT_HIPROC    0x7fffffff    /* End of processor-specific */
#define SHT_LOUSER    0x80000000    /* Start of application-specific */
#define SHT_HIUSER    0x8fffffff    /* End of application-specific */

 /* Legal values for sh_flags (section flags).  */

#define SHF_WRITE        (1 << 0)   /* Writable */
#define SHF_ALLOC        (1 << 1)   /* Occupies memory during execution */
#define SHF_EXECINSTR        (1 << 2)   /* Executable */
#define SHF_MERGE        (1 << 4)   /* Might be merged */
#define SHF_STRINGS      (1 << 5)   /* Contains nul-terminated strings */
#define SHF_INFO_LINK        (1 << 6)   /* `sh_info' contains SHT index */
#define SHF_LINK_ORDER       (1 << 7)   /* Preserve order after combining */
#define SHF_OS_NONCONFORMING (1 << 8)   /* Non-standard OS specific handling
                                                       required */
#define SHF_GROUP        (1 << 9)   /* Section is member of a group.  */
#define SHF_TLS          (1 << 10)  /* Section hold thread-local data.  */
#define SHF_MASKOS       0x0ff00000 /* OS-specific.  */
#define SHF_MASKPROC         0xf0000000 /* Processor-specific */
#define SHF_ORDERED      (1 << 30)  /* Special ordering requirement
                                                   (Solaris).  */
#define SHF_EXCLUDE      (1 << 31)  /* Section is excluded unless
                                                   referenced or allocated (Solaris).*/

 /* Section group handling.  */
#define GRP_COMDAT  0x1     /* Mark group as COMDAT.  */

 /* Possible values for si_boundto.  */
#define SYMINFO_BT_SELF     0xffff  /* Symbol bound to self */
#define SYMINFO_BT_PARENT   0xfffe  /* Symbol bound to parent */
#define SYMINFO_BT_LOWRESERVE   0xff00  /* Beginning of reserved entries */

 /* Possible bitmasks for si_flags.  */
#define SYMINFO_FLG_DIRECT  0x0001  /* Direct bound symbol */
#define SYMINFO_FLG_PASSTHRU    0x0002  /* Pass-thru symbol for translator */
#define SYMINFO_FLG_COPY    0x0004  /* Symbol is a copy-reloc */
#define SYMINFO_FLG_LAZYLOAD    0x0008  /* Symbol bound to object to be lazy
                                                       loaded */
 /* Syminfo version values.  */
#define SYMINFO_NONE        0
#define SYMINFO_CURRENT     1
#define SYMINFO_NUM     2


 /* Legal values for ST_BIND subfield of st_info (symbol binding).  */

#define STB_LOCAL   0       /* Local symbol */
#define STB_GLOBAL  1       /* Global symbol */
#define STB_WEAK    2       /* Weak symbol */
#define STB_NUM     3       /* Number of defined types.  */
#define STB_LOOS    10      /* Start of OS-specific */
#define STB_GNU_UNIQUE  10      /* Unique symbol.  */
#define STB_HIOS    12      /* End of OS-specific */
#define STB_LOPROC  13      /* Start of processor-specific */
#define STB_HIPROC  15      /* End of processor-specific */

 /* Legal values for ST_TYPE subfield of st_info (symbol type).  */

#define STT_NOTYPE  0       /* Symbol type is unspecified */
#define STT_OBJECT  1       /* Symbol is a data object */
#define STT_FUNC    2       /* Symbol is a code object */
#define STT_SECTION 3       /* Symbol associated with a section */
#define STT_FILE    4       /* Symbol's name is file name */
#define STT_COMMON  5       /* Symbol is a common data object */
#define STT_TLS     6       /* Symbol is thread-local data object*/
#define STT_NUM     7       /* Number of defined types.  */
#define STT_LOOS    10      /* Start of OS-specific */
#define STT_GNU_IFUNC   10      /* Symbol is indirect code object */
#define STT_HIOS    12      /* End of OS-specific */
#define STT_LOPROC  13      /* Start of processor-specific */
#define STT_HIPROC  15      /* End of processor-specific */


 /* Symbol table indices are found in the hash buckets and chain table
  * of a symbol hash table section.  This special index value indicates
  * the end of a chain, meaning no further symbols are found in that bucket.  */

#define STN_UNDEF   0       /* End of a chain.  */


 /* Symbol visibility specification encoded in the st_other field.  */
#define STV_DEFAULT 0       /* Default symbol visibility rules */
#define STV_INTERNAL    1       /* Processor specific hidden class */
#define STV_HIDDEN  2       /* Sym unavailable in other modules */
#define STV_PROTECTED   3       /* Not preemptible, not exported */

 /* Special value for e_phnum.  This indicates that the real number of
  * program headers is too large to fit into e_phnum.  Instead the real
  * value is in the field sh_info of section 0.  */

#define PN_XNUM     0xffff

 /* Legal values for p_type (segment type).  */

#define PT_NULL     0       /* Program header table entry unused */
#define PT_LOAD     1       /* Loadable program segment */
#define PT_DYNAMIC  2       /* Dynamic linking information */
#define PT_INTERP   3       /* Program interpreter */
#define PT_NOTE     4       /* Auxiliary information */
#define PT_SHLIB    5       /* Reserved */
#define PT_PHDR     6       /* Entry for header table itself */
#define PT_TLS      7       /* Thread-local storage segment */
#define PT_NUM      8       /* Number of defined types */
#define PT_LOOS     0x60000000  /* Start of OS-specific */
#define PT_GNU_EH_FRAME 0x6474e550  /* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK    0x6474e551  /* Indicates stack executability */
#define PT_GNU_RELRO    0x6474e552  /* Read-only after relocation */
#define PT_LOSUNW   0x6ffffffa
#define PT_SUNWBSS  0x6ffffffa  /* Sun Specific segment */
#define PT_SUNWSTACK    0x6ffffffb  /* Stack segment */
#define PT_HISUNW   0x6fffffff
#define PT_HIOS     0x6fffffff  /* End of OS-specific */
#define PT_LOPROC   0x70000000  /* Start of processor-specific */
#define PT_HIPROC   0x7fffffff  /* End of processor-specific */


/*
 * constants for Elf_Phdr.p_flags 
 */
#define PF_X		1	/* readable segment */
#define PF_W		2	/* writeable segment */
#define PF_R		4	/* executable segment */

/*
 * constants for indexing into Elf64_Header_t.e_ident 
 */
#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION	6

#define ELFMAG0         '\177'
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

#define ELFCLASS32      1
#define ELFCLASS64      2

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7
#define PT_NUM 8

#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

/* Section Header type bits */
#define SHT_PROGBITS 1
#define SHT_RELA 4
#define SHT_SYMTAB 2
#define	SHT_NOBITS 8
#define SHT_REL 9


/* Dynamic section entry types */
#define DT_NULL 0
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19


/* Intel 80386 specific definitions.  */

/* i386 relocs.  */

#define R_386_NONE     0        /* No reloc */
#define R_386_32       1        /* Direct 32 bit  */
#define R_386_PC32     2        /* PC relative 32 bit */
#define R_386_GOT32    3        /* 32 bit GOT entry */
#define R_386_PLT32    4        /* 32 bit PLT address */
#define R_386_COPY     5        /* Copy symbol at runtime */
#define R_386_GLOB_DAT     6        /* Create GOT entry */
#define R_386_JMP_SLOT     7        /* Create PLT entry */
#define R_386_RELATIVE     8        /* Adjust by program base */
#define R_386_GOTOFF       9        /* 32 bit offset to GOT */
#define R_386_GOTPC    10       /* 32 bit PC relative offset to GOT */
#define R_386_32PLT    11
#define R_386_TLS_TPOFF    14       /* Offset in static TLS block */
#define R_386_TLS_IE       15       /* Address of GOT entry for static TLS
                                           block offset */
#define R_386_TLS_GOTIE    16       /* GOT entry for static TLS block
                                           offset */
#define R_386_TLS_LE       17       /* Offset relative to static TLS
                                           block */
#define R_386_TLS_GD       18       /* Direct 32 bit for GNU version of
                                           general dynamic thread local data */
#define R_386_TLS_LDM      19       /* Direct 32 bit for GNU version of
                                           local dynamic thread local data
                                               in LE code */
#define R_386_16       20
#define R_386_PC16     21
#define R_386_8        22
#define R_386_PC8      23
#define R_386_TLS_GD_32    24       /* Direct 32 bit for general dynamic
                                           thread local data */
#define R_386_TLS_GD_PUSH  25       /* Tag for pushl in GD TLS code */
#define R_386_TLS_GD_CALL  26       /* Relocation for call to
                                           __tls_get_addr() */
#define R_386_TLS_GD_POP   27       /* Tag for popl in GD TLS code */
#define R_386_TLS_LDM_32   28       /* Direct 32 bit for local dynamic
                                           thread local data in LE code */
#define R_386_TLS_LDM_PUSH 29       /* Tag for pushl in LDM TLS code */
#define R_386_TLS_LDM_CALL 30       /* Relocation for call to
                                           __tls_get_addr() in LDM code */
#define R_386_TLS_LDM_POP  31       /* Tag for popl in LDM TLS code */
#define R_386_TLS_LDO_32   32       /* Offset relative to TLS block */
#define R_386_TLS_IE_32    33       /* GOT entry for negated static TLS
                                           block offset */
#define R_386_TLS_LE_32    34       /* Negated offset relative to static
                                           TLS block */
#define R_386_TLS_DTPMOD32 35       /* ID of module containing symbol */
#define R_386_TLS_DTPOFF32 36       /* Offset in TLS block */
#define R_386_TLS_TPOFF32  37       /* Negated offset in static TLS block */
/* 38? */
#define R_386_TLS_GOTDESC  39       /* GOT offset for TLS descriptor.  */
#define R_386_TLS_DESC_CALL 40      /* Marker of call through TLS
                                           descriptor for
                                               relaxation.  */
#define R_386_TLS_DESC     41       /* TLS descriptor containing
                                           pointer to code and to
                                               argument, returning the TLS
                                                   offset for the symbol.  */
#define R_386_IRELATIVE    42       /* Adjust indirectly by program base */
/* Keep this the last entry.  */
#define R_386_NUM      43




/**/
#define ELF_PRINT_PROGRAM_HEADERS 1
#define ELF_PRINT_SECTIONS 2
#define ELF_PRINT_ALL (ELF_PRINT_PROGRAM_HEADERS | ELF_PRINT_SECTIONS)


#endif
