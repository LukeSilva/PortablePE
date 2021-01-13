
struct Pe32DataDirectory {
  uint32_t address;
  uint32_t size;
};

struct Pe32DataDirectories {
  struct Pe32DataDirectory ExportTable;
  struct Pe32DataDirectory ImportTable;
  struct Pe32DataDirectory ResourceTable;
  struct Pe32DataDirectory ExceptionTable;
  struct Pe32DataDirectory CertificateTable;
  struct Pe32DataDirectory BaseRelocationTable;
  struct Pe32DataDirectory Debug;
  struct Pe32DataDirectory Architecture;
  struct Pe32DataDirectory GlobalPtr;
  struct Pe32DataDirectory TLSTable;
  struct Pe32DataDirectory LoadConfigTable;
  struct Pe32DataDirectory BoundImport;
  struct Pe32DataDirectory ImportAddressTable;
  struct Pe32DataDirectory DelayImportDescriptor;
  struct Pe32DataDirectory CLRRuntimeHeader;
  struct Pe32DataDirectory Reserved;
}; 

struct Pe32OptionalHeader {
	uint16_t mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	uint8_t  mMajorLinkerVersion;
	uint8_t  mMinorLinkerVersion;
	uint32_t mSizeOfCode;
	uint32_t mSizeOfInitializedData;
	uint32_t mSizeOfUninitializedData;
	uint32_t mAddressOfEntryPoint;
	uint32_t mBaseOfCode;
	uint32_t mBaseOfData;
	uint32_t mImageBase;
	uint32_t mSectionAlignment;
	uint32_t mFileAlignment;
	uint16_t mMajorOperatingSystemVersion;
	uint16_t mMinorOperatingSystemVersion;
	uint16_t mMajorImageVersion;
	uint16_t mMinorImageVersion;
	uint16_t mMajorSubsystemVersion;
	uint16_t mMinorSubsystemVersion;
	uint32_t mWin32VersionValue;
	uint32_t mSizeOfImage;
	uint32_t mSizeOfHeaders;
	uint32_t mCheckSum;
	uint16_t mSubsystem;
	uint16_t mDllCharacteristics;
	uint32_t mSizeOfStackReserve;
	uint32_t mSizeOfStackCommit;
	uint32_t mSizeOfHeapReserve;
	uint32_t mSizeOfHeapCommit;
	uint32_t mLoaderFlags;
	uint32_t mNumberOfRvaAndSizes;
};

/* PE Headers are from wiki.osdev.org */
// 1 byte aligned
struct Pe32Header {
	uint32_t mMagic; // PE\0\0 or 0x00004550
	uint16_t mMachine;
	uint16_t mNumberOfSections;
	uint32_t mTimeDateStamp;
	uint32_t mPointerToSymbolTable;
	uint32_t mNumberOfSymbols;
	uint16_t mSizeOfOptionalHeader;
	uint16_t mCharacteristics;
  
  struct Pe32OptionalHeader optHeader;
  struct Pe32DataDirectories dataDirectories;
};


struct Pe32SectionHeader { // size 40 bytes
	char mName[8];
	uint32_t mVirtualSize;
	uint32_t mVirtualAddress;
	uint32_t mSizeOfRawData;
	uint32_t mPointerToRawData;
	uint32_t mPointerToRelocations;
	uint32_t mPointerToLinenumbers;
	uint16_t mNumberOfRelocations;
	uint16_t mNumberOfLinenumbers;
	uint32_t mCharacteristics;
};

struct DosHeader {
  char stub[60];
  uint32_t PEHeaderOffset;
  char bootstrap[192];
};

#define PE_SECT_FLAG_CODE     0x00000020 
#define PE_SECT_FLAG_DATA     0x00000040
#define PE_SECT_FLAG_BSS      0x00000080 
#define PE_SECT_FLAG_EXECUTE  0x20000000
#define PE_SECT_FLAG_READ     0x40000000
#define PE_SECT_FLAG_WRITE    0x80000000