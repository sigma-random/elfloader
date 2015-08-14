#ifndef __ELF32_H__
#define __ELF32_H__


#include <stdint.h>

 /* Type for a 16-bit quantity.  */
typedef uint16_t Elf32_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef uint64_t Elf32_Xword;
typedef int64_t  Elf32_Sxword;

/* Type of addresses.  */
typedef uint32_t Elf32_Addr;

/* Type of file offsets.  */
typedef uint32_t Elf32_Off;

/* Type for section indices, which are 16-bit quantities.  */
typedef uint16_t Elf32_Section;

/* Type for version symbol information.  */
typedef Elf32_Half Elf32_Versym;


/* The ELF file header.  This appears at the start of every ELF file.  */

#define EI_NIDENT (16)
typedef struct
{
    unsigned char   e_ident[EI_NIDENT]; /* Magic number and other info */
    Elf32_Half  e_type;         /* Object file type */
    Elf32_Half  e_machine;      /* Architecture */
    Elf32_Word  e_version;      /* Object file version */
    Elf32_Addr  e_entry;        /* Entry point virtual address */
    Elf32_Off   e_phoff;        /* Program header table file offset */
    Elf32_Off   e_shoff;        /* Section header table file offset */
    Elf32_Word  e_flags;        /* Processor-specific flags */
    Elf32_Half  e_ehsize;       /* ELF header size in bytes */
    Elf32_Half  e_phentsize;        /* Program header table entry size */
    Elf32_Half  e_phnum;        /* Program header table entry count */
    Elf32_Half  e_shentsize;        /* Section header table entry size */
    Elf32_Half  e_shnum;        /* Section header table entry count */
    Elf32_Half  e_shstrndx;     /* Section header string table index */
} Elf32_Ehdr;


/* Section header.  */

typedef struct
{
    Elf32_Word  sh_name;        /* Section name (string tbl index) */
    Elf32_Word  sh_type;        /* Section type */
    Elf32_Word  sh_flags;       /* Section flags */
    Elf32_Addr  sh_addr;        /* Section virtual addr at execution */
    Elf32_Off   sh_offset;      /* Section file offset */
    Elf32_Word  sh_size;        /* Section size in bytes */
    Elf32_Word  sh_link;        /* Link to another section */
    Elf32_Word  sh_info;        /* Additional section information */
    Elf32_Word  sh_addralign;       /* Section alignment */
    Elf32_Word  sh_entsize;     /* Entry size if section holds table */
} Elf32_Shdr;


/* Symbol table entry.  */

typedef struct
{
    Elf32_Word  st_name;        /* Symbol name (string tbl index) */
    Elf32_Addr  st_value;       /* Symbol value */
    Elf32_Word  st_size;        /* Symbol size */
    unsigned char   st_info;        /* Symbol type and binding */
    unsigned char   st_other;       /* Symbol visibility */
    Elf32_Section   st_shndx;       /* Section index */
} Elf32_Sym;

/* The syminfo section if available contains additional information about
 * every dynamic symbol.  */

typedef struct
{
    Elf32_Half si_boundto;      /* Direct bindings, symbol bound to */
    Elf32_Half si_flags;            /* Per symbol flags */
} Elf32_Syminfo;

/* How to extract and insert information held in the st_info field.  */

#define ELF32_ST_BIND(val)      (((unsigned char) (val)) >> 4)
#define ELF32_ST_TYPE(val)      ((val) & 0xf)
#define ELF32_ST_INFO(bind, type)   (((bind) << 4) + ((type) & 0xf))

/* How to extract and insert information held in the st_other field.  */
#define ELF32_ST_VISIBILITY(o)  ((o) & 0x03)


/* Relocation table entry without addend (in section of type SHT_REL).  */

typedef struct
{
    Elf32_Addr  r_offset;       /* Address */
    Elf32_Word  r_info;         /* Relocation type and symbol index */
} Elf32_Rel;

/* I have seen two different definitions of the Elf64_Rel and
 * Elf64_Rela structures, so we'll leave them out until Novell (or
 * whoever) gets their act together.  */
/* The following, at least, is used on Sparc v9, MIPS, and Alpha.  */

/* Relocation table entry with addend (in section of type SHT_RELA).  */

