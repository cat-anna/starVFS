#pragma once

namespace StarVFS {
namespace RDC {
namespace Headers {
namespace Version_1 {

using SectionIndex = u8;
using SectionPointer = u32;
using FilePointer = u32;
using FileIndex = u32;
using Size = u32;

struct CompressionInfo {
	CompressionMode Mode = CompressionMode::None;
	u8 unused_8_0 = 0;
	u8 unused_8_1 = 0;
	u8 unused_8_2 = 0;
	u32 RawSize = 0;
};
static_assert(sizeof(CompressionInfo) == 8, "DataBlock has invalid size!");

struct EncryptionInfo {
	EncryptionMode Mode = EncryptionMode::None;
	u8 unused_8_0 = 0;
	u8 unused_8_1 = 0;
	u8 unused_8_2 = 0;
};
static_assert(sizeof(EncryptionInfo) == 4, "DataBlock has invalid size!");

struct CheckSumInfo {
	CheckSumMode Mode = CheckSumMode::None;
	u8 unused_8_0 = 0;
	u8 unused_8_1 = 0;
	u8 unused_8_2 = 0;
};
static_assert(sizeof(CheckSumInfo) == 4, "DataBlock has invalid size!");

struct DataBlock {
	Size ContainerSize = 0;
	FilePointer FilePointer = 0;
	CompressionInfo Compression;
	EncryptionInfo Encryption;
	CheckSumInfo CheckSum;

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(DataBlock) % 8 == 0, "DataBlock has invalid size!");

struct FileFooter {
	DataBlock SectionsBlock;
	SectionIndex SectionCount = 0; ///Count of sections
	u8 unused_8_0 = 0;
	u8 unused_8_1 = 0;
	u8 unused_8_2 = 0;
	Signature Signature = Signature::Footer; ///Footer signature

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(FileFooter) % 8 == 0, "FileFooter has invalid size!");

struct SectionDescriptor {
	DataBlock SectionBlock;
	/** Section type */
	SectionType Type = SectionType::EmptyEntry;
	u8 unused_8_0 = 0;
	u8 unused_8_1 = 0;
	u8 unused_8_2 = 0;
	u32 unused_32_0 = 0;

	void Zero() { memset(this, 0, sizeof(*this)); }
};
static_assert(sizeof(SectionDescriptor) % 8 == 0, "SectionDescriptor has invalid size!");

#if 0

struct Version {
	enum {
		Value = 1,
	};
};

//------------ file table section ------------

struct FileTableEntryFlags {
	enum {
		Folder			= 0x0001,	/** Entry describes directory. No data, and ChildrenCount contain valid value */
		SymLink			= 0x0100,	/** File data is shared between multiple files */
		HasSymLink		= 0x0200,	/** file has symlinks pointing to it */
		InvalidEntry	= 0x1000,
	};
};

struct FileTableEntry {
	/** File data block */
	DataBlock FileBlock;
	/** File name offset in string table */
	SectionPointer NamePointer;
	/** parent file index */
	FileIndex ParentIndex;
	/** Symlink file index */
	FileIndex SymLinkIndex;
	/** FileFlags */
	u16 Flags;

	u16 unused_16_0 = 0;
	u64 unused_64_0 = 0;
};

struct FileTableSection {
	/** Index of data section */
	SectionIndex RawDataSection;
	/** Index of String table section */
	SectionIndex StringTableSection;
	/** Some flags. Currently unused */
	u16 Flags;
	/** Elements in table */
	u32 Count;
	/** Space for future use */
	u64 unused_64_0;
	/** The Table */
	FileTableEntry Table[0];
};

//------------ raw data section ------------

struct RawDataSection {
	/** The Table */
	u8 Table[0];
};

//------------ string table section ------------

struct StringTableSection {
	/** The Table */
	char Table[0];
};

#endif

} //namespace Version_1
} //namespace Headers 
} //namespace RDC 
} //namespace StarVFS 
