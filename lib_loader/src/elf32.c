#include <elf/elf.h>
#include <string.h>
#include <inttypes.h>

int
elf32_checkFile(Elf32_Ehdr *file)
{
	if (file->e_ident[EI_MAG0] != ELFMAG0
	    || file->e_ident[EI_MAG1] != ELFMAG1
	    || file->e_ident[EI_MAG2] != ELFMAG2
	    || file->e_ident[EI_MAG3] != ELFMAG3)
		return -1;	/* not an elf file */
	if (file->e_ident[EI_CLASS] != ELFCLASS32)
		return -2;	/* not 32-bit file */
	return 0;		/* elf file looks OK */
}

/*
 * Returns the number of program segments in this elf file. 
 */
unsigned
elf32_getNumSections(Elf32_Ehdr *elfFile)
{
	return elfFile->e_shnum;
}

char *
elf32_getStringTable(Elf32_Ehdr *elfFile)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	return (char *)elfFile + sections[elfFile->e_shstrndx].sh_offset;
}

/* Returns a pointer to the program segment table, which is an array of
 * ELF32_Phdr_t structs.  The size of the array can be found by calling
 * getNumProgramSegments. */
Elf32_Phdr *
elf32_getProgramSegmentTable(Elf32_Ehdr *elfFile)
{
	Elf32_Ehdr *fileHdr = elfFile;
	return (Elf32_Phdr *) (fileHdr->e_phoff + (long) elfFile);
}

/* Returns the number of program segments in this elf file. */
uint16_t
elf32_getNumProgramHeaders(Elf32_Ehdr *elfFile)
{
	Elf32_Ehdr *fileHdr = elfFile;
	return fileHdr->e_phnum;
}

char *
elf32_getSectionName(Elf32_Ehdr *elfFile, int i)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	char *str_table = elf32_getSegmentStringTable(elfFile);
	if (str_table == NULL) {
		return "<corrupted>";
	} else {
		return str_table + sections[i].sh_name;
	}
}

uint32_t
elf32_getSectionSize(Elf32_Ehdr *elfFile, int i)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	return sections[i].sh_size;
}

uint32_t
elf32_getSectionAddr(Elf32_Ehdr *elfFile, int i)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	return sections[i].sh_addr;
}

uint32_t
elf32_getSectioniVirtulAddr(Elf32_Ehdr *elfFile, int i)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	return (uint32_t)elfFile + sections[i].sh_addr;
}
	
void *
elf32_getSection(Elf32_Ehdr *elfFile, int i)
{
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	return (char *)elfFile + sections[i].sh_offset;
}

void *
elf32_getSectionNamed(Elf32_Ehdr *elfFile, char *str)
{
	int numSections = elf32_getNumSections(elfFile);
	int i;
	for (i = 0; i < numSections; i++) {
		if (strcmp(str, elf32_getSectionName(elfFile, i)) == 0) {
			return elf32_getSection(elfFile, i);
		}
	}
	return NULL;
}

char *
elf32_getSegmentStringTable(Elf32_Ehdr *elfFile)
{
	Elf32_Ehdr *fileHdr = (Elf32_Ehdr *) elfFile;
	if (fileHdr->e_shstrndx == 0) {
		return NULL;
	} else {
		return elf32_getStringTable(elfFile);
	}
}

#ifdef ELF_DEBUG
void
elf32_printStringTable(Elf32_Ehdr *elfFile)
{
	int counter;
	Elf32_Shdr *sections = elf32_getSectionTable(elfFile);
	char * stringTable;

	if (!sections) {
		printf("No sections.\n");
		return;
	}
	
	stringTable = ((void *)elfFile) + sections[elfFile->e_shstrndx].sh_offset;
	
	printf("File is %p; sections is %p; string table is %p\n", elfFile, sections, stringTable);

	for (counter=0; counter < sections[elfFile->e_shstrndx].sh_size; counter++) {
		printf("%02x %c ", stringTable[counter], 
				stringTable[counter] >= 0x20 ? stringTable[counter] : '.');
	}
}
#endif

int
elf32_getSegmentType (Elf32_Ehdr *elfFile, int segment)
{
	return elf32_getProgramSegmentTable(elfFile)[segment].p_type;
}

void
elf32_getSegmentInfo(Elf32_Ehdr *elfFile, int segment, uint64_t *p_vaddr, uint64_t *p_addr, uint64_t *p_filesz, uint64_t *p_offset, uint64_t *p_memsz)
{
	Elf32_Phdr *segments;
		
	segments = elf32_getProgramSegmentTable(elfFile);
	*p_addr = segments[segment].p_paddr;
	*p_vaddr = segments[segment].p_vaddr;
	*p_filesz = segments[segment].p_filesz;
	*p_offset = segments[segment].p_offset;
	*p_memsz = segments[segment].p_memsz;
}

