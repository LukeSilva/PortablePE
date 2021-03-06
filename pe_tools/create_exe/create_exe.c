#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "elf_headers.h"
#include "pe_headers.h"

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))


#define IMAGE_BASE 0x00400000

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s <input elf> <output exe>\n", argv[0]);
    return -1;
  }

  FILE* elf_fp = fopen(argv[1], "rb");
  FILE* exe_fp = fopen(argv[2], "wb");

  if (elf_fp == NULL || exe_fp == NULL) {
    printf("Could not open file\n");
    return -1;
  }

  fseek(elf_fp, 0, SEEK_END);
  long size = ftell(elf_fp);
  rewind(elf_fp);

  char * elf_bin = malloc(size);
  if (elf_bin == NULL) {
    printf("Could not allocate %ld bytes\n", size);
    return -1;
  }

  const long read = fread(elf_bin, 1, size, elf_fp);
  if (read != size) {
    printf("Could not read entire file into memory %ld %ld\n", read, size);
    return -1;
  }


  struct elf_header * header =  (struct elf_header *) elf_bin;

  printf("Magic String:       0x%0x\n", header->magic);
  printf("CPU Width:          %s\n", header->cpu_width == 1 ? "32 bit" : header->cpu_width == 2 ? "64 bit" : "Unknown");
  printf("Endianness:         %s\n", header->endian == 1 ? "Little Endian" : header-> endian == 2 ? "Big Endian" : "Unknown");
  printf("Elf Header Version: %d\n", header->elf_header_version);
  printf("OS ABI:             %d\n", header->os_abi);
  printf("Executable Type:    ");
  switch(header->type) {
    case 1:
      printf("Relocatable\n");
      break;
    case 2:
      printf("Executable\n");
      break;
    case 3:
      printf("Shared\n");
      break;
    default:
      printf("Unknown\n");
  }

  printf("Instruction Set:    0x%04x\n", header->isa);
  printf("Elf Version:        %d\n", header->elf_version);
  printf("Entry Point:        0x%08x\n", header->prog_entry);
  printf("Program header pos: 0x%08x\n", header->prog_header_table_pos);
  printf("Section header pos: 0x%08x\n", header->sect_header_table_pos);
  printf("Header size:        %d\n", header->header_size);
  printf("Section name index: 0x%0hx\n", header->sect_names_idx);
  printf("\n");
  printf("Program header has %d entries, %d bytes each.\n", header->prog_header_entry_count, header->prog_header_entry_size);
  printf("Section header has %d entries, %d bytes each.\n", header->sect_header_entry_count, header->sect_header_entry_size);


  if (header->sect_header_entry_size != sizeof(struct elf_sect_header)) {
    printf("Section header entry size unexpected\n");
    return -1;
  }

  struct elf_sect_header * sect_header = (struct elf_sect_header *) (elf_bin + header->sect_header_table_pos);
  char * name_table = elf_bin + sect_header[header->sect_names_idx].offset;


  struct elf_sect_header * text_section = NULL;
  struct elf_sect_header * data_section = NULL;
  struct elf_sect_header * win_imports_section = NULL;
  
  printf("\nSections:\n");
  printf("Num\tFile offset\tVirt Address\tName\n");
  for (int i = 0; i < header->sect_header_entry_count; ++i) {
    const char * name = name_table + sect_header[i].name;
    printf("%d\t0x%08x\t0x%08x\t%s\n", i, sect_header[i].offset, sect_header[i].addr, name);  
    if (strcmp(name, ".text") == 0) text_section = &sect_header[i];
    else if (strcmp(name, ".data") == 0) data_section = &sect_header[i];
    else if (strcmp(name, ".win_imports") == 0) win_imports_section = &sect_header[i];
  }

  printf("\n");

  if (text_section == NULL) { printf("Could not find .text section\n"); return -1;}
  if (data_section == NULL) { printf("Could not find .data section\n"); return -1;}
  if (win_imports_section == NULL) { printf("Could not find .win_imports section\n"); return -1;}


  const long ELF_POSITION = 0x2000; // Todo calculate this

  struct Pe32Header * pe_header = calloc(sizeof(struct Pe32Header), 1);
  pe_header->mMagic = 0x00004550;
  pe_header->mMachine = 0x14c;
  pe_header->mNumberOfSections = 3;
  pe_header->mSizeOfOptionalHeader = sizeof(struct Pe32OptionalHeader) + sizeof(struct Pe32DataDirectories);
  pe_header->mCharacteristics = 0x102;

  char * win_imports_bin = elf_bin + win_imports_section->offset;
  if (win_imports_bin[0] != 'P' || win_imports_bin[1] != 'E' || win_imports_bin[2] != 0 || win_imports_bin[3] != 0) {
    printf(".win_imports section magic / version mismatch\n");
    return -1;
  }
  const uint32_t win_entry = ((uint32_t*) win_imports_bin)[1];

  struct Pe32OptionalHeader * pe_opt_header = &pe_header->optHeader;
  pe_opt_header->mMagic = 0x10b;
  // Todo get from mapfile
  pe_opt_header->mAddressOfEntryPoint = win_entry - IMAGE_BASE;
  pe_opt_header->mImageBase = IMAGE_BASE;
  pe_opt_header->mSectionAlignment = 0x1000;
  pe_opt_header->mFileAlignment = 0x200;
  pe_opt_header->mMajorSubsystemVersion = 4;
  pe_opt_header->mSizeOfImage = 0x6000;// ELF_POSITION + size; // FIXME IDK LOL
  pe_opt_header->mSizeOfHeaders = ROUND_UP(sizeof (struct DosHeader ) + sizeof(struct Pe32Header) + 3 * (sizeof(struct Pe32SectionHeader)), 0x200);
  pe_opt_header->mSubsystem = 3;
  pe_opt_header->mNumberOfRvaAndSizes = 16;

  struct Pe32DataDirectories * pe_data_dirs = &pe_header->dataDirectories;
  pe_data_dirs->ImportTable.address = win_imports_section->offset + 8;
  
  struct Pe32SectionHeader * pe_text_section = calloc(sizeof(struct Pe32SectionHeader), 1);
  strcpy((char*)&pe_text_section->mName, ".text");
  pe_text_section->mVirtualSize = text_section->size;
  pe_text_section->mVirtualAddress = text_section->addr - IMAGE_BASE;
  pe_text_section->mSizeOfRawData = text_section->size;
  pe_text_section->mPointerToRawData = text_section->offset + ELF_POSITION;
  pe_text_section->mCharacteristics = PE_SECT_FLAG_CODE | PE_SECT_FLAG_EXECUTE | PE_SECT_FLAG_READ;
  
  struct Pe32SectionHeader * pe_data_section = calloc(sizeof(struct Pe32SectionHeader), 1);
  strcpy((char*)&pe_data_section->mName, ".data");
  pe_data_section->mVirtualSize = data_section->size;
  pe_data_section->mVirtualAddress = data_section->addr - IMAGE_BASE;
  pe_data_section->mSizeOfRawData = data_section->size;
  pe_data_section->mPointerToRawData = data_section->offset + ELF_POSITION;
  pe_data_section->mCharacteristics = PE_SECT_FLAG_DATA | PE_SECT_FLAG_WRITE | PE_SECT_FLAG_READ;
  
  struct Pe32SectionHeader * pe_win_imports_section = calloc(sizeof(struct Pe32SectionHeader), 1);
  strcpy((char*)&pe_win_imports_section->mName, ".rdata");
  pe_win_imports_section->mVirtualSize = win_imports_section->size;
  pe_win_imports_section->mVirtualAddress = win_imports_section->addr - IMAGE_BASE;
  pe_win_imports_section->mSizeOfRawData = win_imports_section->size;
  pe_win_imports_section->mPointerToRawData = win_imports_section->offset + ELF_POSITION;
  pe_win_imports_section->mCharacteristics = PE_SECT_FLAG_DATA | PE_SECT_FLAG_READ;
  
  

  struct DosHeader * dos = calloc(sizeof(struct DosHeader), 1);
  dos->stub[0] = 'M';
  dos->stub[1] = 'Z';
  strcpy((char*)&dos->stub, "MZ=1\nS=`tail -c+64 $0|head|tr -d '\\0'`\nsh -c \"i=$0;$S\"\nexit\n");
  strcpy((char*)&dos->bootstrap, "dd if=$i bs=1K skip=8 of=$i.elf 2>/dev/null\nchmod a+x $i.elf\n./$i.elf\nexit\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  dos->PEHeaderOffset = 256;


  fseek(exe_fp, 0, SEEK_SET);
  fwrite(dos, sizeof(struct DosHeader), 1, exe_fp);
  fwrite(pe_header, sizeof(struct Pe32Header), 1, exe_fp);
  fwrite(pe_text_section, sizeof(struct Pe32SectionHeader), 1, exe_fp);
  fwrite(pe_data_section, sizeof(struct Pe32SectionHeader), 1, exe_fp);
  fwrite(pe_win_imports_section, sizeof(struct Pe32SectionHeader), 1, exe_fp);
  fseek(exe_fp, 0x2000, SEEK_SET);
  fwrite(elf_bin, 1, size, exe_fp);




  fclose(elf_fp);
  fclose(exe_fp);

}