
OUTPUT_FORMAT("elf32-i386", "elf32-i386",
              "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

PHDRS
{
  text PT_LOAD FILEHDR PHDRS ;
  data PT_LOAD ;
}

SECTIONS
{
  . = 0x00400000;
  /* Allow the text section to be in the first page of the file */
  . += SIZEOF_HEADERS; 
  .text ALIGN(0x1000): { 
    *(.text) 
    . = ALIGN(0x1000);
  } :text
  /* Move to the next page in memory. Align will create a code gap */
  .data ALIGN(0x1000) : {
    *(.data)
    . = ALIGN(0x1000);
  } :data
  .win_imports ALIGN(0x1000) : {
    *(.win_imports)
    . = ALIGN(0x1000);
  } :NONE
  .bss : { *(.bss) }
}