uint32_t
elf32_getEntryPoint (Elf32_Ehdr *elfFile)
{
	return elfFile->e_entry;
}

/*
 * Debugging functions 
 */

/*
 * prints out some details of one elf file 
 */
void
elf32_fprintf(FILE *f, Elf32_Ehdr *file, int size, const char *name, int flags)
{
	Elf32_Phdr *segments;
	unsigned numSegments;
	Elf32_Shdr *sections;
	unsigned numSections;
	int i, r;
	// char *str_table;

	fprintf(f, "Found an elf32 file called \"%s\" located "
		"at address %p\n", name, file);

	if ((r = elf32_checkFile(file)) != 0) {
		char *magic = (char*) file;
		fprintf(f, "Invalid elf file (%d)\n", r);
		fprintf(f, "Magic is: %2.2hhx %2.2hhx %2.2hhx %2.2hhx\n",
			magic[0], magic[1], magic[2], magic[3]);
		return;
	}


	/*
	 * get a pointer to the table of program segments 
	 */
	segments = elf32_getProgramHeaderTable(file);
	numSegments = elf32_getNumProgramHeaders(file);

	sections = elf32_getSectionTable(file);
	numSections = elf32_getNumSections(file);

	if ((uintptr_t) sections >  ((uintptr_t) file + size)) {
		fprintf(f, "Corrupted elfFile..\n");
		return;
	}

		/*
		 * print out info about each section 
		 */
		
	if (flags & ELF_PRINT_PROGRAM_HEADERS) {
		/*
		 * print out info about each program segment 
		 */
		fprintf(f, "Program Headers:\n");
		fprintf(f, "  Type           Offset   VirtAddr   PhysAddr   "
			"FileSiz MemSiz  Flg Align\n");
		for (i = 0; i < numSegments; i++) {
			if (segments[i].p_type != 1) {
				fprintf(f, "segment %d is not loadable, "
					"skipping\n", i);
			} else {
				fprintf(f, "  LOAD           0x%06" PRIx32 " 0x%08" PRIx32 " 0x%08" PRIx32  \
				       " 0x%05" PRIx32" 0x%05" PRIx32 " %c%c%c 0x%04" PRIx32 "\n",
				//fprintf(f, "  LOAD           0x%" PRIxPTR " 0x%" PRIxPTR " 0x%" PRIxPTR 
				//	" 0x%" PRIxPTR" 0x%" PRIxPTR " %c%c%c 0x%" PRIxPTR "\n",
					segments[i].p_offset, segments[i].p_vaddr,
					segments[i].p_paddr,
					segments[i].p_filesz, segments[i].p_memsz,
					segments[i].p_flags & PF_R ? 'R' : ' ',
					segments[i].p_flags & PF_W ? 'W' : ' ',
					segments[i].p_flags & PF_X ? 'E' : ' ',
					segments[i].p_align);
			}
		}
	}
	if (flags & ELF_PRINT_SECTIONS) {
		// str_table = elf32_getSegmentStringTable(file);

		printf("Section Headers:\n");
		printf("  [Nr] Name              Type            Addr     Off\n");
		for (i = 0; i < numSections; i++) {
			//if (elf_checkSection(file, i) == 0) {
			fprintf(f, "[%2d] %s %"PRIx32" %"PRIx32"\n", i, elf32_getSectionName(file, i),
				//fprintf(f, "[%2d] %-17.17s %-15.15s %x %x\n", i, elf32_getSectionName(file, i), " ", 
				///fprintf(f, "%-17.17s %-15.15s %08x %06x\n", elf32_getSectionName(file, i), " "	/* sections[i].sh_type 
				//									 */ ,
				sections[i].sh_addr, sections[i].sh_offset);
			//}
		}
	}
}



//append ...

Elf32_Word 
elf32_getMappingSize(Elf32_Ehdr *elfFile) {

    Elf32_Half ph_num = 0, i = 0;
    Elf32_Word m_size = 0;               // total aligned size of all segements  with PT_LOAD flag
    
    Elf32_Phdr *elf_phdr = elf32_getProgramSegmentTable(elfFile);
    ph_num = elf32_getNumProgramHeaders(elfFile);
    //printf("  ::Program Header Table::\n");
    for( i=0; i<ph_num; i++ ) { 
        //printf("[%04d] 0x%08x 0x%08x\n", i, elf_phdr[i].p_memsz, elf_phdr[i].p_align);
        if( elf_phdr[i].p_type == PT_LOAD ) { 
            m_size += ALIGN_SIZE( (elf_phdr[i].p_offset % elf_phdr[i].p_align + elf_phdr[i].p_filesz),
                                   elf_phdr[i].p_align );
        }   
    } 
    return m_size;
}