typedef struct
{
    Elf32_Addr  r_offset;       /* Address */
    Elf32_Word  r_info;         /* Relocation type and symbol index */
    Elf32_Sword r_addend;       /* Addend */
} Elf32_Rela;

/* How to extract and insert information held in the r_info field.  */

#define ELF32_R_SYM(val)    ((val) >> 8)
#define ELF32_R_TYPE(val)       ((val) & 0xff)
#define ELF32_R_INFO(sym, type)     (((sym) << 8) + ((type) & 0xff))


/* Program segment header.  */

typedef struct
{
    Elf32_Word  p_type;         /* Segment type */
    Elf32_Off   p_offset;       /* Segment file offset */
    Elf32_Addr  p_vaddr;        /* Segment virtual address */
    Elf32_Addr  p_paddr;        /* Segment physical address */
    Elf32_Word  p_filesz;       /* Segment size in file */
    Elf32_Word  p_memsz;        /* Segment size in memory */
    Elf32_Word  p_flags;        /* Segment flags */
    Elf32_Word  p_align;        /* Segment alignment */
} Elf32_Phdr;

/* Dynamic section entry.  */

typedef struct
{
    Elf32_Sword d_tag;          /* Dynamic entry type */
    union
    {
        Elf32_Word d_val;           /* Integer value */
        Elf32_Addr d_ptr;           /* Address value */
    } d_un;
} Elf32_Dyn;

//============================================================//
int elf32_checkFile(Elf32_Ehdr *file);
Elf32_Phdr * elf32_getProgramSegmentTable(Elf32_Ehdr *file);
unsigned elf32_getNumSections(Elf32_Ehdr *file);
char * elf32_getStringTable(Elf32_Ehdr *file);
char * elf32_getSegmentStringTable(Elf32_Ehdr *file);

static inline Elf32_Shdr *
elf32_getSectionTable(Elf32_Ehdr *file)
{
	/* Cast heaven! */
	return (Elf32_Shdr*) (uintptr_t) (((uintptr_t) file) + file->e_shoff);
}

/* accessor functions */
static inline uint32_t 
elf32_getSectionType(Elf32_Ehdr *file, uint16_t s)
{
	return elf32_getSectionTable(file)[s].sh_type;
}

static inline uint32_t 
elf32_getSectionFlags(Elf32_Ehdr *file, uint16_t s)
{
	return elf32_getSectionTable(file)[s].sh_flags;
}

char * elf32_getSectionName(Elf32_Ehdr *file, int i);

uint32_t elf32_getSectionSize(Elf32_Ehdr *file, int i);

uint32_t elf32_getSectionAddr(Elf32_Ehdr *elfFile, int i);

void * elf32_getSection(Elf32_Ehdr *file, int i);

void * elf32_getSectionNamed(Elf32_Ehdr *file, char *str);

int elf32_getSegmentType (Elf32_Ehdr *file, int segment);

void elf32_getSegmentInfo(Elf32_Ehdr *file, int segment, uint64_t *p_vaddr, 
			  uint64_t *p_paddr, uint64_t *p_filesz, 
			  uint64_t *p_offset, uint64_t *p_memsz);

uint32_t elf32_getEntryPoint (Elf32_Ehdr *file);

/* Program header functions */
uint16_t elf32_getNumProgramHeaders(Elf32_Ehdr *file);

static inline Elf32_Phdr *
elf32_getProgramHeaderTable(Elf32_Ehdr *file)
{
	/* Cast heaven! */
	return (Elf32_Phdr*) (uintptr_t) (((uintptr_t) file) + file->e_phoff);
}

/* accessor functions */
static inline uint32_t 
elf32_getProgramHeaderFlags(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_flags;
}

static inline uint32_t 
elf32_getProgramHeaderType(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_type;
}

static inline uint32_t 
elf32_getProgramHeaderFileSize(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_filesz;
}

static inline uint32_t
elf32_getProgramHeaderMemorySize(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_memsz;
}

static inline uint32_t
elf32_getProgramHeaderVaddr(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_vaddr;
}

static inline uint32_t
elf32_getProgramHeaderPaddr(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_paddr;
}

static inline uint32_t
elf32_getProgramHeaderOffset(Elf32_Ehdr *file, uint16_t ph)
{
	return elf32_getProgramHeaderTable(file)[ph].p_offset;
}

#endif /* __ELF_ELF_32_H__ */
