
struct elf_header {
  uint32_t magic;
  uint8_t cpu_width;
  uint8_t endian;
  uint8_t elf_header_version;
  uint8_t os_abi;
  uint32_t padding[2];
  uint16_t type;
  uint16_t isa;
  uint32_t elf_version;
  uint32_t prog_entry;
  uint32_t prog_header_table_pos;
  uint32_t sect_header_table_pos;
  uint32_t flags;
  uint16_t header_size;
  uint16_t prog_header_entry_size;
  uint16_t prog_header_entry_count;
  uint16_t sect_header_entry_size;
  uint16_t sect_header_entry_count;
  uint16_t sect_names_idx;
};

struct elf_sect_header {
  uint32_t name;
  uint32_t type;
  uint32_t flags;
  uint32_t addr;
  uint32_t offset;
  uint32_t size;
  uint32_t link;
  uint32_t info;
  uint32_t addr_align;
  uint32_t entsize;
};